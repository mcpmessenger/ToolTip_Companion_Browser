// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/screenshot_capture.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/task/task_runner.h"
#include "base/task/thread_pool.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/snapshot/snapshot.h"

namespace tooltip {

ScreenshotCapture::ScreenshotCapture() = default;

ScreenshotCapture::~ScreenshotCapture() = default;

void ScreenshotCapture::Initialize() {
  if (initialized_) {
    return;
  }
  initialized_ = true;
}

void ScreenshotCapture::Capture(
    content::WebContents* web_contents,
    const gfx::Rect& rect,
    const std::string& element_identifier,
    base::OnceCallback<void(const std::string&, const gfx::Image&)> callback) {
  DCHECK(web_contents);
  DCHECK(!callback.is_null());

  capture_callback_ = std::move(callback);
  current_element_identifier_ = element_identifier;

  // Capture the screenshot on a separate thread to avoid blocking the UI.
  base::ThreadPool::PostTask(
      FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_VISIBLE},
      base::BindOnce(&ScreenshotCapture::CaptureOnWorkerThread,
                     weak_ptr_factory_.GetWeakPtr(), web_contents,
                     rect));
}

void ScreenshotCapture::CaptureOnWorkerThread(
    content::WebContents* web_contents,
    const gfx::Rect& rect) {
  // This method runs on a worker thread, so we need to post the result back
  // to the UI thread.
  content::RenderFrameHost* render_frame_host = web_contents->GetPrimaryMainFrame();
  if (!render_frame_host) {
    std::move(capture_callback_).Run(current_element_identifier_, gfx::Image());
    return;
  }

  // Capture the screenshot.
  gfx::Image image = ui::snapshot::GrabView(render_frame_host->GetView(), rect);

  // Post the result back to the UI thread.
  base::ThreadPool::PostTask(
      FROM_HERE, {base::TaskPriority::USER_VISIBLE},
      base::BindOnce(&ScreenshotCapture::OnScreenshotCaptured,
                     weak_ptr_factory_.GetWeakPtr(), current_element_identifier_, image.AsBitmap()));
}

void ScreenshotCapture::OnScreenshotCaptured(const std::string& element_identifier, const SkBitmap& bitmap) {
  if (capture_callback_.is_null()) {
    return;
  }

  if (bitmap.empty()) {
    std::move(capture_callback_).Run(element_identifier, gfx::Image());
    return;
  }

  std::move(capture_callback_).Run(element_identifier, gfx::Image::CreateFrom1xBitmap(bitmap));
}

}  // namespace tooltip


