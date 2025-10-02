# Developer Instructions: Building NaviGrab C++ Files for ToolTip Companion Browser

This document provides instructions for building and integrating the NaviGrab C++ files, specifically addressing the requirements for the ToolTip Companion Browser project. The ultimate goal is to enable a system that displays a thumbnail on mouse-over for any page element and to incorporate a "fresh crawl" button into the GUI.

## 1. Understanding the NaviGrab Integration Context

The ToolTip Companion Browser project relies on NaviGrab for its web automation and screenshot capabilities. However, as detailed in `NAVIGRAB_REQUIREMENTS.md` [1], the existing NaviGrab implementation has several critical missing functions, methods, and type inconsistencies that must be addressed for seamless integration. These instructions assume you are working within the `ToolTip_Companion_Browser` Chromium fork and need to either patch an existing NaviGrab library or build a custom version that includes the necessary API extensions.

## 2. Prerequisites

Before proceeding, ensure you have the following:

*   **Chromium Development Environment**: A fully set up Chromium development environment, as described in the `ToolTip_Companion_Browser` project's `README.md` [2] and `BUILD_PLAN.md` [3].
*   **C++17 Compatible Compiler**: (e.g., Clang 12+ or MSVC 2019+).
*   **CMake**: Version 3.16 or later.
*   **Git**: For cloning repositories.
*   **NaviGrab Source Code**: Access to the NaviGrab C++ source code. If a specific version or fork is required, ensure it is available.

## 3. Building NaviGrab with Required Extensions

Since the `NAVIGRAB_REQUIREMENTS.md` [1] indicates missing functionalities, you will need to either modify the NaviGrab source directly or create a wrapper/extension library that implements these missing pieces. The following steps outline a general approach for a custom build or patching process.

### 3.1. Obtain NaviGrab Source Code

If NaviGrab is an external dependency, clone its repository. For this example, we assume a hypothetical NaviGrab repository. If NaviGrab is part of the Chromium source tree, locate its directory.

```bash
# Example: If NaviGrab is a separate repository
git clone https://github.com/navigrab/navigrab.git
cd navigrab
# Checkout the appropriate branch or tag if necessary
# git checkout <branch_or_tag>
```

### 3.2. Implement Missing NaviGrab APIs

Based on `NAVIGRAB_REQUIREMENTS.md` [1], you need to implement the following:

#### 3.2.1. Missing Factory Functions

Ensure the `navigrab` namespace provides factory functions for creating core objects. These typically reside in a central factory or utility file within the NaviGrab library.

```cpp
// Example: navigrab_factory.h (or similar)
namespace navigrab {
    std::unique_ptr<WebAutomation> CreateWebAutomation();
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    std::unique_ptr<Locator> CreateLocator();
}
```

#### 3.2.2. Extend `WebAutomation` Class

Modify the `WebAutomation` class definition (e.g., `web_automation.h` and `web_automation.cc` within NaviGrab) to include the required methods and correct return types.

```cpp
// Example: web_automation.h
class WebAutomation {
public:
    // ... existing methods ...

    // REQUIRED: New methods for creating browser components
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

    // REQUIRED: Add overload for map parameter for FillForm
    bool FillForm(const std::string& formSelector, const std::map<std::string, std::string>& fields);

    // ... other methods ...
};
```

#### 3.2.3. Extend `Page` Class

Modify the `Page` class definition (e.g., `page.h` and `page.cc` within NaviGrab) to include the required aliases and new methods.

```cpp
// Example: page.h
class Page {
public:
    // ... existing methods ...

    // REQUIRED: Add these aliases for consistency
    bool ClickElement(const std::string& selector);     // Alias for Click()
    bool TypeText(const std::string& selector, const std::string& text);  // Alias for Type()
    bool HoverElement(const std::string& selector);     // Alias for Hover()
    
    // REQUIRED: Add these missing methods
    std::string GetElementText(const std::string& selector);
    std::string GetElementAttribute(const std::string& selector, const std::string& attribute);
    bool ExecuteScriptOnElement(const std::string& selector, const std::string& script);

    // ... other methods ...
};
```

#### 3.2.4. Extend `ScreenshotCapture` Class

Modify the `ScreenshotCapture` class definition (e.g., `screenshot_capture.h` and `screenshot_capture.cc` within NaviGrab) to include the required aliases and new methods for direct image data return.

