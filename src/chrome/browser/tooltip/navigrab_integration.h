// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_NAVIGRAB_INTEGRATION_H_
#define CHROME_BROWSER_TOOLTIP_NAVIGRAB_INTEGRATION_H_

#include <memory>
#include <string>
#include <vector>
#include <map>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "content/public/browser/web_contents_observer.h"
#include "ui/gfx/image/image.h"

// Forward declarations
namespace tooltip {
struct ElementInfo;
}

// Include NaviGrab headers
#include "src/navigrab/navigrab_core.h"

namespace content {
class WebContents;
}

namespace tooltip {

// Automation action types
enum class AutomationActionType {
  CLICK_ELEMENT,
  TYPE_TEXT,
  HOVER_ELEMENT,
  CAPTURE_SCREENSHOT,
  FILL_FORM,
  NAVIGATE_TO_LINK,
  EXECUTE_SCRIPT,
  WAIT_FOR_ELEMENT,
  GET_ELEMENT_TEXT,
  GET_ELEMENT_ATTRIBUTE
};

// Automation action data
struct AutomationAction {
  AutomationActionType type;
  std::string selector;
  std::string text_input;
  std::string script_code;
  std::map<std::string, std::string> form_data;
  int timeout_ms;
  std::string attribute_name;
  
  AutomationAction();
  ~AutomationAction();
  
  // Copy constructor and assignment operator
  AutomationAction(const AutomationAction& other);
  AutomationAction& operator=(const AutomationAction& other);
  
  // Move constructor and assignment operator
  AutomationAction(AutomationAction&& other) noexcept;
  AutomationAction& operator=(AutomationAction&& other) noexcept;
};

// Automation result
struct AutomationResult {
  bool success;
  std::string error_message;
  std::string result_data;
  gfx::Image screenshot;
  int64_t execution_time_ms;
  
  AutomationResult();
  ~AutomationResult();
  
  // Copy constructor and assignment operator
  AutomationResult(const AutomationResult& other);
  AutomationResult& operator=(const AutomationResult& other);
  
  // Move constructor and assignment operator
  AutomationResult(AutomationResult&& other) noexcept;
  AutomationResult& operator=(AutomationResult&& other) noexcept;
};

// Bridge between ChromiumFresh tooltip system and NaviGrab automation
class NaviGrabIntegration : public content::WebContentsObserver {
 public:
  NaviGrabIntegration();
  ~NaviGrabIntegration() override;

  // Initialize NaviGrab integration
  void Initialize();

  // Start monitoring a web contents for automation
  void StartMonitoring(content::WebContents* web_contents);

  // Stop monitoring
  void StopMonitoring();

  // Execute automation actions
  void ExecuteAction(const ElementInfo& element_info,
                    const AutomationAction& action,
                    base::OnceCallback<void(const AutomationResult&)> callback);

  // Get suggested automation actions for an element
  std::vector<AutomationAction> GetSuggestedActions(const ElementInfo& element_info) const;

  // Check if automation is available for element
  bool CanAutomateElement(const ElementInfo& element_info) const;

  // Element interaction methods
  void ClickElement(const ElementInfo& element_info,
                   base::OnceCallback<void(const AutomationResult&)> callback);
  
  void TypeText(const ElementInfo& element_info,
               const std::string& text,
               base::OnceCallback<void(const AutomationResult&)> callback);
  
  void HoverElement(const ElementInfo& element_info,
                   base::OnceCallback<void(const AutomationResult&)> callback);

  // Screenshot methods
  void CaptureElementScreenshot(const ElementInfo& element_info,
                               base::OnceCallback<void(const AutomationResult&)> callback);
  
  void CapturePageScreenshot(base::OnceCallback<void(const AutomationResult&)> callback);

  // Form interaction methods
  void FillForm(const ElementInfo& element_info,
               const std::map<std::string, std::string>& form_data,
               base::OnceCallback<void(const AutomationResult&)> callback);
  
