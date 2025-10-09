// Copyright 2025
// Minimal spider toolbar button with simple animated icon while "crawling".

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLBAR_SPIDER_BUTTON_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLBAR_SPIDER_BUTTON_H_

#include "base/memory/raw_ptr.h"
#include "base/timer/timer.h"
#include "base/memory/weak_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/label_button.h"
#include "src/navigrab/navigrab_core.h"

class BrowserView;

// A minimal toolbar button that animates a spider icon while active.
class SpiderButton : public views::LabelButton {
 public:
  METADATA_HEADER(SpiderButton, views::LabelButton)
 
 public:
  
  explicit SpiderButton(BrowserView* browser_view);
  SpiderButton(const SpiderButton&) = delete;
  SpiderButton& operator=(const SpiderButton&) = delete;
  ~SpiderButton() override;

  // Starts/stops the crawling state and icon animation.
  void StartCrawl();
  void StopCrawl();
  bool is_crawling() const { return is_crawling_; }

  // views::View:
  void OnThemeChanged() override;
  void AddedToWidget() override;
  void RemovedFromWidget() override;
  void OnMouseEntered(const ui::MouseEvent& event) override;
  void OnMouseExited(const ui::MouseEvent& event) override;

 private:
  void OnPressed(const ui::Event& event);
  void TickAnimation();
  void UpdateIconImage();
  gfx::ImageSkia GenerateFrameImage(int frame_index, int total_frames, int dip);
  void CaptureSnapshot();
  void OnSnapshotCaptured(gfx::Image snapshot);
  void CaptureNavigrabPreview();
  void ShowPreviewBubble();
  void HidePreviewBubble();

  raw_ptr<BrowserView> browser_view_ = nullptr;
  bool is_crawling_ = false;

  // Stores the last captured screenshot and an on-hover preview bubble.
  gfx::Image last_snapshot_;
  raw_ptr<views::Widget> preview_bubble_widget_ = nullptr;
  raw_ptr<views::ImageView> preview_image_view_ = nullptr;

  base::WeakPtrFactory<SpiderButton> weak_factory_{this};
};

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLBAR_SPIDER_BUTTON_H_