```cpp
// Example: screenshot_capture.h
class ScreenshotCapture {
public:
    // ... existing methods ...

    // REQUIRED: Add these aliases for consistency
    bool CapturePage(const std::string& filename);      // Alias for CaptureFullPage()
    bool CaptureElement(const std::string& selector, const std::string& filename);  // Alias for existing method
    
    // REQUIRED: Add these missing methods
    std::vector<uint8_t> CapturePageData();             // Return image data directly
    std::vector<uint8_t> CaptureElementData(const std::string& selector);
    bool CaptureToMemory(std::vector<uint8_t>& data);   // Capture to memory buffer

    // ... other methods ...
};
```

#### 3.2.5. Extend `Browser` Class

Modify the `Browser` class definition (e.g., `browser.h` and `browser.cc` within NaviGrab) to add the `NavigateTo` method.

```cpp
// Example: browser.h
class Browser {
public:
    // ... existing methods ...

    // REQUIRED: Add this method
    bool NavigateTo(const std::string& url);
    std::unique_ptr<Page> NewPage();

    // ... other methods ...
};
```

### 3.3. Build NaviGrab

After implementing the necessary changes, build the NaviGrab library. This typically involves CMake.

```bash
# Assuming you are in the NaviGrab source directory
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
# This will generate the NaviGrab library files (e.g., .lib, .a, .so, .dylib)
```

## 4. Integrating NaviGrab into ToolTip Companion Browser

Once the NaviGrab library is built with the required extensions, you need to integrate it into the Chromium fork.

### 4.1. Update `BUILD.gn`

Modify the `BUILD.gn` files within the `ToolTip_Companion_Browser` project (e.g., `src/chrome/browser/tooltip/BUILD.gn`) to link against your custom NaviGrab library. You will need to specify the include paths and library paths.

```gn
# Example BUILD.gn modification

source_set("tooltip") {
  # ... existing sources ...

  # Add NaviGrab integration source files
  sources += [
    "navigrab_integration.cc",
    "navigrab_integration.h",
  ]

  # Add NaviGrab include directories
  include_dirs = [
    "//path/to/navigrab/include", # Adjust this path to your NaviGrab headers
  ]

  # Link against the NaviGrab library
  libs = [
    "navigrab", # Name of your NaviGrab library
  ]
  lib_dirs = [
    "//path/to/navigrab/build/Release", # Adjust this path to your NaviGrab library build output
  ]

  # ... other dependencies ...
}
```

### 4.2. Implement `NaviGrabIntegration` in `tooltip_service.cc`

The `ToolTip_Companion_Browser` project already has `navigrab_integration.h` and `tooltip_service.h` [4, 5]. The `TooltipService` class has a `std::unique_ptr<class NaviGrabIntegration> navigrab_integration_` member. You will need to implement the logic within `tooltip_service.cc` to initialize and utilize the `NaviGrabIntegration` class, which in turn will use your extended NaviGrab library.

Specifically, the `TooltipService::InitializeComponents()` method should instantiate `NaviGrabIntegration` and call its `Initialize()` method. The `ExecuteAutomationAction` and related methods in `TooltipService` will then delegate to the `NaviGrabIntegration` instance.

### 4.3. Implement Mouse-Over Thumbnail Display

This functionality will primarily involve the `ElementDetector`, `ScreenshotCapture`, and `TooltipView` components within the `ToolTip_Companion_Browser` project.

1.  **Element Detection**: The `ElementDetector` (e.g., `element_detector.cc/.h`) will need to continuously monitor the mouse position and identify the element under the cursor. This might involve JavaScript injection into the `WebContents` to get element information and bounds.
2.  **Screenshot Capture**: When an element is hovered, the `ScreenshotCapture` component (e.g., `screenshot_capture.cc/.h`) will use the extended NaviGrab `ScreenshotCapture` methods (e.g., `CaptureElementData()`) to capture a thumbnail of the hovered element. This image data should be returned as `std::vector<uint8_t>`.
3.  **Tooltip Display**: The `TooltipService` will receive the captured thumbnail data and the `ElementInfo`. It will then call `ShowTooltipForElement()` to display a `TooltipView` (e.g., `tooltip_view.cc/.h`) containing the thumbnail. The `TooltipView` will be responsible for rendering the image data in a small, non-intrusive overlay near the mouse cursor.

This process will need to be optimized for performance to ensure a smooth user experience without noticeable lag during mouse movements.

## 5. GUI Modifications: "Fresh Crawl" Button

