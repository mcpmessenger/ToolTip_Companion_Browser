// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_MANAGER_SERVICE_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_MANAGER_SERVICE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/tooltip/element_detector.h"
#include "chrome/browser/tooltip/screenshot_capture.h"
#include "content/public/browser/web_contents_observer.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/image/image.h"

namespace content {
class WebContents;
}

namespace tooltip {

struct ElementInfo {
  gfx::Rect bounding_box;
  std::string url_or_action;
  // Other relevant metadata can be added here
};

class LocalStorageManager;
class TooltipUIController;

// Manages the lifecycle of tooltips, including element detection, screenshot
// capture, local storage, and UI display.
class TooltipManagerService {
 public:
  TooltipManagerService();
  ~TooltipManagerService();

  // Initializes the service components.
  void Initialize();

  // Triggers the crawling of interactive elements and captures screenshots.
  // If `proactive` is true, it runs automatically. If false, it runs on-demand.
  void StartCrawlingAndCapture(content::WebContents* web_contents, bool proactive);

  // Displays a tooltip for the element at the given screen coordinates.
  void ShowTooltip(content::WebContents* web_contents, const gfx::Point& screen_point);

  // Hides any active tooltip.
  void HideTooltip();

 private:
  // Callbacks for ElementDetector and ScreenshotCapture.
  void OnElementsDetected(const std::vector<gfx::Rect>& elements,
                          const std::vector<std::string>& identifiers);
  void OnScreenshotCaptured(const std::string& element_identifier,
                            const gfx::Image& image);

  std::unique_ptr<ElementDetector> element_detector_;
  std::unique_ptr<ScreenshotCapture> screenshot_capture_;
  std::unique_ptr<LocalStorageManager> local_storage_manager_;
  std::unique_ptr<TooltipUIController> tooltip_ui_controller_;

  // Map to store element identifiers to their bounding boxes and associated URLs/actions.
  std::map<std::string, ElementInfo> element_info_map_;

  base::WeakPtrFactory<TooltipManagerService> weak_ptr_factory_{this};
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_MANAGER_SERVICE_H_

