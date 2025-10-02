# ToolTip Companion Browser: C++ Library Integration Guide

## 1. Introduction
This document provides detailed instructions for integrating the newly developed C++ tooltip library into the `ToolTip_Companion_Browser` Chromium fork. It covers modifications to the Chromium build system (GN/Ninja), the structure of the new C++ components, and how to enable and test the intelligent tooltip functionality.

## 2. C++ Library Overview
The C++ library consists of several key components designed to work within the Chromium browser process:

*   **`TooltipManagerService`**: The central orchestrator, managing the lifecycle of tooltips, coordinating element detection, screenshot capture, local storage, and UI display.
*   **`ElementDetector`**: Responsible for identifying interactive elements (buttons and links) on a webpage using JavaScript injection and providing their bounding box coordinates and unique identifiers.
*   **`ScreenshotCapture`**: Handles capturing screenshots of specific regions of the webpage and encoding them into Base64 format.
*   **`LocalStorageManager`**: Manages client-side storage and retrieval of Base64 encoded images, indexed by element identifiers.
*   **`TooltipUIController`**: Responsible for rendering and hiding the visual tooltip UI in the browser.

These components are designed to be integrated directly into the `tooltip_components/chrome/browser/tooltip` directory of the Chromium source tree.

## 3. Integrating into the Chromium Build System (GN/Ninja)
Chromium uses GN (Generate Ninja) to generate Ninja build files. To incorporate the new C++ library, you will need to modify the `BUILD.gn` files. The following steps outline the necessary changes.

### 3.1. Add New Files to `BUILD.gn`
Navigate to the `tooltip_components/chrome/browser/tooltip` directory within your Chromium source tree. You will need to modify the `BUILD.gn` file in this directory to include the new `.cc` and `.h` files.

**Example `BUILD.gn` modifications:**

```gn
source_set("tooltip") {
  sources = [
    "element_detector.cc",
    "element_detector.h",
    "screenshot_capture.cc",
    "screenshot_capture.h",
    "tooltip_browser_integration.cc",
    "tooltip_browser_integration.h",
    "tooltip_service.cc",
    "tooltip_service.h",
    "local_storage_manager.cc", # New file
    "local_storage_manager.h",  # New file
    "tooltip_manager_service.cc", # New file
    "tooltip_manager_service.h",  # New file
    "tooltip_ui_controller.cc", # New file
    "tooltip_ui_controller.h",  # New file
  ]

  deps = [
    "//base",
    "//content/public/browser",
    "//ui/gfx",
    "//ui/gfx/geometry",
    "//ui/gfx/image",
    "//ui/snapshot",
    "//skia",
    "//third_party/abseil-cpp/absl/types:optional",
    "//base:i18n",
    "//base:json",
    "//base:strings",
    "//base:values",
    "//ui/gfx/codec",
  ]
}
```

**Explanation of changes:**
*   **`sources`**: Add `local_storage_manager.cc`, `local_storage_manager.h`, `tooltip_manager_service.cc`, `tooltip_manager_service.h`, `tooltip_ui_controller.cc`, and `tooltip_ui_controller.h` to the `sources` list of the `source_set("tooltip")` target.
*   **`deps`**: Ensure that all necessary Chromium dependencies are listed. The new components rely on `base/json`, `base/values`, `ui/gfx/codec`, and `third_party/abseil-cpp/absl/types:optional` for JSON parsing, image encoding, and optional types. Verify these are present or add them if missing.

### 3.2. Regenerate Ninja Build Files
After modifying the `BUILD.gn` file, you need to regenerate the Ninja build files. Navigate to your Chromium source directory (e.g., `src/`) and run the following command:

