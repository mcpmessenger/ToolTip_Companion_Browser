// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "element_detector.h"

#include "base/functional/bind.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/values.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/geometry/point.h"

namespace tooltip {

// Static member definitions
const std::vector<std::string> ElementDetector::kInteractiveTags = {
    "button", "a", "input", "select", "textarea", "label", "area", "details",
    "summary", "iframe", "video", "audio", "canvas", "svg", "object", "embed"
};

const std::vector<std::string> ElementDetector::kInteractiveAttributes = {
    "onclick", "onmousedown", "onmouseup", "onmouseover", "onmouseout",
    "role", "tabindex", "data-action", "data-toggle", "data-target",
    "href", "src", "action", "method"
};

const std::vector<std::string> ElementDetector::kInteractiveClasses = {
    "btn", "button", "link", "clickable", "interactive", "menu-item",
    "nav-item", "tab", "card", "tile", "dropdown", "modal", "popup"
};

ElementDetector::ElementDetector()
    : initialized_(false), monitoring_(false) {}

ElementDetector::~ElementDetector() = default;

void ElementDetector::Initialize() {
  if (initialized_) {
    return;
  }

  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  initialized_ = true;
  
  VLOG(1) << "ElementDetector initialized";
}

void ElementDetector::StartMonitoring(content::WebContents* web_contents) {
  if (!initialized_) {
    Initialize();
  }

  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  
  Observe(web_contents);
  monitoring_ = true;
  
  VLOG(1) << "ElementDetector started monitoring web contents";
}

void ElementDetector::StopMonitoring() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  
  Observe(nullptr);
  monitoring_ = false;
  
  VLOG(1) << "ElementDetector stopped monitoring";
}

bool ElementDetector::IsInteractiveElement(const std::string& tag_name,
                                          const std::string& attributes,
                                          const std::string& class_name) const {
  // Check for interactive tags
  std::string lower_tag = base::ToLowerASCII(tag_name);
  for (const auto& interactive_tag : kInteractiveTags) {
    if (lower_tag == interactive_tag) {
      return true;
    }
  }

  // Check for interactive attributes
  std::string lower_attributes = base::ToLowerASCII(attributes);
  for (const auto& interactive_attr : kInteractiveAttributes) {
    if (lower_attributes.find(interactive_attr) != std::string::npos) {
      return true;
    }
  }

  // Check for interactive classes
  std::string lower_class = base::ToLowerASCII(class_name);
  for (const auto& interactive_class : kInteractiveClasses) {
    if (lower_class.find(interactive_class) != std::string::npos) {
      return true;
    }
  }

  return false;
}

void ElementDetector::GetElementAtPoint(
    content::WebContents* web_contents,
    const gfx::Point& point,
    base::OnceCallback<void(const ElementInfo&)> callback) {
  
  if (!web_contents) {
    std::move(callback).Run(ElementInfo());
    return;
  }

  // JavaScript code to get element information at a point
  std::string script = R"(
    (function() {
      const element = document.elementFromPoint()" + std::to_string(point.x()) + 
      ", " + std::to_string(point.y()) + R"();
      
      if (!element) {
        return null;
      }
      
      const rect = element.getBoundingClientRect();
      const computedStyle = window.getComputedStyle(element);
      
      return {
        tagName: element.tagName.toLowerCase(),
        id: element.id || '',
        className: element.className || '',
        textContent: element.textContent ? element.textContent.trim().substring(0, 100) : '',
        href: element.href || '',
        src: element.src || '',
        alt: element.alt || '',
        title: element.title || '',
        role: element.getAttribute('role') || '',
        ariaLabel: element.getAttribute('aria-label') || '',
        bounds: {
          x: Math.round(rect.left),
          y: Math.round(rect.top),
          width: Math.round(rect.width),
          height: Math.round(rect.height)
        },
        styles: {
          backgroundColor: computedStyle.backgroundColor,
          color: computedStyle.color,
          fontSize: computedStyle.fontSize,
          fontFamily: computedStyle.fontFamily
        }
      };
    })();
  )";

  // Execute JavaScript and get result
  web_contents->GetMainFrame()->ExecuteJavaScript(
      base::UTF8ToUTF16(script),
      base::BindOnce(&ElementDetector::OnElementInfoReceived,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void ElementDetector::OnElementInfoReceived(
    base::OnceCallback<void(const ElementInfo&)> callback,
    base::Value result) {
  
  ElementInfo element_info;
  
  if (result.is_dict()) {
    const base::DictionaryValue* dict = nullptr;
    if (result.GetAsDictionary(&dict)) {
      ExtractElementInfo(dict, &element_info);
    }
  }
  
  std::move(callback).Run(element_info);
}

void ElementDetector::ExtractElementInfo(const base::DictionaryValue* dict,
                                        ElementInfo* element_info) {
  if (!dict || !element_info) {
    return;
  }

  // Extract basic properties
  dict->GetString("tagName", &element_info->tag_name);
  dict->GetString("id", &element_info->id);
  dict->GetString("className", &element_info->class_name);
  dict->GetString("textContent", &element_info->text_content);
  dict->GetString("href", &element_info->href);
  dict->GetString("src", &element_info->src);
  dict->GetString("alt", &element_info->alt_text);
  dict->GetString("title", &element_info->title);
  dict->GetString("role", &element_info->role);
  dict->GetString("ariaLabel", &element_info->aria_label);

  // Extract bounds
  const base::DictionaryValue* bounds = nullptr;
  if (dict->GetDictionary("bounds", &bounds)) {
    int x, y, width, height;
    if (bounds->GetInteger("x", &x) && bounds->GetInteger("y", &y) &&
        bounds->GetInteger("width", &width) && bounds->GetInteger("height", &height)) {
      element_info->bounds = gfx::Rect(x, y, width, height);
    }
  }

  // Extract styles
  const base::DictionaryValue* styles = nullptr;
  if (dict->GetDictionary("styles", &styles)) {
    std::string bg_color, color, font_size, font_family;
    if (styles->GetString("backgroundColor", &bg_color) &&
        styles->GetString("color", &color) &&
        styles->GetString("fontSize", &font_size) &&
        styles->GetString("fontFamily", &font_family)) {
      element_info->computed_styles = 
          "background-color: " + bg_color + "; " +
          "color: " + color + "; " +
          "font-size: " + font_size + "; " +
          "font-family: " + font_family;
    }
  }
}

bool ElementDetector::ShouldShowTooltip(const ElementInfo& element_info) const {
  // Don't show tooltip for empty elements
  if (element_info.tag_name.empty()) {
    return false;
  }

  // Don't show tooltip for very small elements
  if (element_info.bounds.width() < 10 || element_info.bounds.height() < 10) {
    return false;
  }

  // Check if element is interactive
  std::string attributes = element_info.href + " " + element_info.src + " " + 
                          element_info.role + " " + element_info.aria_label;
  
  return IsInteractiveElement(element_info.tag_name, attributes, element_info.class_name);
}

void ElementDetector::DidFinishLoad(content::RenderFrameHost* render_frame_host,
                                   const GURL& validated_url) {
  if (!monitoring_) {
    return;
  }

  VLOG(1) << "Page finished loading, ready for element detection: " << validated_url.spec();
}

}  // namespace tooltip

