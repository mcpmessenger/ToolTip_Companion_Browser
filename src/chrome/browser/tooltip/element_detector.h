// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_ELEMENT_DETECTOR_H_
#define CHROME_BROWSER_TOOLTIP_ELEMENT_DETECTOR_H_

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

  // Initialize the detector
  void Initialize();

  // Start monitoring a web contents
  void StartMonitoring(content::WebContents* web_contents);

  // Stop monitoring
  void StopMonitoring();

  // Check if an element is interactive
  bool IsInteractiveElement(const std::string& tag_name,
                           const std::string& attributes,
                           const std::string& class_name) const;

  // Get element information from coordinates
  void GetElementAtPoint(content::WebContents* web_contents,
                        const gfx::Point& point,
                        base::OnceCallback<void(const ElementInfo&)> callback);

  // WebContentsObserver overrides
  void DidFinishLoad(content::RenderFrameHost* render_frame_host,
                    const GURL& validated_url) override;

 private:
  // Process mouse move events
  void OnMouseMove(const gfx::Point& point);

  // Extract element information from JavaScript
  void ExtractElementInfo(const std::string& element_data,
                         ElementInfo* element_info);

  // Check if element should show tooltip
  bool ShouldShowTooltip(const ElementInfo& element_info) const;

  // Interactive element types
  static const std::vector<std::string> kInteractiveTags;
  static const std::vector<std::string> kInteractiveAttributes;
  static const std::vector<std::string> kInteractiveClasses;

  bool initialized_;
  bool monitoring_;
  base::WeakPtrFactory<ElementDetector> weak_factory_{this};

  ElementDetector(const ElementDetector&) = delete;
  ElementDetector& operator=(const ElementDetector&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_ELEMENT_DETECTOR_H_

