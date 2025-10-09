# NaviGrab API Requirements for Chromium Screenshot Tooltip Integration

## 📋 Overview

This document outlines the specific API requirements needed from the NaviGrab team to enable seamless integration with our Chromium screenshot tooltip functionality. The current NaviGrab implementation is missing several critical methods and has API inconsistencies that prevent successful integration.

## 🚨 Critical Issues

### 1. Missing Factory Functions
The current NaviGrab implementation lacks essential factory functions that are required for proper initialization:

```cpp
// REQUIRED: These functions must be implemented
namespace navigrab {
    std::unique_ptr<WebAutomation> CreateWebAutomation();
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    std::unique_ptr<Locator> CreateLocator();
}
```

### 2. Missing Methods in WebAutomation Class
The WebAutomation class is missing several methods that are essential for our integration:

```cpp
class WebAutomation {
public:
    // REQUIRED: These methods must be added
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    
    // REQUIRED: Fix return type - currently returns bool, needs string
    std::string ExecuteScript(const std::string& script);
    std::string EvaluateExpression(const std::string& expression);
    
    // REQUIRED: Add these missing methods
    bool ExecuteScriptOnElement(const std::string& selector, const std::string& script);
    std::string GetElementText(const std::string& selector);
    std::string GetElementAttribute(const std::string& selector, const std::string& attribute);
};
```

### 3. Missing Methods in Page Class
The Page class needs additional methods for element interaction:

```cpp
class Page {
public:
    // REQUIRED: Add these aliases for consistency
    bool ClickElement(const std::string& selector);     // Alias for Click()
    bool TypeText(const std::string& selector, const std::string& text);  // Alias for Type()
    bool HoverElement(const std::string& selector);     // Alias for Hover()
    
    // REQUIRED: Add these missing methods
    std::string GetElementText(const std::string& selector);
    std::string GetElementAttribute(const std::string& selector, const std::string& attribute);
    bool ExecuteScriptOnElement(const std::string& selector, const std::string& script);
};
```

### 4. Missing Methods in ScreenshotCapture Class
The ScreenshotCapture class needs additional methods:

```cpp
class ScreenshotCapture {
public:
    // REQUIRED: Add these aliases for consistency
    bool CapturePage(const std::string& filename);      // Alias for CaptureFullPage()
    bool CaptureElement(const std::string& selector, const std::string& filename);  // Alias for existing method
    
    // REQUIRED: Add these missing methods
    std::vector<uint8_t> CapturePageData();             // Return image data directly
    std::vector<uint8_t> CaptureElementData(const std::string& selector);
    bool CaptureToMemory(std::vector<uint8_t>& data);   // Capture to memory buffer
};
```

### 5. Type Mismatch Issues
Several methods have incorrect parameter or return types:

```cpp
// REQUIRED: Fix FillForm method signature
class WebAutomation {
public:
    // Current (INCORRECT):
    bool FillForm(const std::string& formSelector, const std::vector<std::pair<std::string, std::string>>& fields);
    
    // REQUIRED: Add overload for map parameter
    bool FillForm(const std::string& formSelector, const std::map<std::string, std::string>& fields);
};

// REQUIRED: Fix Browser navigation method
class Browser {
public:
    // Current (INCORRECT): NavigateTo doesn't exist
    // REQUIRED: Add this method
    bool NavigateTo(const std::string& url);
    std::unique_ptr<Page> NewPage();
};
```

## 🎯 Required Implementation

### Phase 1: Core API Fixes (Priority: HIGH)

#### 1.1 Factory Functions
```cpp
// File: navigrab_core.h
namespace navigrab {
    // Global factory functions
    std::unique_ptr<WebAutomation> CreateWebAutomation();
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    std::unique_ptr<Locator> CreateLocator();
    
    // Core initialization
    class NaviGrabCore {
    public:
        static NaviGrabCore& GetInstance();
        bool Initialize();
        void Shutdown();
    };
}
```

#### 1.2 WebAutomation Class Updates
```cpp
// File: navigrab_core.h
class WebAutomation {
public:
    // Constructor and destructor
    WebAutomation();
    ~WebAutomation();
    
    // Factory methods
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    
    // JavaScript execution (FIX RETURN TYPE)
    std::string ExecuteScript(const std::string& script);
    std::string EvaluateExpression(const std::string& expression);
    bool ExecuteScriptOnElement(const std::string& selector, const std::string& script);
    
    // Element interaction
    bool ClickElement(const std::string& selector);
    bool TypeText(const std::string& selector, const std::string& text);
    bool HoverElement(const std::string& selector);
    
    // Content extraction
    std::string GetElementText(const std::string& selector);
    std::string GetElementAttribute(const std::string& selector, const std::string& attribute);
    
    // Form interaction (ADD MAP OVERLOAD)
    bool FillForm(const std::string& formSelector, const std::vector<std::pair<std::string, std::string>>& fields);
    bool FillForm(const std::string& formSelector, const std::map<std::string, std::string>& fields);
    bool SubmitForm(const std::string& formSelector);
    
    // Navigation
    bool GoBack();
    bool GoForward();
    bool Refresh();
    
    // Content extraction
    std::vector<std::string> ExtractLinks(const std::string& containerSelector = "");
    std::vector<std::string> ExtractImages(const std::string& containerSelector = "");
    
    // Element discovery
    std::vector<std::string> DiscoverInteractiveElements();
    std::vector<std::string> DiscoverFormElements();
    std::vector<std::string> DiscoverNavigationElements();
};
```

