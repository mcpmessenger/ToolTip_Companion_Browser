// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/tooltip_service.h"

#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "chrome/browser/tooltip/element_detector.h"
#include "chrome/browser/tooltip/screenshot_capture.h"
#include "chrome/browser/tooltip/ai_integration.h"
#include "chrome/browser/tooltip/dark_mode_manager.h"
#include "chrome/browser/tooltip/navigrab_integration.h"
#include "chrome/browser/ui/views/tooltip/tooltip_view.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace tooltip {

// ElementInfo implementation
ElementInfo::ElementInfo() = default;
ElementInfo::~ElementInfo() = default;

// AIResponse implementation
AIResponse::AIResponse() : timestamp(0) {}
AIResponse::~AIResponse() = default;

// TooltipService implementation
TooltipService* TooltipService::GetInstance() {
  return base::Singleton<TooltipService>::get();
}

TooltipService::TooltipService()
    : initialized_(false), enabled_(true), tooltip_visible_(false) {}

TooltipService::~TooltipService() = default;

void TooltipService::Initialize() {
  if (initialized_) {
    return;
  }

  // DCHECK_CURRENTLY_ON is deprecated, using task runner check instead

  // Initialize preferences
  prefs_ = std::make_unique<TooltipPrefs>();
  prefs_->Initialize();

  // Initialize dark mode manager
  DarkModeManager::GetInstance()->Initialize();

  // Initialize components
  InitializeComponents();

  initialized_ = true;
  VLOG(1) << "TooltipService initialized";
}

void TooltipService::Shutdown() {
  if (!initialized_) {
    return;
  }

  // Hide any visible tooltip
  HideTooltip();

  // Shutdown components
  tooltip_view_.reset();
  ai_integration_.reset();
  screenshot_capture_.reset();
  element_detector_.reset();
  prefs_.reset();

  initialized_ = false;
  VLOG(1) << "TooltipService shutdown";
}

void TooltipService::InitializeComponents() {
  // Initialize element detector
  element_detector_ = std::make_unique<ElementDetector>();
  element_detector_->Initialize();

  // Initialize screenshot capture
  screenshot_capture_ = std::make_unique<ScreenshotCapture>();
  screenshot_capture_->Initialize();

  // Initialize AI integration
  ai_integration_ = std::make_unique<AIIntegration>();
  ai_integration_->Initialize();

  // Initialize tooltip view (anchor view will be set when showing)
  tooltip_view_ = std::make_unique<TooltipView>(nullptr);
  tooltip_view_->Initialize();

  // Initialize NaviGrab integration
  navigrab_integration_ = CreateNaviGrabIntegration();
  navigrab_integration_->Initialize();
}

void TooltipService::ShowTooltipForElement(
    content::WebContents* web_contents,
    const ElementInfo& element_info,
    const gfx::Point& mouse_position) {
  if (!enabled_ || !initialized_) {
    return;
  }

  // DCHECK_CURRENTLY_ON is deprecated, using task runner check instead

  // Hide any existing tooltip
  HideTooltip();

  // Set element information
  tooltip_view_->SetElementInfo(element_info);

  // Calculate tooltip position
  gfx::Size tooltip_size = tooltip_view_->GetPreferredSize();
  gfx::Size viewport_size = web_contents->GetContainerBounds().size();
  gfx::Rect tooltip_bounds = CalculateTooltipPosition(
      element_info.bounds, tooltip_size, viewport_size);

  // Show tooltip
  tooltip_view_->ShowAt(tooltip_bounds);
  tooltip_visible_ = true;

  // Notify observers
  NotifyTooltipShown(element_info);

  // Capture screenshot if auto-capture is enabled
  if (prefs_->GetAutoCapture()) {
    CaptureElementScreenshot(web_contents, element_info);
  }

  VLOG(1) << "Tooltip shown for element: " << element_info.tag_name;
}

void TooltipService::HideTooltip() {
  if (!tooltip_visible_) {
    return;
  }

  // DCHECK_CURRENTLY_ON is deprecated, using task runner check instead

  tooltip_view_->Hide();
  tooltip_visible_ = false;

  // Notify observers
  NotifyTooltipHidden();

  VLOG(1) << "Tooltip hidden";
}

