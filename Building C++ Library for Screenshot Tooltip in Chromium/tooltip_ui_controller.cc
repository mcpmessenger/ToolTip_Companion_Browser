// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/tooltip_ui_controller.h"

#include "base/logging.h"

namespace tooltip {

TooltipUIController::TooltipUIController() = default;

TooltipUIController::~TooltipUIController() = default;

void TooltipUIController::Initialize() {
  // TODO(manus): Initialize UI components here, e.g., create a views::Widget.
  VLOG(1) << "TooltipUIController initialized.";
}

void TooltipUIController::DisplayTooltip(
    const std::string& base64_image, const gfx::Point& screen_point) {
  // TODO(manus): Implement actual UI display logic here.
  // This would involve decoding the base64_image and rendering it in a
  // transient UI element at `screen_point`.
  VLOG(1) << "Displaying tooltip at: " << screen_point.ToString()
          << " with image (base64 length): " << base64_image.length();
}

void TooltipUIController::HideTooltip() {
  // TODO(manus): Implement actual UI hiding logic here.
  VLOG(1) << "Hiding tooltip.";
}

}  // namespace tooltip


