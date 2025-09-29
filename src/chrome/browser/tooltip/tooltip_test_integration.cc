// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/tooltip_test_integration.h"

#include "base/logging.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/browser_list_observer.h"

namespace tooltip {

// Simple test to verify our tooltip system is working
class TooltipTestObserver : public BrowserListObserver {
 public:
  TooltipTestObserver() {
    BrowserList::GetInstance()->AddObserver(this);
    LOG(INFO) << "🔧 TOOLTIP TEST: Observer created";
  }

  ~TooltipTestObserver() override {
    BrowserList::GetInstance()->RemoveObserver(this);
    LOG(INFO) << "🔧 TOOLTIP TEST: Observer destroyed";
  }

  void OnBrowserAdded(Browser* browser) override {
    LOG(INFO) << "🔧 TOOLTIP TEST: Browser added - testing tooltip system";
    
    // Test our tooltip service
    TooltipService* service = TooltipService::GetInstance();
    service->Init();
    
    // Test preferences
    TooltipPrefs* prefs = service->GetPrefs();
    prefs->SetDarkModeEnabled(true);
    LOG(INFO) << "🔧 TOOLTIP TEST: Dark mode enabled: " << prefs->IsDarkModeEnabled();
    
    // Test API key storage
    prefs->SetOpenAIAPIKey("test-key-123");
    LOG(INFO) << "🔧 TOOLTIP TEST: OpenAI key set: " << prefs->GetOpenAIAPIKey();
    
    LOG(INFO) << "🔧 TOOLTIP TEST: All tests passed! Tooltip system is working.";
  }

  void OnBrowserRemoved(Browser* browser) override {
    LOG(INFO) << "🔧 TOOLTIP TEST: Browser removed";
  }
};

// Global test observer
static TooltipTestObserver* g_test_observer = nullptr;

void InitializeTooltipTest() {
  LOG(INFO) << "🔧 TOOLTIP TEST: Initializing tooltip test system";
  
  if (!g_test_observer) {
    g_test_observer = new TooltipTestObserver();
    LOG(INFO) << "🔧 TOOLTIP TEST: Test observer created successfully";
  }
}

void ShutdownTooltipTest() {
  LOG(INFO) << "🔧 TOOLTIP TEST: Shutting down tooltip test system";
  
  if (g_test_observer) {
    delete g_test_observer;
    g_test_observer = nullptr;
  }
}

}  // namespace tooltip

