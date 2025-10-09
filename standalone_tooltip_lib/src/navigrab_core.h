#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace navigrab {

// Forward declarations
class Browser;
class Page;
class Locator;
class ScreenshotCapture;
class WebAutomation;
class ImageStorage;
class TooltipIntegration;

// Factory functions - REQUIRED for integration
std::unique_ptr<WebAutomation> CreateWebAutomation();
std::unique_ptr<Browser> CreateBrowser();
std::unique_ptr<Page> CreatePage();
std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
std::unique_ptr<Locator> CreateLocator();
std::unique_ptr<ImageStorage> CreateImageStorage();
std::unique_ptr<TooltipIntegration> CreateTooltipIntegration();

// Core NaviGrab interface
class NaviGrabCore {
public:
    NaviGrabCore();
    ~NaviGrabCore();
    
    // Singleton pattern
    static NaviGrabCore& GetInstance();
    static void Shutdown();
    
    // Core initialization
    bool Initialize();
    bool IsInitialized() const;

    // Browser management
    std::unique_ptr<Browser> CreateBrowser();
    bool LaunchBrowser();
    void CloseBrowser();

    // Page operations
    std::unique_ptr<Page> CreatePage();
    bool NavigateToPage(const std::string& url);
    
    // Element interaction
    std::unique_ptr<Locator> CreateLocator();
    bool ClickElement(const std::string& selector);
    bool TypeText(const std::string& selector, const std::string& text);
    bool HoverElement(const std::string& selector);
    
    // Screenshot and capture
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    bool CaptureScreenshot(const std::string& filename);
    bool CaptureElementScreenshot(const std::string& selector, const std::string& filename);
    
    // Web automation
    std::unique_ptr<WebAutomation> CreateWebAutomation();
    bool ExecuteJavaScript(const std::string& script);
    std::string GetPageContent();
    std::vector<std::string> GetPageLinks();
    
    // Utility functions
    bool IsPageLoaded();
    std::string GetCurrentUrl();
    std::string GetPageTitle();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Browser class
class Browser {
public:
    Browser();
    ~Browser();
    
    bool Launch();
    void Close();
    bool IsRunning() const;
    
    std::unique_ptr<Page> NewPage();
    
    // Navigation - NEW METHODS
    bool NavigateTo(const std::string& url);
    std::string GetCurrentUrl() const;
    std::string GetTitle() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Page class
class Page {
public:
    Page();
    ~Page();
    
    bool NavigateTo(const std::string& url);
    bool WaitForLoad();
    bool IsLoaded() const;
    
    std::string GetUrl() const;
    std::string GetTitle() const;
    std::string GetContent() const;
    
    // Element interaction
    bool Click(const std::string& selector);
    bool Type(const std::string& selector, const std::string& text);
    bool Hover(const std::string& selector);
    bool Focus(const std::string& selector);
    
    // Element interaction aliases - REQUIRED for consistency
    bool ClickElement(const std::string& selector);     // Alias for Click()
    bool TypeText(const std::string& selector, const std::string& text);  // Alias for Type()
    bool HoverElement(const std::string& selector);     // Alias for Hover()
    
    // Content extraction - NEW METHODS
    std::string GetElementText(const std::string& selector);
    std::string GetElementAttribute(const std::string& selector, const std::string& attribute);
    bool ExecuteScriptOnElement(const std::string& selector, const std::string& script);
    
    // Screenshots
    bool Screenshot(const std::string& filename);
    bool ElementScreenshot(const std::string& selector, const std::string& filename);
    
    // JavaScript execution
    bool ExecuteScript(const std::string& script);
    std::string EvaluateScript(const std::string& script);
    
    // Element discovery
    std::vector<std::string> GetLinks();
    std::vector<std::string> GetButtons();
    std::vector<std::string> GetFormElements();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Locator class for finding elements
class Locator {
public:
    Locator();
    ~Locator();
    
    // Find elements
    std::vector<std::string> FindByTag(const std::string& tag);
    std::vector<std::string> FindByClass(const std::string& className);
    std::vector<std::string> FindById(const std::string& id);
    std::vector<std::string> FindBySelector(const std::string& selector);
    
    // Element properties
    bool IsVisible(const std::string& selector);
    bool IsEnabled(const std::string& selector);
    bool IsClickable(const std::string& selector);
    
