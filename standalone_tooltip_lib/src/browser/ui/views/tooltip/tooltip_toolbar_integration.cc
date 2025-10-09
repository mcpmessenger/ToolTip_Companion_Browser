// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/tooltip/tooltip_toolbar_integration.h"

#include "base/logging.h"
#include "base/functional/bind.h"
#include "chrome/browser/tooltip/dark_mode_manager.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "chrome/browser/ui/views/tooltip/fresh_crawl_button.h"
// #include "chrome/browser/ui/views/tooltip/dark_mode_toggle_button.h"  // Removed for now
#include "chrome/browser/ui/views/toolbar/toolbar_view.h"

namespace tooltip {

TooltipToolbarIntegration::TooltipToolbarIntegration(ToolbarView* toolbar_view)
    : toolbar_view_(toolbar_view),
      fresh_crawl_button_(nullptr) {
  LOG(INFO) << "🔧 TOOLTIP: TooltipToolbarIntegration created";
}

TooltipToolbarIntegration::~TooltipToolbarIntegration() {
  LOG(INFO) << "🔧 TOOLTIP: TooltipToolbarIntegration destroyed";
}

void TooltipToolbarIntegration::AddDarkModeToggle() {
  LOG(INFO) << "🔧 TOOLTIP: Adding dark mode toggle to toolbar";
  
  // TODO: Implement dark mode toggle button
  // For now, just log that we would add a dark mode toggle
  LOG(INFO) << "🔧 TOOLTIP: Dark mode toggle functionality ready (button implementation pending)";
  
  // The actual toolbar integration would require more complex layout management
}

void TooltipToolbarIntegration::AddFreshCrawlButton() {
  LOG(INFO) << "🔧 TOOLTIP: Adding fresh crawl button to toolbar";
  
  if (!toolbar_view_) {
    LOG(ERROR) << "🔧 TOOLTIP: No toolbar view available for fresh crawl button";
    return;
  }
  
  // Create the fresh crawl button
  fresh_crawl_button_ = new FreshCrawlButton(
      base::BindRepeating(&TooltipToolbarIntegration::OnFreshCrawlClicked,
                         base::Unretained(this)));
  
  // Add button to toolbar (this would need proper toolbar integration)
  // For now, just log that we would add the button
  LOG(INFO) << "🔧 TOOLTIP: Fresh crawl button created and ready for toolbar integration";
  
  // TODO: Actually add the button to the toolbar layout
  // This would require integration with the specific toolbar implementation
}

void TooltipToolbarIntegration::OnFreshCrawlClicked() {
  LOG(INFO) << "🔧 TOOLTIP: Fresh crawl button clicked";
  
  if (!fresh_crawl_button_) {
    return;
  }
  
  // Set loading state
  fresh_crawl_button_->SetLoading(true);
  
  // Get tooltip service and perform fresh crawl
  TooltipService* service = TooltipService::GetInstance();
  if (service) {
    service->PerformFreshCrawl(
        base::BindOnce(&TooltipToolbarIntegration::OnFreshCrawlComplete,
                      base::Unretained(this)));
  } else {
    LOG(ERROR) << "🔧 TOOLTIP: TooltipService not available for fresh crawl";
    fresh_crawl_button_->SetLoading(false);
  }
}

void TooltipToolbarIntegration::OnFreshCrawlComplete(bool success) {
  LOG(INFO) << "🔧 TOOLTIP: Fresh crawl completed with success: " << success;
  
  if (fresh_crawl_button_) {
    fresh_crawl_button_->SetLoading(false);
  }
}

}  // namespace tooltip
