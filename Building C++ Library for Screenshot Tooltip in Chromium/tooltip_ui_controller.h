// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_UI_CONTROLLER_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_UI_CONTROLLER_H_

#include <string>

#include "base/memory/weak_ptr.h"
#include "ui/gfx/geometry/point.h"

namespace tooltip {

// Controls the display and hiding of the tooltip UI.
class TooltipUIController {
 public:
  TooltipUIController();
  ~TooltipUIController();

  // Initializes the UI controller.
  void Initialize();

  // Displays a tooltip with the given Base64 image at the specified screen coordinates.
  void DisplayTooltip(const std::string& base64_image, const gfx::Point& screen_point);

  // Hides the currently displayed tooltip.
  void HideTooltip();

 private:
  base::WeakPtrFactory<TooltipUIController> weak_ptr_factory_{this};
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_UI_CONTROLLER_H_

