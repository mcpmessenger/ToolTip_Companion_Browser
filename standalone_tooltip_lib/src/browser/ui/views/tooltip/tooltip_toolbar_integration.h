// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_TOOLBAR_INTEGRATION_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_TOOLBAR_INTEGRATION_H_

#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"

namespace tooltip {

class FreshCrawlButton;

// Integrates tooltip features into the browser toolbar
class TooltipToolbarIntegration {
 public:
  explicit TooltipToolbarIntegration(ToolbarView* toolbar_view);
  ~TooltipToolbarIntegration();

  // Add the dark mode toggle button to the toolbar
  void AddDarkModeToggle();
  
  // Add the fresh crawl button to the toolbar
  void AddFreshCrawlButton();

 private:
  // Fresh crawl button callbacks
  void OnFreshCrawlClicked();
  void OnFreshCrawlComplete(bool success);

  raw_ptr<ToolbarView> toolbar_view_;
  raw_ptr<FreshCrawlButton> fresh_crawl_button_;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_TOOLBAR_INTEGRATION_H_

