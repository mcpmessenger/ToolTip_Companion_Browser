// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_BROWSER_INTEGRATION_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_BROWSER_INTEGRATION_H_

#include "base/check.h"

namespace content {
class BrowserContext;
}

namespace tooltip {

// Integrates tooltip functionality with the browser
class TooltipBrowserIntegration {
 public:
  // Initialize tooltip service for a browser context
  static void InitializeForBrowserContext(content::BrowserContext* context);

  // Shutdown tooltip service for a browser context
  static void ShutdownForBrowserContext(content::BrowserContext* context);

  // Register tooltip preferences
  static void RegisterProfilePrefs();

 private:
  TooltipBrowserIntegration() = delete;
  TooltipBrowserIntegration(const TooltipBrowserIntegration&) = delete;
  TooltipBrowserIntegration& operator=(const TooltipBrowserIntegration&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_BROWSER_INTEGRATION_H_

