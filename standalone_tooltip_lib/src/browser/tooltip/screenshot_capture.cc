// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "screenshot_capture.h"

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

ScreenshotCapture::ScreenshotCapture()
    : initialized_(false) {}

ScreenshotCapture::~ScreenshotCapture() = default;

void ScreenshotCapture::Initialize() {
  if (initialized_) {
    return;
  }

  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  initialized_ = true;
  
  VLOG(1) << "ScreenshotCapture initialized";
}

void ScreenshotCapture::CaptureElement(
    content::WebContents* web_contents,
    const ElementInfo& element_info,
    base::OnceCallback<void(const gfx::Image&)> callback) {
  
  if (!initialized_ || !web_contents) {
    std::move(callback).Run(gfx::Image());
    return;
  }

  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  // For now, capture the entire viewport and crop to element
  // In a full implementation, we'd use more sophisticated methods
  CaptureViewport(web_contents, 
                  base::BindOnce(&ScreenshotCapture::OnViewportCaptured,
                                weak_factory_.GetWeakPtr(),
                                element_info, std::move(callback)));
}

void ScreenshotCapture::CapturePage(
    content::WebContents* web_contents,
    base::OnceCallback<void(const gfx::Image&)> callback) {
  
  if (!initialized_ || !web_contents) {
    std::move(callback).Run(gfx::Image());
    return;
  }

  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  // Capture the entire page content
  web_contents->GetMainFrame()->CopyImageAt(
      gfx::Point(0, 0),
      base::BindOnce(&ScreenshotCapture::OnScreenshotCaptured,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void ScreenshotCapture::CaptureViewport(
    content::WebContents* web_contents,
    base::OnceCallback<void(const gfx::Image&)> callback) {
  
  if (!initialized_ || !web_contents) {
    std::move(callback).Run(gfx::Image());
    return;
  }

  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  // Get the viewport bounds
  gfx::Rect viewport_bounds = web_contents->GetContainerBounds();
  
  // Use Chromium's snapshot API to capture the viewport
  ui::GrabViewSnapshotAsync(
      web_contents->GetNativeView(),
      viewport_bounds,
      base::BindOnce(&ScreenshotCapture::OnScreenshotCaptured,
                     weak_factory_.GetWeakPtr(), std::move(callback)));
}

void ScreenshotCapture::OnScreenshotCaptured(
    base::OnceCallback<void(const gfx::Image&)> callback,
    const gfx::Image& image) {
  
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  
  if (image.IsEmpty()) {
    VLOG(1) << "Screenshot capture failed - empty image";
    std::move(callback).Run(gfx::Image());
    return;
  }

  VLOG(1) << "Screenshot captured successfully";
  std::move(callback).Run(image);
}

void ScreenshotCapture::OnViewportCaptured(
    const ElementInfo& element_info,
    base::OnceCallback<void(const gfx::Image&)> callback,
    const gfx::Image& viewport_image) {
  
  if (viewport_image.IsEmpty()) {
    std::move(callback).Run(gfx::Image());
    return;
  }

  // Process the image on a background thread
  base::ThreadPool::PostTaskAndReplyWithResult(
      FROM_HERE,
      base::BindOnce(&ScreenshotCapture::ProcessImage,
                     viewport_image, element_info),
      std::move(callback));
}

gfx::Image ScreenshotCapture::ProcessImage(const gfx::Image& image,
                                          const ElementInfo& element_info) {
  // Crop to element bounds if specified
  gfx::Image processed_image = image;
  
  if (!element_info.bounds.IsEmpty()) {
    processed_image = CropToElement(image, element_info.bounds);
  }
  
  // Resize if needed (max 1024px)
  processed_image = ResizeImage(processed_image, 1024);
  
  return processed_image;
}

gfx::Image ScreenshotCapture::CropToElement(const gfx::Image& image,
                                           const gfx::Rect& element_bounds) {
  if (image.IsEmpty() || element_bounds.IsEmpty()) {
    return image;
  }

  // Get the image representation
  const gfx::ImageSkia* image_skia = image.ToImageSkia();
  if (!image_skia || image_skia->isNull()) {
    return image;
  }

  // Create a cropped version
  gfx::ImageSkia cropped_skia = gfx::ImageSkia::CreateFrom1xBitmap(
      image_skia->GetRepresentation(1.0f).GetBitmap());
  
  // Crop to the element bounds
  gfx::Rect crop_bounds = element_bounds;
  crop_bounds.Intersect(gfx::Rect(image_skia->size()));
  
  if (crop_bounds.IsEmpty()) {
    return image;
  }

  // Extract the cropped region
  SkBitmap cropped_bitmap;
  if (image_skia->GetRepresentation(1.0f).GetBitmap().extractSubset(
          &cropped_bitmap, gfx::RectToSkIRect(crop_bounds))) {
    return gfx::Image::CreateFrom1xBitmap(cropped_bitmap);
  }

  return image;
}

gfx::Image ScreenshotCapture::ResizeImage(const gfx::Image& image, int max_size) {
  if (image.IsEmpty()) {
    return image;
  }

  const gfx::ImageSkia* image_skia = image.ToImageSkia();
  if (!image_skia || image_skia->isNull()) {
    return image;
  }

  gfx::Size current_size = image_skia->size();
  
  // Don't resize if already smaller than max_size
  if (current_size.width() <= max_size && current_size.height() <= max_size) {
    return image;
  }

  // Calculate new size maintaining aspect ratio
  double scale = 1.0;
  if (current_size.width() > current_size.height()) {
    scale = static_cast<double>(max_size) / current_size.width();
  } else {
    scale = static_cast<double>(max_size) / current_size.height();
  }

  gfx::Size new_size(
      static_cast<int>(current_size.width() * scale),
      static_cast<int>(current_size.height() * scale));

  // Create resized image
  gfx::ImageSkia resized_skia = gfx::ImageSkia::CreateFrom1xBitmap(
      image_skia->GetRepresentation(1.0f).GetBitmap());
  
  // Scale the image
  SkBitmap resized_bitmap;
  if (resized_skia.GetRepresentation(1.0f).GetBitmap().scalePixels(
          &resized_bitmap, SkFilterQuality::kMedium_SkFilterQuality)) {
    return gfx::Image::CreateFrom1xBitmap(resized_bitmap);
  }

  return image;
}

}  // namespace tooltip