```bash
gn gen out/Default
```
(Replace `out/Default` with your actual build directory if it's different.)

This command will re-evaluate the GN files and update the Ninja build configuration to include your new C++ files.

## 4. Enabling and Using the Tooltip Service

To enable and use the `TooltipManagerService`, you will need to integrate it into the appropriate browser context, typically within a `BrowserContext` or `WebContents` observer.

### 4.1. Instantiating and Initializing `TooltipManagerService`

Find a suitable location within the browser process (e.g., a `BrowserContext` keyed service or a `WebContentsObserver` that lives for the duration of a tab) to instantiate and initialize the `TooltipManagerService`. A common pattern is to use a `KeyedService` for services tied to a `BrowserContext`.

**Example (Conceptual Integration):**

```cpp
// In a BrowserContext-keyed service or similar lifecycle-managed class:

#include "chrome/browser/tooltip/tooltip_manager_service.h"

// ...

class MyBrowserFeatureService {
 public:
  MyBrowserFeatureService() {
    tooltip_manager_service_ = std::make_unique<tooltip::TooltipManagerService>();
    tooltip_manager_service_->Initialize();
  }

  void OnWebContentsCreated(content::WebContents* web_contents) {
    // Optionally, start crawling proactively for new tabs/web_contents
    tooltip_manager_service_->StartCrawlingAndCapture(web_contents, true);
  }

  void OnSpiderButtonClicked(content::WebContents* active_web_contents) {
    // Trigger on-demand crawling when the spider button is clicked
    tooltip_manager_service_->StartCrawlingAndCapture(active_web_contents, false);
  }

  void OnMouseEvent(content::WebContents* web_contents, const gfx::Point& screen_point) {
    // Forward mouse events to the tooltip manager for hover detection
    tooltip_manager_service_->ShowTooltip(web_contents, screen_point);
  }

  void OnMouseExit(content::WebContents* web_contents) {
    // Hide tooltip when mouse leaves an element or web_contents
    tooltip_manager_service_->HideTooltip();
  }

 private:
  std::unique_ptr<tooltip::TooltipManagerService> tooltip_manager_service_;
};
```

### 4.2. Integrating the Spider Button

To connect the 


spider button icon (as mentioned in the user prompt) to the `StartCrawlingAndCapture` method, you will need to:

1.  **Identify the UI element**: Locate the code responsible for rendering the spider button in the Chromium UI. This will likely be in a `views` or `toolbar` related directory.
2.  **Add an event handler**: Attach an event handler (e.g., a `views::ButtonListener` or similar) to the spider button.
3.  **Call `StartCrawlingAndCapture`**: In the event handler, obtain a pointer to the active `WebContents` and call `tooltip_manager_service_->StartCrawlingAndCapture(active_web_contents, false);` to trigger on-demand crawling and screenshot capture.

## 5. Testing and Debugging

*   **Logging**: Utilize Chromium's `VLOG` and `LOG` macros for debugging. Increase the verbosity level (e.g., `--v=1` command-line argument) to see detailed `VLOG` messages.
*   **Unit Tests**: Write unit tests for each new class (`LocalStorageManager`, `TooltipManagerService`, `TooltipUIController`) and for the extended functionalities of `ElementDetector` and `ScreenshotCapture`.
*   **Browser Tests**: Implement browser tests to verify the end-to-end functionality of the tooltip system, including element detection, screenshot capture, storage, and UI display.

## 6. Future Enhancements

*   **Persistent Storage**: Replace the in-memory `image_cache_` in `LocalStorageManager` with a persistent storage solution like `leveldb` or `PrefService` to ensure images are retained across browser restarts.
*   **Optimized JavaScript**: Refine the JavaScript injection in `ElementDetector` to handle dynamic content, iframes, and shadow DOM more robustly and efficiently.
*   **Advanced UI**: Develop a more sophisticated `TooltipUIController` that can render rich tooltips with additional information (e.g., element type, associated URL) alongside the thumbnail.
*   **Performance Monitoring**: Implement metrics to track the performance of screenshot capture and tooltip display, identifying and addressing any bottlenecks.

## 7. Developer Credit

This C++ library and integration guide were developed by Manus AI, with contributions from automationalien.com.

