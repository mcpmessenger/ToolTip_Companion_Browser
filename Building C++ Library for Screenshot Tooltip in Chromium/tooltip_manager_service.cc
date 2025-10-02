// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/tooltip_manager_service.h"

#include "base/base66_encode.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "chrome/browser/tooltip/local_storage_manager.h"
#include "chrome/browser/tooltip/tooltip_ui_controller.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/codec/png_codec.h"

namespace tooltip {

TooltipManagerService::TooltipManagerService()
    : element_detector_(std::make_unique<ElementDetector>()),
      screenshot_capture_(std::make_unique<ScreenshotCapture>()),
      local_storage_manager_(std::make_unique<LocalStorageManager>()),
      tooltip_ui_controller_(std::make_unique<TooltipUIController>()) {}

TooltipManagerService::~TooltipManagerService() = default;

void TooltipManagerService::Initialize() {
  element_detector_->Initialize();
  screenshot_capture_->Initialize();
  local_storage_manager_->Initialize();
  tooltip_ui_controller_->Initialize();
}

void TooltipManagerService::StartCrawlingAndCapture(
    content::WebContents* web_contents, bool proactive) {
  DCHECK(web_contents);
  element_info_map_.clear();

  element_detector_->StartDetection(
      web_contents,
      base::BindOnce(&TooltipManagerService::OnElementsDetected,
                     weak_ptr_factory_.GetWeakPtr()));
}

void TooltipManagerService::ShowTooltip(
    content::WebContents* web_contents, const gfx::Point& screen_point) {
  // Find if the screen_point is within any detected element.
  for (const auto& pair : element_info_map_) {
    if (pair.second.bounding_box.Contains(screen_point)) {
      const std::string& identifier = pair.first;
      std::string base64_image = local_storage_manager_->RetrieveImage(identifier);
      if (!base64_image.empty()) {
        tooltip_ui_controller_->DisplayTooltip(base64_image, screen_point);
        return;
      }
    }
  }
  HideTooltip();
}

void TooltipManagerService::HideTooltip() {
  tooltip_ui_controller_->HideTooltip();
}

void TooltipManagerService::OnElementsDetected(
    const std::vector<gfx::Rect>& elements,
    const std::vector<std::string>& identifiers) {
  DCHECK_EQ(elements.size(), identifiers.size());

  for (size_t i = 0; i < elements.size(); ++i) {
    const std::string& identifier = identifiers[i];
    const gfx::Rect& rect = elements[i];

    ElementInfo info;
    info.bounding_box = rect;
    // TODO(manus): Populate url_or_action from element attributes if available.
    element_info_map_[identifier] = info;

    // Capture screenshot for each detected element.
    screenshot_capture_->Capture(
        element_detector_->web_contents(), rect, identifier,
        base::BindOnce(&TooltipManagerService::OnScreenshotCaptured,
                       weak_ptr_factory_.GetWeakPtr()));
  }
}

void TooltipManagerService::OnScreenshotCaptured(
    const std::string& element_identifier, const gfx::Image& image) {
  if (image.IsEmpty()) {
    LOG(ERROR) << "Failed to capture screenshot for element: " << element_identifier;
    return;
  }

  // Convert gfx::Image to SkBitmap, then to PNG, then to Base64.
  SkBitmap bitmap = image.AsBitmap();
  std::vector<unsigned char> png_data;
  if (!gfx::PNGCodec::Encode(bitmap, gfx::PNGCodec::ProcessPNGOptions(), &png_data)) {
    LOG(ERROR) << "Failed to encode screenshot to PNG for element: " << element_identifier;
    return;
  }

  std::string base64_image;
  base::Base64Encode(std::string(png_data.begin(), png_data.end()), &base64_image);

  local_storage_manager_->StoreImage(element_identifier, base64_image);
  VLOG(1) << "Screenshot captured and stored for element: " << element_identifier;
}

}  // namespace tooltip


