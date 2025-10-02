// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/element_detector.h"

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

ElementDetector::ElementDetector() = default;

ElementDetector::~ElementDetector() = default;

void ElementDetector::Initialize() {
  // No specific initialization needed for now.
}

void ElementDetector::StartDetection(content::WebContents* web_contents,
                                     base::OnceCallback<void(const std::vector<gfx::Rect>&, const std::vector<std::string>&)> callback) {
  DCHECK(web_contents);
  Observe(web_contents);
  detection_callback_ = std::move(callback);

  // Execute JavaScript to find interactive elements and generate unique identifiers.
  const std::string script = R"(
    (function() {
      const interactiveElements = [];
      const tags = ["button", "a", "input", "select", "textarea", "label", "area", "details", "summary", "iframe", "video", "audio", "canvas", "svg", "object", "embed"];
      const attributes = ["onclick", "onmousedown", "onmouseup", "onmouseover", "onmouseout", "role", "tabindex", "data-action", "data-toggle", "data-target", "href", "src", "action", "method"];

      let elementCounter = 0;

      const processElement = (el) => {
        const rect = el.getBoundingClientRect();
        if (rect.width > 0 && rect.height > 0) {
          const identifier = `element_${elementCounter++}_${el.tagName.toLowerCase()}`;
          interactiveElements.push({
            x: rect.left + window.scrollX,
            y: rect.top + window.scrollY,
            width: rect.width,
            height: rect.height,
            identifier: identifier
          });
        }
      };

      document.querySelectorAll(tags.join(", ")).forEach(processElement);

      attributes.forEach(attr => {
        document.querySelectorAll(`[${attr}]`).forEach(processElement);
      });

      return JSON.stringify(interactiveElements);
    })();
  )";

  web_contents->GetPrimaryMainFrame()->ExecuteJavaScript(
      base::UTF8ToUTF16(script),
      base::BindOnce(&ElementDetector::OnInteractiveElementsDetected,
                     weak_ptr_factory_.GetWeakPtr()));
}

void ElementDetector::StopDetection() {
  Observe(nullptr);
  interactive_elements_.clear();
  element_identifiers_.clear();
}

const std::vector<gfx::Rect>& ElementDetector::GetInteractiveElements() const {
  return interactive_elements_;
}

void ElementDetector::OnMouseEvent(const gfx::Point& screen_point) {
  // This method would typically be called by a browser event listener.
  // For now, we just log the event.
  VLOG(1) << "Mouse event at: " << screen_point.ToString();
}

void ElementDetector::OnInteractiveElementsDetected(
    const std::string& json_elements) {
  VLOG(1) << "Detected elements: " << json_elements;

  absl::optional<base::Value> value = base::JSONReader::Read(json_elements);
  if (!value || !value->is_list()) {
    LOG(ERROR) << "Failed to parse interactive elements JSON.";
    std::move(detection_callback_).Run({}, {});
    return;
  }

  interactive_elements_.clear();
  element_identifiers_.clear();
  for (const auto& element_value : value->GetList()) {
    if (!element_value.is_dict()) {
      continue;
    }
    const base::Value::Dict& element_dict = element_value.GetDict();
    absl::optional<int> x = element_dict.FindInt("x");
    absl::optional<int> y = element_dict.FindInt("y");
    absl::optional<int> width = element_dict.FindInt("width");
    absl::optional<int> height = element_dict.FindInt("height");
    const std::string* identifier = element_dict.FindString("identifier");

    if (x && y && width && height && identifier) {
      interactive_elements_.emplace_back(*x, *y, *width, *height);
      element_identifiers_.push_back(*identifier);
    }
  }
  std::move(detection_callback_).Run(interactive_elements_, element_identifiers_);
}

}  // namespace tooltip


