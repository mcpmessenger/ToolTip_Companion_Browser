#include "chrome/browser/ui/views/toolbar/spider_button.h"

#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/snapshot/snapshot.h"
#include "ui/gfx/image/image.h"
#include "base/containers/span.h"
#include "ui/base/models/image_model.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/widget/widget.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/views/controls/button/label_button.h"

SpiderButton::SpiderButton(BrowserView* browser_view)
    : views::LabelButton(base::BindRepeating(&SpiderButton::OnPressed,
                                             base::Unretained(this)),
                         u"🕷️"),
      browser_view_(browser_view) {
  LabelButton::SetTooltipText(u"Start crawl");
}

SpiderButton::~SpiderButton() = default;

void SpiderButton::OnPressed(const ui::Event& event) {
  if (is_crawling_) {
    return;
  }
  StartCrawl();

  // TODO: Wire to Navigrab backend. For now, just run a short demo timer.
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE, base::BindOnce(&SpiderButton::StopCrawl, weak_factory_.GetWeakPtr()),
      base::Seconds(3));
}

void SpiderButton::StartCrawl() {
  if (is_crawling_) return;
  is_crawling_ = true;
  LabelButton::SetText(u"🕷️...");
}

void SpiderButton::StopCrawl() {
  if (!is_crawling_) return;
  is_crawling_ = false;
  LabelButton::SetText(u"🕷️");
  // Prefer Navigrab preview if available; fallback to window snapshot.
  CaptureNavigrabPreview();
}

void SpiderButton::TickAnimation() {
  // Not implemented yet
}

void SpiderButton::OnThemeChanged() {
  views::LabelButton::OnThemeChanged();
}

void SpiderButton::AddedToWidget() {
  views::LabelButton::AddedToWidget();
}

void SpiderButton::RemovedFromWidget() {
  views::LabelButton::RemovedFromWidget();
  HidePreviewBubble();
}

void SpiderButton::UpdateIconImage() {
  // Not implemented yet
}

gfx::ImageSkia SpiderButton::GenerateFrameImage(int frame_index, int total_frames, int dip) {
  // Not implemented yet
  return gfx::ImageSkia();
}

BEGIN_METADATA(SpiderButton)
END_METADATA

void SpiderButton::OnMouseEntered(const ui::MouseEvent& event) {
  views::LabelButton::OnMouseEntered(event);
  if (!last_snapshot_.IsEmpty()) {
    ShowPreviewBubble();
  }
}

void SpiderButton::OnMouseExited(const ui::MouseEvent& event) {
  views::LabelButton::OnMouseExited(event);
  HidePreviewBubble();
}

void SpiderButton::CaptureSnapshot() {
  if (!GetWidget()) {
    return;
  }
  gfx::Rect rect = GetWidget()->GetWindowBoundsInScreen();
  ui::GrabWindowSnapshot(
      GetWidget()->GetNativeWindow(), rect,
      base::BindOnce(&SpiderButton::OnSnapshotCaptured,
                     weak_factory_.GetWeakPtr()));
}

void SpiderButton::OnSnapshotCaptured(gfx::Image snapshot) {
  last_snapshot_ = std::move(snapshot);
}

void SpiderButton::CaptureNavigrabPreview() {
  // Use Navigrab to capture a viewport image into memory and display it.
  auto& core = navigrab::NaviGrabCore::GetInstance();
  if (!core.IsInitialized()) {
    core.Initialize();
  }
  std::unique_ptr<navigrab::ScreenshotCapture> cap = navigrab::CreateScreenshotCapture();
  std::vector<uint8_t> data;
  if (cap && cap->CaptureToMemory(data) && !data.empty()) {
    // Decode into gfx::Image from PNG bytes.
    gfx::Image image = gfx::Image::CreateFrom1xPNGBytes(
        base::span<const uint8_t>(data.data(), data.size()));
    if (!image.IsEmpty()) {
      last_snapshot_ = image;
      return;
    }
  }
  // Fallback to window snapshot.
  CaptureSnapshot();
}

void SpiderButton::ShowPreviewBubble() {
  if (preview_bubble_widget_) {
    return;
  }
  auto preview_widget = std::make_unique<views::Widget>();
  views::Widget::InitParams params(views::Widget::InitParams::CLIENT_OWNS_WIDGET,
                                   views::Widget::InitParams::TYPE_TOOLTIP);
  params.activatable = views::Widget::InitParams::Activatable::kNo;
  params.parent = GetWidget()->GetNativeView();
  preview_widget->Init(std::move(params));

  auto container = std::make_unique<views::View>();
  preview_image_view_ = container->AddChildView(std::make_unique<views::ImageView>());
  preview_image_view_->SetImage(ui::ImageModel::FromImageSkia(last_snapshot_.AsImageSkia()));
  preview_image_view_->SetImageSize(gfx::Size(240, 150));
  preview_widget->SetContentsView(std::move(container));

  gfx::Rect anchor = GetBoundsInScreen();
  gfx::Rect bubble_bounds(anchor.right(), anchor.bottom(), 240, 150);
  preview_widget->SetBounds(bubble_bounds);
  preview_widget->ShowInactive();
  preview_bubble_widget_ = preview_widget.release();
}

void SpiderButton::HidePreviewBubble() {
  if (!preview_bubble_widget_) {
    return;
  }
  preview_bubble_widget_->CloseNow();
  preview_bubble_widget_ = nullptr;
  preview_image_view_ = nullptr;
}
