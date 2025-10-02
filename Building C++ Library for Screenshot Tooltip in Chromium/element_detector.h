// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_ELEMENT_DETECTOR_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_ELEMENT_DETECTOR_H_

#include <memory>
#include <string>
#include <vector>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "content/public/browser/web_contents_observer.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"

namespace content {
class WebContents;
}

namespace tooltip {

// Detects interactive elements and manages hover events
class ElementDetector : public content::WebContentsObserver {
 public:
  ElementDetector();
  ~ElementDetector() override;

  // Initialize the element detection system.
  void Initialize();

  // Starts detecting interactive elements on the given `web_contents`.
  // The `callback` will be invoked with a list of bounding boxes and unique identifiers.
  void StartDetection(content::WebContents* web_contents,
                      base::OnceCallback<void(const std::vector<gfx::Rect>&, const std::vector<std::string>&)> callback);

  // Stops detecting interactive elements.
  void StopDetection();

  // Returns the currently detected interactive elements.
  const std::vector<gfx::Rect>& GetInteractiveElements() const;

  // Called when the mouse hovers over an element.
  void OnMouseEvent(const gfx::Point& screen_point);

 private:
  // Callback for JavaScript execution to get interactive elements.
  void OnInteractiveElementsDetected(const std::string& json_elements);

  std::vector<gfx::Rect> interactive_elements_;
  std::vector<std::string> element_identifiers_;
  base::OnceCallback<void(const std::vector<gfx::Rect>&, const std::vector<std::string>&)> detection_callback_;
  base::WeakPtrFactory<ElementDetector> weak_ptr_factory_{this};
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_ELEMENT_DETECTOR_H_

