// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_SERVICE_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_SERVICE_H_

#include <memory>
#include <string>
#include <vector>

#ifdef STANDALONE_TOOLTIP_BUILD
#include "base/base_stubs.h"
#else
#include "base/memory/singleton.h"
#include "base/observer_list.h"
#include "base/values.h"
#endif
#include "chrome/browser/tooltip/tooltip_prefs.h"
#include "chrome/browser/tooltip/dark_mode_manager.h"
#include "chrome/browser/tooltip/navigrab_integration.h"
#ifdef STANDALONE_TOOLTIP_BUILD
#include "base/base_stubs.h"
#else
#include "content/public/browser/web_contents_observer.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/image/image.h"
#endif

namespace content {
class WebContents;
}

namespace blink {
class Element;
}

namespace tooltip {

class ElementDetector;
class ScreenshotCapture;
class AIIntegration;
class TooltipView;

// Forward declarations for automation types
struct AutomationAction;
struct AutomationResult;

// Information about a detected element
struct ElementInfo {
  std::string tag_name;
  std::string id;
  std::string class_name;
  std::string text_content;
  std::string href;
  std::string src;
  std::string alt_text;
  std::string title;
  std::string role;
  std::string aria_label;
  std::string type;
  gfx::Rect bounds;
  std::string computed_styles;
  
  ElementInfo();
  ~ElementInfo();
};

// AI response data
struct AIResponse {
  std::string provider;
  std::string description;
  std::string confidence;
  int64_t timestamp;
  std::vector<std::string> suggested_actions;
  
  AIResponse();
  ~AIResponse();
};

// Observer for tooltip events
class TooltipObserver : public base::CheckedObserver {
 public:
  virtual void OnTooltipShown(const ElementInfo& element_info) {}
  virtual void OnTooltipHidden() {}
  virtual void OnScreenshotCaptured(const gfx::Image& screenshot) {}
  virtual void OnAIResponseReceived(const AIResponse& response) {}
  virtual void OnError(const std::string& error_message) {}
};

// Main tooltip service that manages tooltip functionality
class TooltipService : public base::Singleton<TooltipService> {
 public:
  static TooltipService* GetInstance();

  // Initialize the service
  void Initialize();

  // Shutdown the service
  void Shutdown();

  // Show tooltip for an element
  void ShowTooltipForElement(content::WebContents* web_contents,
                            const ElementInfo& element_info,
                            const gfx::Point& mouse_position);

  // Hide current tooltip
  void HideTooltip();

  // Capture screenshot of element
  void CaptureElementScreenshot(content::WebContents* web_contents,
                               const ElementInfo& element_info);

  // Get AI description for element
  void GetAIDescription(const ElementInfo& element_info,
                       const gfx::Image& screenshot);

  // Observer management
  void AddObserver(TooltipObserver* observer);
  void RemoveObserver(TooltipObserver* observer);

  // Settings management
  TooltipPrefs* GetPrefs() { return prefs_.get(); }
  
  // Dark mode management
  DarkModeManager* GetDarkModeManager() { return DarkModeManager::GetInstance(); }

  // Check if tooltip is currently visible
  bool IsTooltipVisible() const;

  // Enable/disable tooltip functionality
  void SetEnabled(bool enabled);
  bool IsEnabled() const;

  // NaviGrab automation integration
  void ExecuteAutomationAction(const ElementInfo& element_info,
                              const AutomationAction& action,
                              base::OnceCallback<void(const AutomationResult&)> callback);
  std::vector<AutomationAction> GetAvailableActions(const ElementInfo& element_info);
  void SetAutomationEnabled(bool enabled);
  bool IsAutomationEnabled() const;
  class NaviGrabIntegration* GetNaviGrabIntegration() { return navigrab_integration_.get(); }

 private:
  friend struct base::DefaultSingletonTraits<TooltipService>;

  TooltipService();
  ~TooltipService();

  // Initialize components
  void InitializeComponents();

  // Handle tooltip positioning
  gfx::Rect CalculateTooltipPosition(const gfx::Rect& element_bounds,
                                    const gfx::Size& tooltip_size,
                                    const gfx::Size& viewport_size);

  // Notify observers
  void NotifyTooltipShown(const ElementInfo& element_info);
  void NotifyTooltipHidden();
  void NotifyScreenshotCaptured(const gfx::Image& screenshot);
  void NotifyAIResponseReceived(const AIResponse& response);
  void NotifyError(const std::string& error_message);

  // Component instances
  std::unique_ptr<ElementDetector> element_detector_;
  std::unique_ptr<ScreenshotCapture> screenshot_capture_;
  std::unique_ptr<AIIntegration> ai_integration_;
  std::unique_ptr<TooltipView> tooltip_view_;
  std::unique_ptr<TooltipPrefs> prefs_;
  std::unique_ptr<class NaviGrabIntegration> navigrab_integration_;

  // State
  bool initialized_;
  bool enabled_;
  bool tooltip_visible_;
  base::ObserverList<TooltipObserver> observers_;

  TooltipService(const TooltipService&) = delete;
  TooltipService& operator=(const TooltipService&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_SERVICE_H_
