// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/tooltip/tooltip_view.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/functional/bind.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/widget/widget.h"

namespace tooltip {

TooltipView::TooltipView(views::View* anchor_view)
    : loading_(false) {
  SetLayoutManager(std::make_unique<views::FillLayout>());
  CreateViewHierarchy();
}

TooltipView::~TooltipView() = default;

void TooltipView::Initialize() {
  VLOG(1) << "TooltipView initialized";
}

void TooltipView::SetElementInfo(const ElementInfo& element_info) {
  element_info_ = element_info;
  UpdateContent();
}

void TooltipView::SetScreenshot(const gfx::Image& screenshot) {
  screenshot_ = screenshot;
  UpdateContent();
}

void TooltipView::SetAIResponse(const AIResponse& response) {
  ai_response_ = response;
  loading_ = false;
  UpdateContent();
}

void TooltipView::ShowAt(const gfx::Rect& bounds) {
  // Create and show the tooltip widget
  views::Widget* widget = new views::Widget;
  views::Widget::InitParams params(views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET);
  params.type = views::Widget::InitParams::TYPE_POPUP;
  params.bounds = bounds;
  params.delegate = this;
  
  widget->Init(std::move(params));
  widget->Show();
  
  VLOG(1) << "Tooltip shown at bounds: " << bounds.ToString();
}

void TooltipView::Hide() {
  // Hide the tooltip widget
  views::WidgetDelegate::GetWidget()->Hide();
  
  VLOG(1) << "Tooltip hidden";
}

void TooltipView::OnThemeChanged() {
  views::View::OnThemeChanged();
  UpdateTheme();
}

gfx::Size TooltipView::CalculatePreferredSize(const views::SizeBounds& available_size) const {
  return gfx::Size(320, 200);
}

void TooltipView::OnButtonPressed(views::Button* sender, const ui::Event& event) {
  if (sender == describe_button_) {
    OnDescribeButtonClicked();
  } else if (sender == capture_button_) {
    OnCaptureButtonClicked();
  } else if (sender == close_button_) {
    OnCloseButtonClicked();
  }
}

void TooltipView::CreateViewHierarchy() {
  // Create main container
  views::View* container = new views::View();
  container->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, gfx::Insets(12), 8));
  
  // Create header
  views::View* header = new views::View();
  header->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kHorizontal));
  
  title_label_ = new views::Label(u"Element Detected");
  title_label_->SetFontList(gfx::FontList().DeriveWithWeight(gfx::Font::Weight::BOLD));
  header->AddChildView(title_label_);
  
  close_button_ = new views::MdTextButton(
      base::BindRepeating(&TooltipView::OnCloseButtonClicked, base::Unretained(this)), u"×");
  header->AddChildView(close_button_);
  
  container->AddChildView(header);
  
  // Create element info section
  element_info_label_ = new views::Label();
  element_info_label_->SetMultiLine(true);
  element_info_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  container->AddChildView(element_info_label_);
  
  // Create screenshot section
  screenshot_view_ = new views::ImageView();
  screenshot_view_->SetPreferredSize(gfx::Size(200, 100));
  container->AddChildView(screenshot_view_);
  
  // Create AI response section
  ai_response_label_ = new views::Label();
  ai_response_label_->SetMultiLine(true);
  ai_response_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  container->AddChildView(ai_response_label_);
  
  // Create loading bar
  loading_bar_ = new views::ProgressBar();
  loading_bar_->SetVisible(false);
  container->AddChildView(loading_bar_);
  
  // Create action buttons
  views::View* button_container = new views::View();
  button_container->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kHorizontal, gfx::Insets(), 8));
  
  describe_button_ = new views::MdTextButton(
      base::BindRepeating(&TooltipView::OnDescribeButtonClicked, base::Unretained(this)), u"🤖 Describe");
  capture_button_ = new views::MdTextButton(
      base::BindRepeating(&TooltipView::OnCaptureButtonClicked, base::Unretained(this)), u"📸 Capture");
  
  button_container->AddChildView(describe_button_);
  button_container->AddChildView(capture_button_);
  
  container->AddChildView(button_container);
  
  AddChildView(container);
  
  // Set initial theme
  UpdateTheme();
}

void TooltipView::UpdateContent() {
  if (!element_info_.tag_name.empty()) {
    // Update element info
    std::string info_text = "<" + element_info_.tag_name + ">";
    if (!element_info_.id.empty()) {
      info_text += " #" + element_info_.id;
    }
    if (!element_info_.class_name.empty()) {
      info_text += " ." + element_info_.class_name;
    }
    if (!element_info_.text_content.empty()) {
      info_text += "\nText: " + element_info_.text_content;
    }
    
    element_info_label_->SetText(base::UTF8ToUTF16(info_text));
  }
  
  // Update screenshot
  if (!screenshot_.IsEmpty()) {
    screenshot_view_->SetImage(ui::ImageModel::FromImageSkia(*screenshot_.ToImageSkia()));
    screenshot_view_->SetVisible(true);
  } else {
    screenshot_view_->SetVisible(false);
  }
  
  // Update AI response
  if (!ai_response_.description.empty()) {
    ai_response_label_->SetText(base::UTF8ToUTF16(ai_response_.description));
    ai_response_label_->SetVisible(true);
  } else {
    ai_response_label_->SetVisible(false);
  }
  
  // Update loading state
  loading_bar_->SetVisible(loading_);
  
  // Update button states
  describe_button_->SetEnabled(!loading_);
  capture_button_->SetEnabled(!loading_);
}

void TooltipView::UpdateTheme() {
  // Set background color
  SetBackground(views::CreateSolidBackground(SK_ColorWHITE));
  
  // Set border
  SetBorder(views::CreateSolidBorder(1, SK_ColorGRAY));
  
  // Update text colors
  if (title_label_) {
    title_label_->SetEnabledColor(SK_ColorBLACK);
  }
  if (element_info_label_) {
    element_info_label_->SetEnabledColor(SK_ColorBLACK);
  }
  if (ai_response_label_) {
    ai_response_label_->SetEnabledColor(SK_ColorBLACK);
  }
}

void TooltipView::OnDescribeButtonClicked() {
  VLOG(1) << "Describe button clicked";
  loading_ = true;
  UpdateContent();
  
  // TODO: Trigger AI description request
}

void TooltipView::OnCaptureButtonClicked() {
  VLOG(1) << "Capture button clicked";
  
  // TODO: Trigger screenshot capture
}

void TooltipView::OnCloseButtonClicked() {
  VLOG(1) << "Close button clicked";
  Hide();
}

}  // namespace tooltip