  void SubmitForm(const ElementInfo& element_info,
                 base::OnceCallback<void(const AutomationResult&)> callback);

  // Navigation methods
  void NavigateToLink(const ElementInfo& element_info,
                     base::OnceCallback<void(const AutomationResult&)> callback);
  
  void GoBack(base::OnceCallback<void(const AutomationResult&)> callback);
  void GoForward(base::OnceCallback<void(const AutomationResult&)> callback);
  void Reload(base::OnceCallback<void(const AutomationResult&)> callback);

  // JavaScript execution
  void ExecuteScript(const std::string& script,
                    base::OnceCallback<void(const AutomationResult&)> callback);
  
  void ExecuteScriptOnElement(const ElementInfo& element_info,
                             const std::string& script,
                             base::OnceCallback<void(const AutomationResult&)> callback);

  // Wait functions
  void WaitForElement(const std::string& selector,
                     int timeout_ms,
                     base::OnceCallback<void(const AutomationResult&)> callback);
  
  void WaitForNavigation(int timeout_ms,
                        base::OnceCallback<void(const AutomationResult&)> callback);

  // Content extraction
  void GetElementText(const ElementInfo& element_info,
                     base::OnceCallback<void(const AutomationResult&)> callback);
  
  void GetElementAttribute(const ElementInfo& element_info,
                          const std::string& attribute_name,
                          base::OnceCallback<void(const AutomationResult&)> callback);

  // Utility methods
  void GetPageSource(base::OnceCallback<void(const AutomationResult&)> callback);
  void GetLinks(base::OnceCallback<void(const AutomationResult&)> callback);
  void GetImages(base::OnceCallback<void(const AutomationResult&)> callback);

  // Settings
  void SetEnabled(bool enabled);
  bool IsEnabled() const;
  
  void SetTimeout(int timeout_ms);
  int GetTimeout() const;

  // WebContentsObserver overrides
  void DidFinishLoad(content::RenderFrameHost* render_frame_host,
                    const GURL& validated_url) override;
  void WebContentsDestroyed() override;

 private:
  // Initialize NaviGrab components
  void InitializeNaviGrab();

  // Create CSS selector from ElementInfo
  std::string CreateSelector(const ElementInfo& element_info) const;

  // Convert ElementInfo to automation context
  std::string CreateAutomationContext(const ElementInfo& element_info) const;

  // Process automation result
  AutomationResult ProcessResult(bool success, 
                                const std::string& result_data = "",
                                const std::string& error_message = "") const;

  // Handle automation completion
  void OnAutomationComplete(base::OnceCallback<void(const AutomationResult&)> callback,
                           bool success,
                           const std::string& result_data,
                           const std::string& error_message);

  // Handle screenshot completion
  void OnScreenshotComplete(base::OnceCallback<void(const AutomationResult&)> callback,
                           const gfx::Image& screenshot);

  // Determine suggested actions based on element type
  std::vector<AutomationAction> GetActionsForElementType(const std::string& tag_name,
                                                        const std::string& type,
                                                        const std::string& role) const;

  // NaviGrab components
  std::unique_ptr<navigrab::WebAutomation> web_automation_;
  std::unique_ptr<navigrab::Browser> browser_;
  std::unique_ptr<navigrab::Page> page_;
  std::unique_ptr<navigrab::ScreenshotCapture> screenshot_capture_;

  // State
  bool initialized_;
  bool enabled_;
  bool monitoring_;
  int default_timeout_ms_;
  
  base::WeakPtrFactory<NaviGrabIntegration> weak_factory_{this};

  NaviGrabIntegration(const NaviGrabIntegration&) = delete;
  NaviGrabIntegration& operator=(const NaviGrabIntegration&) = delete;
};

// Factory function
std::unique_ptr<NaviGrabIntegration> CreateNaviGrabIntegration();

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_NAVIGRAB_INTEGRATION_H_