To add a "fresh crawl" button, you will need to modify the Chromium UI. This typically involves changes in the `chrome/browser/ui` and `chrome/browser/ui/views` directories of the Chromium source.

### 5.1. Button Placement

Decide where the "fresh crawl" button should be placed. Common locations include:

*   **Toolbar**: Next to the address bar or other extension buttons.
*   **Context Menu**: As an option when right-clicking on a page.
*   **Developer Tools Panel**: As a dedicated tool for developers.

For simplicity, let's assume placement in the browser toolbar.

### 5.2. UI Implementation

1.  **Icon**: Create a spider icon (e.g., `spider_icon.png` or `spider_icon.svg`) and add it to the Chromium resource files.
2.  **Button Creation**: In the relevant UI code (e.g., `toolbar_view.cc` or a custom `BrowserAction` implementation), create a new button element. This button should use the spider icon.
3.  **Event Handling**: Attach an event handler to the button's click event. When clicked, this event handler should trigger the "fresh crawl" logic within the `TooltipService` or `NaviGrabIntegration`.

### 5.3. "Fresh Crawl" Logic

The "fresh crawl" action should instruct the `NaviGrabIntegration` to re-evaluate the current page's elements and potentially re-cache their information or re-initialize any element detection mechanisms. This ensures that if the page content has changed dynamically, the tooltip system has the most up-to-date information.

```cpp
// Example: In TooltipService or NaviGrabIntegration
void PerformFreshCrawl() {
    // Clear existing element cache (if any)
    element_detector_->ClearCache();
    // Re-scan the current page for elements
    element_detector_->DetectElements(web_contents_);
    // Potentially re-capture base screenshots or update internal state
    // ...
}
```

## 6. Building the Chromium Fork

After making all the necessary code changes and integrating the NaviGrab library, build the entire Chromium fork.

```bash
cd src/src

# Set up build environment (if not already done)
# For Windows:
# $env:PATH = "C:\chromium\depot_tools;$env:PATH"
# For Linux/macOS:
# export PATH="/path/to/depot_tools:$PATH"

# Build the Chrome browser with your modifications
autoninja -C out/Default chrome # Or your specific build target

# Run Chrome with tooltip functionality
# For Windows:
# .\out\Default\chrome.exe
# For Linux/macOS:
# ./out/Default/chrome
```

## 7. Testing

Thoroughly test the new functionalities:

*   **NaviGrab Integration**: Verify that all previously missing NaviGrab functions and methods are correctly called and return expected results.
*   **Mouse-Over Thumbnails**: Navigate to various web pages and ensure that a thumbnail appears reliably and quickly when hovering over different elements.
*   **"Fresh Crawl" Button**: Test the button's functionality. Click it after dynamic content changes on a page and verify that the tooltip system updates accordingly.

## References

[1] [ToolTip_Companion_Browser/NAVIGRAB_REQUIREMENTS.md at screenshot-tooltip-library · mcpmessenger/ToolTip_Companion_Browser · GitHub](https://github.com/mcpmessenger/ToolTip_Companion_Browser/blob/screenshot-tooltip-library/NAVIGRAB_REQUIREMENTS.md)
[2] [GitHub - mcpmessenger/ToolTip_Companion_Browser at screenshot-tooltip-library](https://github.com/mcpmessenger/ToolTip_Companion_Browser/tree/screenshot-tooltip-library)
[3] [ToolTip_Companion_Browser/BUILD_PLAN.md at screenshot-tooltip-library · mcpmessenger/ToolTip_Companion_Browser · GitHub](https://github.com/mcpmessenger/ToolTip_Companion_Browser/blob/screenshot-tooltip-library/BUILD_PLAN.md)
[4] [ToolTip_Companion_Browser/src/chrome/browser/tooltip/navigrab_integration.h at screenshot-tooltip-library · mcpmessenger/ToolTip_Companion_Browser · GitHub](https://github.com/mcpmessenger/ToolTip_Companion_Browser/blob/screenshot-tooltip-library/src/chrome/browser/tooltip/navigrab_integration.h)
[5] [ToolTip_Companion_Browser/src/chrome/browser/tooltip/tooltip_service.h at screenshot-tooltip-library · mcpmessenger/ToolTip_Companion_Browser · GitHub](https://github.com/mcpmessenger/ToolTip_Companion_Browser/blob/screenshot-tooltip-library/src/chrome/browser/tooltip/tooltip_service.h)