    // Element information
    std::string GetText(const std::string& selector);
    std::string GetAttribute(const std::string& selector, const std::string& attribute);
    std::pair<int, int> GetPosition(const std::string& selector);
    std::pair<int, int> GetSize(const std::string& selector);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Screenshot capture functionality
class ScreenshotCapture {
public:
    ScreenshotCapture();
    ~ScreenshotCapture();
    
    // Screenshot methods
    bool CaptureFullPage(const std::string& filename);
    bool CaptureViewport(const std::string& filename);
    bool CaptureElement(const std::string& selector, const std::string& filename);
    
    // Screenshot aliases - REQUIRED for consistency
    bool CapturePage(const std::string& filename);      // Alias for CaptureFullPage()
    
    // Memory-based capture - NEW METHODS for tooltips
    std::vector<uint8_t> CapturePageData();             // Return image data directly
    std::vector<uint8_t> CaptureElementData(const std::string& selector);
    bool CaptureToMemory(std::vector<uint8_t>& data);   // Capture to memory buffer
    
    // Thumbnail generation for tooltips
    std::vector<uint8_t> GenerateThumbnail(const std::vector<uint8_t>& image_data, 
                                          int max_width = 200, int max_height = 150);
    
    // Screenshot options
    void SetQuality(int quality); // 1-100
    void SetFormat(const std::string& format); // "png", "jpeg"
    void SetFullPage(bool fullPage);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Web automation functionality
class WebAutomation {
public:
    WebAutomation();
    ~WebAutomation();
    
    // Factory methods - REQUIRED for integration
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    
    // Form interaction
    bool FillForm(const std::string& formSelector, const std::vector<std::pair<std::string, std::string>>& fields);
    bool FillForm(const std::string& formSelector, const std::map<std::string, std::string>& fields); // Map overload
    bool SubmitForm(const std::string& formSelector);
    
    // Navigation
    bool GoBack();
    bool GoForward();
    bool Refresh();
    
    // Content extraction
    std::string ExtractText(const std::string& selector);
    std::vector<std::string> ExtractLinks(const std::string& containerSelector = "");
    std::vector<std::string> ExtractImages(const std::string& containerSelector = "");
    
    // JavaScript execution - FIXED RETURN TYPES
    std::string ExecuteScript(const std::string& script); // Changed from bool to string
    std::string EvaluateExpression(const std::string& expression); // Changed from bool to string
    bool ExecuteScriptOnElement(const std::string& selector, const std::string& script);
    
    // Element interaction - NEW METHODS
    bool ClickElement(const std::string& selector);
    bool TypeText(const std::string& selector, const std::string& text);
    bool HoverElement(const std::string& selector);
    std::string GetElementText(const std::string& selector);
    std::string GetElementAttribute(const std::string& selector, const std::string& attribute);
    
    // Element discovery
    std::vector<std::string> DiscoverInteractiveElements();
    std::vector<std::string> DiscoverFormElements();
    std::vector<std::string> DiscoverNavigationElements();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Image storage for tooltip thumbnails
class ImageStorage {
public:
    ImageStorage();
    ~ImageStorage();
    
    // Storage management
    bool Initialize(const std::string& storage_path);
    void Shutdown();
    
    // Image operations
    bool StoreImage(const std::string& key, const std::vector<uint8_t>& image_data);
    std::vector<uint8_t> GetImage(const std::string& key);
    bool DeleteImage(const std::string& key);
    bool ImageExists(const std::string& key);
    
    // Storage management
    std::vector<std::string> ListImages();
    size_t GetStorageSize();
    bool ClearStorage();
    
    // Image processing
    std::vector<uint8_t> CompressImage(const std::vector<uint8_t>& image_data, int quality);
    std::vector<uint8_t> ResizeImage(const std::vector<uint8_t>& image_data, int width, int height);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Tooltip integration for Chromium
class TooltipIntegration {
public:
    TooltipIntegration();
    ~TooltipIntegration();
    
    // Tooltip management
    bool ShowTooltip(const std::string& selector, const std::string& content);
    void HideTooltip();
    void UpdateTooltip(const std::string& content);
    
    // Element highlighting
    bool HighlightElement(const std::string& selector);
    void RemoveHighlight(const std::string& selector);
    
    // Content extraction
    std::string ExtractElementInfo(const std::string& selector);
    std::vector<uint8_t> CaptureElementScreenshot(const std::string& selector);
    
    // Settings
    void SetDarkMode(bool enabled);
    void SetTooltipStyle(const std::string& style);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace navigrab

