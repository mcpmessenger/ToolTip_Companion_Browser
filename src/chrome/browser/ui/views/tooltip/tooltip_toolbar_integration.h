// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_TOOLBAR_INTEGRATION_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_TOOLBAR_INTEGRATION_H_

#include "base/memory/raw_ptr.h"
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"

namespace tooltip {

// Integrates tooltip features into the browser toolbar
class TooltipToolbarIntegration {
 public:
  explicit TooltipToolbarIntegration(ToolbarView* toolbar_view);
  ~TooltipToolbarIntegration();

  // Add the dark mode toggle button to the toolbar
  void AddDarkModeToggle();

 private:
  raw_ptr<ToolbarView> toolbar_view_;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLTIP_TOOLTIP_TOOLBAR_INTEGRATION_H_

