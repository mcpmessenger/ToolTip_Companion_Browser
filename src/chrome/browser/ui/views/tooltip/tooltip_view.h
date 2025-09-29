// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_VIEW_H_

#include <memory>
#include <string>

#include "base/memory/weak_ptr.h"
#include "base/memory/raw_ptr.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/image/image.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/view.h"
#include "ui/views/widget/widget_delegate.h"

namespace tooltip {

// Native tooltip view using Views framework
class TooltipView : public views::View, public views::WidgetDelegate {
 public:
  explicit TooltipView(views::View* anchor_view);
  ~TooltipView() override;

  // Initialize the view
  void Initialize();

  // Set element information
  void SetElementInfo(const ElementInfo& element_info);

  // Set screenshot
  void SetScreenshot(const gfx::Image& screenshot);

  // Set AI response
  void SetAIResponse(const AIResponse& response);

  // Show tooltip at specified bounds
  void ShowAt(const gfx::Rect& bounds);

  // Hide tooltip
  void Hide();

  // views::View overrides
  void OnThemeChanged() override;
  gfx::Size CalculatePreferredSize(const views::SizeBounds& available_size) const override;

  // Button event handlers
  void OnButtonPressed(views::Button* sender, const ui::Event& event);

 private:
  // Create the view hierarchy
  void CreateViewHierarchy();

  // Update view content
  void UpdateContent();

  // Update theme colors
  void UpdateTheme();

  // Handle button clicks
  void OnDescribeButtonClicked();
  void OnCaptureButtonClicked();
  void OnCloseButtonClicked();

  // View components
  raw_ptr<views::Label> title_label_;
  raw_ptr<views::Label> element_info_label_;
  raw_ptr<views::ImageView> screenshot_view_;
  raw_ptr<views::Label> ai_response_label_;
  raw_ptr<views::ProgressBar> loading_bar_;
  raw_ptr<views::Button> describe_button_;
  raw_ptr<views::Button> capture_button_;
  raw_ptr<views::Button> close_button_;

  // Data
  ElementInfo element_info_;
  gfx::Image screenshot_;
  AIResponse ai_response_;
  bool loading_;

  base::WeakPtrFactory<TooltipView> weak_factory_{this};

  TooltipView(const TooltipView&) = delete;
  TooltipView& operator=(const TooltipView&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_VIEW_H_

