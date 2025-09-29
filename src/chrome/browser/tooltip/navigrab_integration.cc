// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "navigrab_integration.h"

#include <algorithm>
#include <sstream>

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/codec/png_codec.h"

namespace tooltip {

// AutomationAction implementation
AutomationAction::AutomationAction() 
    : type(AutomationActionType::CLICK_ELEMENT),
      timeout_ms(5000) {}

AutomationAction::~AutomationAction() = default;

// Copy constructor and assignment operator
AutomationAction::AutomationAction(const AutomationAction& other)
    : type(other.type),
      selector(other.selector),
      text_input(other.text_input),
      script_code(other.script_code),
      form_data(other.form_data),
      timeout_ms(other.timeout_ms),
      attribute_name(other.attribute_name) {}

AutomationAction& AutomationAction::operator=(const AutomationAction& other) {
  if (this != &other) {
    type = other.type;
    selector = other.selector;
    text_input = other.text_input;
    script_code = other.script_code;
    form_data = other.form_data;
    timeout_ms = other.timeout_ms;
    attribute_name = other.attribute_name;
  }
  return *this;
}

// Move constructor and assignment operator
AutomationAction::AutomationAction(AutomationAction&& other) noexcept
    : type(std::move(other.type)),
      selector(std::move(other.selector)),
      text_input(std::move(other.text_input)),
      script_code(std::move(other.script_code)),
      form_data(std::move(other.form_data)),
      timeout_ms(std::move(other.timeout_ms)),
      attribute_name(std::move(other.attribute_name)) {}

AutomationAction& AutomationAction::operator=(AutomationAction&& other) noexcept {
  if (this != &other) {
    type = std::move(other.type);
    selector = std::move(other.selector);
    text_input = std::move(other.text_input);
    script_code = std::move(other.script_code);
    form_data = std::move(other.form_data);
    timeout_ms = std::move(other.timeout_ms);
    attribute_name = std::move(other.attribute_name);
  }
  return *this;
}

// AutomationResult implementation
AutomationResult::AutomationResult() 
    : success(false),
      execution_time_ms(0) {}

AutomationResult::~AutomationResult() = default;

// Copy constructor and assignment operator
AutomationResult::AutomationResult(const AutomationResult& other)
    : success(other.success),
      error_message(other.error_message),
      result_data(other.result_data),
      screenshot(other.screenshot),
      execution_time_ms(other.execution_time_ms) {}

AutomationResult& AutomationResult::operator=(const AutomationResult& other) {
  if (this != &other) {
    success = other.success;
    error_message = other.error_message;
    result_data = other.result_data;
    screenshot = other.screenshot;
    execution_time_ms = other.execution_time_ms;
  }
  return *this;
}

// Move constructor and assignment operator
AutomationResult::AutomationResult(AutomationResult&& other) noexcept
    : success(std::move(other.success)),
      error_message(std::move(other.error_message)),
      result_data(std::move(other.result_data)),
      screenshot(std::move(other.screenshot)),
      execution_time_ms(std::move(other.execution_time_ms)) {}

AutomationResult& AutomationResult::operator=(AutomationResult&& other) noexcept {
  if (this != &other) {
    success = std::move(other.success);
    error_message = std::move(other.error_message);
    result_data = std::move(other.result_data);
    screenshot = std::move(other.screenshot);
    execution_time_ms = std::move(other.execution_time_ms);
  }
  return *this;
}

// NaviGrabIntegration implementation
NaviGrabIntegration::NaviGrabIntegration()
    : initialized_(false),
      enabled_(true),
      monitoring_(false),
      default_timeout_ms_(5000) {}

NaviGrabIntegration::~NaviGrabIntegration() {
  StopMonitoring();
}

void NaviGrabIntegration::Initialize() {
  if (initialized_) {
    return;
  }

  LOG(INFO) << "Initializing NaviGrab integration for ChromiumFresh tooltips";
  
  InitializeNaviGrab();
  initialized_ = true;
  
  LOG(INFO) << "NaviGrab integration initialized successfully";
}

void NaviGrabIntegration::InitializeNaviGrab() {
  // Create NaviGrab components
  web_automation_ = navigrab::CreateWebAutomation();
  browser_ = web_automation_->CreateBrowser();
  page_ = web_automation_->CreatePage();
  screenshot_capture_ = web_automation_->CreateScreenshotCapture();
  
  LOG(INFO) << "NaviGrab components created successfully";
}

void NaviGrabIntegration::StartMonitoring(content::WebContents* web_contents) {
  if (!initialized_ || !enabled_) {
    return;
  }

  if (monitoring_ && web_contents == web_contents()) {
    return; // Already monitoring this WebContents
  }

  StopMonitoring();
  Observe(web_contents);
  monitoring_ = true;
  
  LOG(INFO) << "Started monitoring WebContents for automation";
}

void NaviGrabIntegration::StopMonitoring() {
  if (monitoring_) {
    Observe(nullptr);
    monitoring_ = false;
    LOG(INFO) << "Stopped monitoring WebContents";
  }
}

void NaviGrabIntegration::ExecuteAction(
    const ElementInfo& element_info,
    const AutomationAction& action,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  if (!initialized_ || !enabled_) {
    AutomationResult result;
    result.success = false;
    result.error_message = "NaviGrab integration not initialized or disabled";
    std::move(callback).Run(result);
    return;
  }

  base::TimeTicks start_time = base::TimeTicks::Now();
  
  switch (action.type) {
    case AutomationActionType::CLICK_ELEMENT:
      ClickElement(element_info, std::move(callback));
      break;
      
    case AutomationActionType::TYPE_TEXT:
      TypeText(element_info, action.text_input, std::move(callback));
      break;
      
    case AutomationActionType::HOVER_ELEMENT:
      HoverElement(element_info, std::move(callback));
      break;
      
    case AutomationActionType::CAPTURE_SCREENSHOT:
      CaptureElementScreenshot(element_info, std::move(callback));
      break;
      
    case AutomationActionType::FILL_FORM:
      FillForm(element_info, action.form_data, std::move(callback));
      break;
      
    case AutomationActionType::NAVIGATE_TO_LINK:
      NavigateToLink(element_info, std::move(callback));
      break;
      
    case AutomationActionType::EXECUTE_SCRIPT:
      ExecuteScriptOnElement(element_info, action.script_code, std::move(callback));
      break;
      
    case AutomationActionType::WAIT_FOR_ELEMENT:
      WaitForElement(action.selector, action.timeout_ms, std::move(callback));
      break;
      
    case AutomationActionType::GET_ELEMENT_TEXT:
      GetElementText(element_info, std::move(callback));
      break;
      
    case AutomationActionType::GET_ELEMENT_ATTRIBUTE:
      GetElementAttribute(element_info, action.attribute_name, std::move(callback));
      break;
      
    default:
      AutomationResult result;
      result.success = false;
      result.error_message = "Unknown automation action type";
      std::move(callback).Run(result);
      break;
  }
}

std::vector<AutomationAction> NaviGrabIntegration::GetSuggestedActions(
    const ElementInfo& element_info) const {
  
  return GetActionsForElementType(element_info.tag_name, 
                                 "", // type attribute
                                 element_info.role);
}

bool NaviGrabIntegration::CanAutomateElement(const ElementInfo& element_info) const {
  if (!initialized_ || !enabled_) {
    return false;
  }

  // Check if element is interactive
  const std::vector<std::string> interactive_tags = {
    "a", "button", "input", "select", "textarea", "form", "label"
  };
  
  std::string tag_lower = base::ToLowerASCII(element_info.tag_name);
  return std::find(interactive_tags.begin(), interactive_tags.end(), tag_lower) 
         != interactive_tags.end();
}

void NaviGrabIntegration::ClickElement(
    const ElementInfo& element_info,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  bool success = page_->ClickElement(selector);
  AutomationResult result = ProcessResult(success,
                                        success ? "Element clicked successfully" : "",
                                        success ? "" : "Failed to click element");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::TypeText(
    const ElementInfo& element_info,
    const std::string& text,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  bool success = page_->TypeText(selector, text);
  AutomationResult result = ProcessResult(success,
                                        success ? "Text typed successfully" : "",
                                        success ? "" : "Failed to type text");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::HoverElement(
    const ElementInfo& element_info,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  bool success = page_->HoverElement(selector);
  AutomationResult result = ProcessResult(success,
                                        success ? "Element hovered successfully" : "",
                                        success ? "" : "Failed to hover element");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::CaptureElementScreenshot(
    const ElementInfo& element_info,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  std::string filename = "element_screenshot_" + std::to_string(base::Time::Now().ToTimeT()) + ".png";
  
  bool success = screenshot_capture_->CaptureElement(selector, filename);
  AutomationResult result = ProcessResult(success,
                                        success ? filename : "",
                                        success ? "" : "Failed to capture element screenshot");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::CapturePageScreenshot(
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string filename = "page_screenshot_" + std::to_string(base::Time::Now().ToTimeT()) + ".png";
  
  bool success = screenshot_capture_->CapturePage(filename);
  AutomationResult result = ProcessResult(success,
                                        success ? filename : "",
                                        success ? "" : "Failed to capture page screenshot");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::FillForm(
    const ElementInfo& element_info,
    const std::map<std::string, std::string>& form_data,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  bool success = web_automation_->FillForm(selector, form_data);
  AutomationResult result = ProcessResult(success,
                                        success ? "Form filled successfully" : "",
                                        success ? "" : "Failed to fill form");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::SubmitForm(
    const ElementInfo& element_info,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  bool success = web_automation_->SubmitForm(selector);
  AutomationResult result = ProcessResult(success,
                                        success ? "Form submitted successfully" : "",
                                        success ? "" : "Failed to submit form");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::NavigateToLink(
    const ElementInfo& element_info,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  if (element_info.href.empty()) {
    AutomationResult result = ProcessResult(false, "", "Element has no href attribute");
    std::move(callback).Run(result);
    return;
  }
  
  bool success = browser_->NavigateTo(element_info.href);
  AutomationResult result = ProcessResult(success,
                                        success ? "Navigation successful" : "",
                                        success ? "" : "Failed to navigate");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::ExecuteScript(
    const std::string& script,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string result_data = web_automation_->ExecuteScript(script);
  AutomationResult result = ProcessResult(true, result_data, "");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::ExecuteScriptOnElement(
    const ElementInfo& element_info,
    const std::string& script,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  // Use Page's EvaluateScript method instead
  std::string result_data = page_->EvaluateScript(script);
  AutomationResult result = ProcessResult(true, result_data, "");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::GetElementText(
    const ElementInfo& element_info,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  // Use Locator's GetText method instead
  std::unique_ptr<navigrab::Locator> locator = std::make_unique<navigrab::Locator>();
  std::string text = locator->GetText(selector);
  AutomationResult result = ProcessResult(true, text, "");
  std::move(callback).Run(result);
}

void NaviGrabIntegration::GetElementAttribute(
    const ElementInfo& element_info,
    const std::string& attribute_name,
    base::OnceCallback<void(const AutomationResult&)> callback) {
  
  std::string selector = CreateSelector(element_info);
  
  // Use Locator's GetAttribute method instead
  std::unique_ptr<navigrab::Locator> locator = std::make_unique<navigrab::Locator>();
  std::string attribute_value = locator->GetAttribute(selector, attribute_name);
  AutomationResult result = ProcessResult(true, attribute_value, "");
  std::move(callback).Run(result);
}

std::string NaviGrabIntegration::CreateSelector(const ElementInfo& element_info) const {
  std::stringstream selector;
  
  // Start with tag name
  selector << element_info.tag_name;
  
  // Add ID if available
  if (!element_info.id.empty()) {
    selector << "#" << element_info.id;
    return selector.str(); // ID is unique, return early
  }
  
  // Add class if available
  if (!element_info.class_name.empty()) {
    std::string class_name = element_info.class_name;
    base::ReplaceChars(class_name, " ", ".", &class_name);
    selector << "." << class_name;
  }
  
  // Add attribute selectors for more specificity
  if (!element_info.href.empty()) {
    selector << "[href=\"" << element_info.href << "\"]";
  }
  
  if (!element_info.src.empty()) {
    selector << "[src=\"" << element_info.src << "\"]";
  }
  
  if (!element_info.title.empty()) {
    selector << "[title=\"" << element_info.title << "\"]";
  }
  
  return selector.str();
}

AutomationResult NaviGrabIntegration::ProcessResult(
    bool success, 
    const std::string& result_data,
    const std::string& error_message) const {
  
  AutomationResult result;
  result.success = success;
  result.result_data = result_data;
  result.error_message = error_message;
  result.execution_time_ms = 0; // TODO: Add timing
  
  return result;
}

std::vector<AutomationAction> NaviGrabIntegration::GetActionsForElementType(
    const std::string& tag_name,
    const std::string& type,
    const std::string& role) const {
  
  std::vector<AutomationAction> actions;
  std::string tag_lower = base::ToLowerASCII(tag_name);
  
  // Common actions for all interactive elements
  AutomationAction click_action;
  click_action.type = AutomationActionType::CLICK_ELEMENT;
  actions.push_back(click_action);
  
  AutomationAction hover_action;
  hover_action.type = AutomationActionType::HOVER_ELEMENT;
  actions.push_back(hover_action);
  
  AutomationAction screenshot_action;
  screenshot_action.type = AutomationActionType::CAPTURE_SCREENSHOT;
  actions.push_back(screenshot_action);
  
  // Element-specific actions
  if (tag_lower == "input" || tag_lower == "textarea") {
    AutomationAction type_action;
    type_action.type = AutomationActionType::TYPE_TEXT;
    actions.push_back(type_action);
  }
  
  if (tag_lower == "form") {
    AutomationAction fill_action;
    fill_action.type = AutomationActionType::FILL_FORM;
    actions.push_back(fill_action);
  }
  
  if (tag_lower == "a") {
    AutomationAction navigate_action;
    navigate_action.type = AutomationActionType::NAVIGATE_TO_LINK;
    actions.push_back(navigate_action);
  }
  
  return actions;
}

void NaviGrabIntegration::SetEnabled(bool enabled) {
  enabled_ = enabled;
  LOG(INFO) << "NaviGrab integration " << (enabled ? "enabled" : "disabled");
}

bool NaviGrabIntegration::IsEnabled() const {
  return enabled_;
}

void NaviGrabIntegration::SetTimeout(int timeout_ms) {
  default_timeout_ms_ = timeout_ms;
}

int NaviGrabIntegration::GetTimeout() const {
  return default_timeout_ms_;
}

void NaviGrabIntegration::DidFinishLoad(content::RenderFrameHost* render_frame_host,
                                       const GURL& validated_url) {
  LOG(INFO) << "Page loaded, NaviGrab ready for automation: " << validated_url.spec();
}

void NaviGrabIntegration::WebContentsDestroyed() {
  StopMonitoring();
}

// Factory function
std::unique_ptr<NaviGrabIntegration> CreateNaviGrabIntegration() {
  return std::make_unique<NaviGrabIntegration>();
}

}  // namespace tooltip
