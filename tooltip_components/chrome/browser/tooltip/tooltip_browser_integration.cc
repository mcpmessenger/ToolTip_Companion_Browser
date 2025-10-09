// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/tooltip_browser_integration.h"

#ifdef STANDALONE_TOOLTIP_BUILD
#include "base/base_stubs.h"
#else
#include "base/logging.h"
#endif
#include "chrome/browser/tooltip/tooltip_service.h"
#include "chrome/browser/tooltip/tooltip_prefs.h"
#ifdef STANDALONE_TOOLTIP_BUILD
#include "base/base_stubs.h"
#else
#include "components/prefs/pref_registry_simple.h"
#endif

namespace tooltip {

void TooltipBrowserIntegration::InitializeForBrowserContext(
    content::BrowserContext* context) {
  DCHECK(context);
  
  VLOG(1) << "Initializing tooltip service for browser context";
  
  // Initialize the tooltip service
  TooltipService::GetInstance()->Initialize();
}

void TooltipBrowserIntegration::ShutdownForBrowserContext(
    content::BrowserContext* context) {
  DCHECK(context);
  
  VLOG(1) << "Shutting down tooltip service for browser context";
  
  // Shutdown the tooltip service
  TooltipService::GetInstance()->Shutdown();
}

void TooltipBrowserIntegration::RegisterProfilePrefs() {
  VLOG(1) << "Registering tooltip profile preferences";
  
  // This will be called from the main browser initialization
  // to register our preferences with the preference system
}

}  // namespace tooltip

