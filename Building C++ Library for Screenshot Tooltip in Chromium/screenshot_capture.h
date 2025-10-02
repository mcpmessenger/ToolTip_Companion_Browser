// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_SCREENSHOT_CAPTURE_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_SCREENSHOT_CAPTURE_H_

#include <memory>
#include <string>

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

  // Captures a screenshot of the specified `rect` within the `web_contents`.
  // The `callback` will be invoked with the captured image (or an empty image
  // on failure) and the element identifier.
  void Capture(content::WebContents* web_contents,
               const gfx::Rect& rect,
               const std::string& element_identifier,
               base::OnceCallback<void(const std::string&, const gfx::Image&)> callback);

 private:
  // Called when the screenshot is ready.
  void OnScreenshotCaptured(const std::string& element_identifier, const SkBitmap& bitmap);

  bool initialized_ = false;
  base::OnceCallback<void(const std::string&, const gfx::Image&)> capture_callback_;
  std::string current_element_identifier_;

  base::WeakPtrFactory<ScreenshotCapture> weak_ptr_factory_{this};
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_SCREENSHOT_CAPTURE_H_

