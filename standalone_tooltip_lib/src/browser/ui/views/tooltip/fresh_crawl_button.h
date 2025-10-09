// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLTIP_FRESH_CRAWL_BUTTON_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLTIP_FRESH_CRAWL_BUTTON_H_

#include "base/functional/callback.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/button/image_button.h"

namespace tooltip {

// Button for triggering fresh crawl functionality
class FreshCrawlButton : public views::ImageButton {
 public:
  explicit FreshCrawlButton(PressedCallback callback);
  ~FreshCrawlButton() override;

  // views::ImageButton overrides
  void OnThemeChanged() override;
  void OnMouseEntered(const ui::MouseEvent& event) override;
  void OnMouseExited(const ui::MouseEvent& event) override;

  // Update button state
  void SetEnabled(bool enabled);
  void SetLoading(bool loading);

 private:
  void UpdateButtonImage();
  void UpdateTooltipText();

  bool enabled_;
  bool loading_;
  PressedCallback callback_;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLTIP_FRESH_CRAWL_BUTTON_H_