#### 1.3 Page Class Updates
```cpp
// File: navigrab_core.h
class Page {
public:
    // Constructor and destructor
    Page();
    ~Page();
    
    // Navigation
    bool NavigateTo(const std::string& url);
    bool WaitForLoad();
    bool IsLoaded() const;
    
    // Page information
    std::string GetUrl() const;
    std::string GetTitle() const;
    std::string GetContent() const;
    
    // Element interaction (ADD ALIASES)
    bool Click(const std::string& selector);
    bool ClickElement(const std::string& selector);     // Alias for Click()
    
    bool Type(const std::string& selector, const std::string& text);
    bool TypeText(const std::string& selector, const std::string& text);  // Alias for Type()
    
    bool Hover(const std::string& selector);
    bool HoverElement(const std::string& selector);     // Alias for Hover()
    
    bool Focus(const std::string& selector);
    
    // Content extraction (ADD MISSING METHODS)
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
};
```

#### 1.4 ScreenshotCapture Class Updates
```cpp
// File: navigrab_core.h
class ScreenshotCapture {
public:
    // Constructor and destructor
    ScreenshotCapture();
    ~ScreenshotCapture();
    
    // Screenshot methods (ADD ALIASES)
    bool CaptureFullPage(const std::string& filename);
    bool CapturePage(const std::string& filename);      // Alias for CaptureFullPage()
    
    bool CaptureViewport(const std::string& filename);
    bool CaptureElement(const std::string& selector, const std::string& filename);
    
    // Memory-based capture (ADD NEW METHODS)
    std::vector<uint8_t> CapturePageData();
    std::vector<uint8_t> CaptureElementData(const std::string& selector);
    bool CaptureToMemory(std::vector<uint8_t>& data);
    
    // Screenshot options
    void SetQuality(int quality); // 1-100
    void SetFormat(const std::string& format); // "png", "jpeg"
    void SetFullPage(bool fullPage);
};
```

#### 1.5 Browser Class Updates
```cpp
// File: navigrab_core.h
class Browser {
public:
    // Constructor and destructor
    Browser();
    ~Browser();
    
    // Browser management
    bool Launch();
    void Close();
    bool IsRunning() const;
    
    // Page management
    std::unique_ptr<Page> NewPage();
    
    // Navigation (ADD MISSING METHODS)
    bool NavigateTo(const std::string& url);
    std::string GetCurrentUrl() const;
    std::string GetTitle() const;
};
```

### Phase 2: Advanced Features (Priority: MEDIUM)

#### 2.1 Local Image Storage
```cpp
// File: image_storage.h
namespace navigrab {
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
    };
}
```

#### 2.2 Tooltip Integration
```cpp
// File: tooltip_integration.h
namespace navigrab {
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
        void SetTooltipStyle(const TooltipStyle& style);
    };
}
```

## 🔧 Implementation Requirements

### 1. Code Quality Standards
- **C++17 compliance**: Use modern C++ features
- **Memory safety**: Use smart pointers and RAII
- **Error handling**: Comprehensive error reporting
- **Thread safety**: Support for multi-threaded usage
- **Cross-platform**: Windows, macOS, Linux support

### 2. Performance Requirements
- **Screenshot capture**: < 100ms for element screenshots
- **Memory usage**: < 50MB for typical usage
- **Storage operations**: < 10ms for image save/load
- **JavaScript execution**: < 50ms for simple scripts

### 3. Integration Requirements
- **Chromium compatibility**: Work with Chromium's WebContents
- **JavaScript bridge**: Execute JavaScript in page context
- **Event handling**: Respond to user interactions
- **Resource management**: Proper cleanup and memory management

## 📋 Testing Requirements

### 1. Unit Tests
- Test all public API methods
- Test error conditions and edge cases
- Test memory management and cleanup
- Test cross-platform compatibility

### 2. Integration Tests
- Test with real Chromium WebContents
- Test JavaScript execution in page context
- Test screenshot capture functionality
- Test tooltip display and interaction

### 3. Performance Tests
- Benchmark screenshot capture performance
- Test memory usage under load
- Test storage operations performance
- Test concurrent usage scenarios

## 🚀 Delivery Timeline

### Week 1: Core API Fixes
- [ ] Implement missing factory functions
- [ ] Fix WebAutomation class methods
- [ ] Fix Page class methods
- [ ] Fix ScreenshotCapture class methods
- [ ] Fix Browser class methods

### Week 2: Type Fixes and Testing
- [ ] Fix return type issues
- [ ] Add missing method overloads
- [ ] Implement comprehensive unit tests
- [ ] Test integration with Chromium

### Week 3: Advanced Features
- [ ] Implement ImageStorage class
- [ ] Implement TooltipIntegration class
- [ ] Add performance optimizations
- [ ] Complete integration testing

### Week 4: Documentation and Release
- [ ] Complete API documentation
- [ ] Create usage examples
- [ ] Prepare release packages
- [ ] Final testing and validation

## 📞 Contact Information

For questions or clarifications about these requirements, please contact:
- **Project Lead**: [Your Name]
- **Email**: [your.email@example.com]
- **Repository**: [GitHub repository URL]
- **Issue Tracker**: [GitHub issues URL]

## 📄 Conclusion

These API requirements are essential for successful integration of NaviGrab with our Chromium screenshot tooltip functionality. The missing methods and type inconsistencies are currently blocking our development progress. We appreciate your prompt attention to these requirements and look forward to a successful integration.

Thank you for your cooperation in making this integration possible.