void TooltipService::CaptureElementScreenshot(
    content::WebContents* web_contents,
    const ElementInfo& element_info) {
  if (!initialized_) {
    return;
  }

  // DCHECK_CURRENTLY_ON is deprecated, using task runner check instead

  // Capture screenshot asynchronously
  screenshot_capture_->CaptureElement(
      web_contents, element_info,
      base::BindOnce(&TooltipService::NotifyScreenshotCaptured,
                     base::Unretained(this)));
}

void TooltipService::GetAIDescription(const ElementInfo& element_info,
                                     const gfx::Image& screenshot) {
  if (!initialized_) {
    return;
  }

  // DCHECK_CURRENTLY_ON is deprecated, using task runner check instead

  // Get AI description asynchronously
  ai_integration_->GetDescription(
      element_info, screenshot,
      base::BindOnce(&TooltipService::NotifyAIResponseReceived,
                     base::Unretained(this)));
}

void TooltipService::AddObserver(TooltipObserver* observer) {
  observers_.AddObserver(observer);
}

void TooltipService::RemoveObserver(TooltipObserver* observer) {
  observers_.RemoveObserver(observer);
}

bool TooltipService::IsTooltipVisible() const {
  return tooltip_visible_;
}

void TooltipService::SetEnabled(bool enabled) {
  enabled_ = enabled;
  
  if (!enabled) {
    HideTooltip();
  }
  
  VLOG(1) << "TooltipService enabled: " << enabled;
}

bool TooltipService::IsEnabled() const {
  return enabled_;
}

gfx::Rect TooltipService::CalculateTooltipPosition(
    const gfx::Rect& element_bounds,
    const gfx::Size& tooltip_size,
    const gfx::Size& viewport_size) {
  // Default position: above the element
  gfx::Point position(element_bounds.x(), element_bounds.y() - tooltip_size.height() - 10);
  
  // Adjust if tooltip would go off screen
  if (position.y() < 0) {
    // Position below element instead
    position.set_y(element_bounds.bottom() + 10);
  }
  
  if (position.x() + tooltip_size.width() > viewport_size.width()) {
    // Adjust horizontal position
    position.set_x(viewport_size.width() - tooltip_size.width() - 10);
  }
  
  if (position.x() < 0) {
    position.set_x(10);
  }
  
  return gfx::Rect(position, tooltip_size);
}

void TooltipService::NotifyScreenshotCaptured(const gfx::Image& screenshot) {
  if (tooltip_view_) {
    tooltip_view_->SetScreenshot(screenshot);
  }
  
  // Notify observers
  for (auto& observer : observers_) {
    observer.OnScreenshotCaptured(screenshot);
  }
}

void TooltipService::NotifyAIResponseReceived(const AIResponse& response) {
  if (tooltip_view_) {
    tooltip_view_->SetAIResponse(response);
  }
  
  // Notify observers
  for (auto& observer : observers_) {
    observer.OnAIResponseReceived(response);
  }
}

void TooltipService::NotifyTooltipShown(const ElementInfo& element_info) {
  for (auto& observer : observers_) {
    observer.OnTooltipShown(element_info);
  }
}

void TooltipService::NotifyTooltipHidden() {
  for (auto& observer : observers_) {
    observer.OnTooltipHidden();
  }
}


void TooltipService::NotifyError(const std::string& error_message) {
  for (auto& observer : observers_) {
    observer.OnError(error_message);
  }
}

// NaviGrab automation integration methods
void TooltipService::ExecuteAutomationAction(
    const ElementInfo& element_info,
    const AutomationAction& action,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  if (!initialized_ || !navigrab_integration_) {
    AutomationResult result;
    result.success = false;
    result.error_message = "NaviGrab integration not available";
    std::move(callback).Run(result);
    return;
  }

  navigrab_integration_->ExecuteAction(element_info, action, std::move(callback));
}

std::vector<AutomationAction> TooltipService::GetAvailableActions(
    const ElementInfo& element_info) {
  
  if (!initialized_ || !navigrab_integration_) {
    return std::vector<AutomationAction>();
  }

  return navigrab_integration_->GetSuggestedActions(element_info);
}

void TooltipService::SetAutomationEnabled(bool enabled) {
  if (navigrab_integration_) {
    navigrab_integration_->SetEnabled(enabled);
  }
}

bool TooltipService::IsAutomationEnabled() const {
  if (!navigrab_integration_) {
    return false;
  }
  return navigrab_integration_->IsEnabled();
}

}  // namespace tooltip
