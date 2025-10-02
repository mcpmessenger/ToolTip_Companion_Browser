// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/tooltip/fresh_crawl_button.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/ui/views/tooltip/tooltip_toolbar_integration.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/highlight_path_generator.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/vector_icons.h"

namespace tooltip {

namespace {
// Icon for the fresh crawl button (using refresh icon)
const gfx::VectorIcon& GetFreshCrawlIcon() {
  return ui::kRefreshIcon;
}

// Button size
constexpr int kButtonSize = 24;
constexpr int kIconSize = 16;
}  // namespace

FreshCrawlButton::FreshCrawlButton(PressedCallback callback)
    : ImageButton(std::move(callback)),
      enabled_(true),
      loading_(false) {
  SetImageHorizontalAlignment(ALIGN_CENTER);
  SetImageVerticalAlignment(ALIGN_MIDDLE);
  SetPreferredSize(gfx::Size(kButtonSize, kButtonSize));
  
  // Set up button styling
  SetFocusBehavior(FocusBehavior::ACCESSIBLE_ONLY);
  SetTooltipText(base::UTF8ToUTF16("Fresh Crawl - Re-scan page elements"));
  
  // Add highlight path for better visual feedback
  views::InstallCircleHighlightPathGenerator(this);
  
  UpdateButtonImage();
}

FreshCrawlButton::~FreshCrawlButton() = default;

void FreshCrawlButton::OnThemeChanged() {
  ImageButton::OnThemeChanged();
  UpdateButtonImage();
}

void FreshCrawlButton::OnMouseEntered(const ui::MouseEvent& event) {
  ImageButton::OnMouseEntered(event);
  UpdateButtonImage();
}

void FreshCrawlButton::OnMouseExited(const ui::MouseEvent& event) {
  ImageButton::OnMouseExited(event);
  UpdateButtonImage();
}

void FreshCrawlButton::SetEnabled(bool enabled) {
  if (enabled_ == enabled) {
    return;
  }
  
  enabled_ = enabled;
  ImageButton::SetEnabled(enabled);
  UpdateButtonImage();
  UpdateTooltipText();
}

void FreshCrawlButton::SetLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  
  loading_ = loading;
  UpdateButtonImage();
  UpdateTooltipText();
}

void FreshCrawlButton::UpdateButtonImage() {
  if (!GetWidget()) {
    return;
  }
  
  const ui::ColorId icon_color = enabled_ 
      ? (loading_ ? ui::kColorAccent : ui::kColorButtonForeground)
      : ui::kColorButtonForegroundDisabled;
  
  const gfx::VectorIcon& icon = GetFreshCrawlIcon();
  
  SetImage(views::Button::STATE_NORMAL,
           ui::ImageModel::FromVectorIcon(icon, icon_color, kIconSize));
  SetImage(views::Button::STATE_HOVERED,
           ui::ImageModel::FromVectorIcon(icon, icon_color, kIconSize));
  SetImage(views::Button::STATE_PRESSED,
           ui::ImageModel::FromVectorIcon(icon, icon_color, kIconSize));
  SetImage(views::Button::STATE_DISABLED,
           ui::ImageModel::FromVectorIcon(icon, icon_color, kIconSize));
}

void FreshCrawlButton::UpdateTooltipText() {
  std::u16string tooltip_text;
  
  if (loading_) {
    tooltip_text = base::UTF8ToUTF16("Crawling page...");
  } else if (enabled_) {
    tooltip_text = base::UTF8ToUTF16("Fresh Crawl - Re-scan page elements");
  } else {
    tooltip_text = base::UTF8ToUTF16("Fresh Crawl - Not available");
  }
  
  SetTooltipText(tooltip_text);
}

BEGIN_METADATA(FreshCrawlButton, views::ImageButton)
END_METADATA

}  // namespace tooltip

