// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_SCREENSHOT_CAPTURE_H_
#define CHROME_BROWSER_TOOLTIP_SCREENSHOT_CAPTURE_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "content/public/browser/web_contents_observer.h"
#include "ui/gfx/image/image.h"

namespace content {
class WebContents;
}

namespace tooltip {

// Captures screenshots of web elements
class ScreenshotCapture : public content::WebContentsObserver {
 public:
  ScreenshotCapture();
  ~ScreenshotCapture() override;

  // Initialize the capture system
  void Initialize();

  // Capture screenshot of an element
  void CaptureElement(content::WebContents* web_contents,
                     const ElementInfo& element_info,
                     base::OnceCallback<void(const gfx::Image&)> callback);

  // Capture screenshot of entire page
  void CapturePage(content::WebContents* web_contents,
                  base::OnceCallback<void(const gfx::Image&)> callback);

  // Capture screenshot of viewport
  void CaptureViewport(content::WebContents* web_contents,
                      base::OnceCallback<void(const gfx::Image&)> callback);

 private:
  // Handle screenshot capture completion
  void OnScreenshotCaptured(base::OnceCallback<void(const gfx::Image&)> callback,
                           const gfx::Image& image);

  // Process captured image
  gfx::Image ProcessImage(const gfx::Image& image, const ElementInfo& element_info);

  // Crop image to element bounds
  gfx::Image CropToElement(const gfx::Image& image, const gfx::Rect& element_bounds);

  // Resize image if needed
  gfx::Image ResizeImage(const gfx::Image& image, int max_size);

  bool initialized_;
  base::WeakPtrFactory<ScreenshotCapture> weak_factory_{this};

  ScreenshotCapture(const ScreenshotCapture&) = delete;
  ScreenshotCapture& operator=(const ScreenshotCapture&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_SCREENSHOT_CAPTURE_H_

