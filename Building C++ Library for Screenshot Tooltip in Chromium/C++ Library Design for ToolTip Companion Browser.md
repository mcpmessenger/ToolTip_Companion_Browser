# C++ Library Design for ToolTip Companion Browser

## 1. Introduction
This document outlines the design for a C++ library to enhance the `ToolTip_Companion_Browser` Chromium fork. The primary goal is to implement an intelligent tooltip system that proactively or on-demand captures screenshots of interactive elements (buttons and links) on a webpage, stores them client-side as Base64 encoded images, and displays these thumbnails as tooltips when a user hovers over the respective elements.

## 2. Overall Architecture
The new C++ library will integrate directly into the existing `tooltip_components/chrome/browser/tooltip` directory of the Chromium fork. It will leverage and extend the `ScreenshotCapture` and `ElementDetector` classes already present in the codebase. The core logic will reside within the browser process, interacting with the renderer process via `WebContentsObserver` and JavaScript execution for DOM manipulation and screenshot requests. A new service, `TooltipManagerService`, will orchestrate the entire process, managing element detection, screenshot capture, local storage, and tooltip display.

### Component Diagram
```mermaid
graph TD
    A[Browser Process] --> B(TooltipManagerService)
    B --> C(ElementDetector)
    B --> D(ScreenshotCapture)
    B --> E(LocalStorageManager)
    B --> F(TooltipUIController)
    C --> G{WebContents (Renderer Process)}
    D --> G
    F --> G
    G --> H[Web Page DOM]
    H --> I[Interactive Elements]
    E --> J[Client-side Storage]
```

*   **`TooltipManagerService`**: The central orchestrator. It will be responsible for initializing and coordinating `ElementDetector`, `ScreenshotCapture`, `LocalStorageManager`, and `TooltipUIController`. It will expose APIs for triggering element crawling and managing tooltip display.
*   **`ElementDetector`**: Extended to identify all interactive buttons and links on a page, providing their bounding box coordinates and associated URLs/actions.
*   **`ScreenshotCapture`**: Enhanced to capture screenshots of specific regions (bounding boxes) of the webpage and encode them into Base64 format.
*   **`LocalStorageManager`**: A new component responsible for storing and retrieving Base64 encoded images client-side, indexed by element identifiers (e.g., hash of element properties or URL).
*   **`TooltipUIController`**: A new component responsible for rendering the thumbnail tooltips in the browser UI when a user hovers over an interactive element.
*   **WebContents (Renderer Process)**: The interface through which the browser process communicates with the web page's DOM, executing JavaScript and receiving information about elements.

## 3. API Specification

### `TooltipManagerService` (New Service)
This service will be a singleton accessible within the browser process, managing the lifecycle of tooltips.

```cpp
namespace tooltip {

class TooltipManagerService {
 public:
  // Initializes the service.
  void Initialize();

  // Triggers the crawling of interactive elements and captures screenshots.
  // If `proactive` is true, it runs automatically. If false, it runs on-demand.
  void StartCrawlingAndCapture(content::WebContents* web_contents, bool proactive);

  // Displays a tooltip for the element at the given screen coordinates.
  void ShowTooltip(content::WebContents* web_contents, const gfx::Point& screen_point);

  // Hides any active tooltip.
  void HideTooltip();

 private:
  // Callbacks for ElementDetector and ScreenshotCapture.
  void OnElementsDetected(const std::vector<gfx::Rect>& elements, const std::vector<std::string>& identifiers);
  void OnScreenshotCaptured(const std::string& element_identifier, const gfx::Image& image);

  std::unique_ptr<ElementDetector> element_detector_;
  std::unique_ptr<ScreenshotCapture> screenshot_capture_;
  std::unique_ptr<LocalStorageManager> local_storage_manager_;
  std::unique_ptr<TooltipUIController> tooltip_ui_controller_;

  // Map to store element identifiers to their bounding boxes and associated URLs/actions.
  std::map<std::string, ElementInfo> element_info_map_;
};

struct ElementInfo {
  gfx::Rect bounding_box;
  std::string url_or_action;
  // Other relevant metadata
};

}  // namespace tooltip
```

### `ElementDetector` (Extension)

```cpp
namespace tooltip {

class ElementDetector : public content::WebContentsObserver {
 public:
  // ... existing methods ...

  // Starts detecting interactive elements on the given `web_contents`.
  // The `callback` will be invoked with a list of bounding boxes and unique identifiers.
  void StartDetection(content::WebContents* web_contents,
                      base::OnceCallback<void(const std::vector<gfx::Rect>&, const std::vector<std::string>&)> callback);

 private:
  // ... existing methods ...
  void OnInteractiveElementsDetected(const std::string& json_elements);

  base::OnceCallback<void(const std::vector<gfx::Rect>&, const std::vector<std::string>&)> detection_callback_;
};

}  // namespace tooltip
```

### `ScreenshotCapture` (Extension)

```cpp
namespace tooltip {

class ScreenshotCapture : public content::WebContentsObserver {
 public:
  // ... existing methods ...

  // Captures a screenshot of the specified `rect` within the `web_contents`.
  // The `callback` will be invoked with the captured image (or an empty image
  // on failure) and the element identifier.
  void Capture(content::WebContents* web_contents,
               const gfx::Rect& rect,
               const std::string& element_identifier,
               base::OnceCallback<void(const std::string&, const gfx::Image&)> callback);

 private:
  // ... existing methods ...
  void OnScreenshotCaptured(const std::string& element_identifier, const SkBitmap& bitmap);

  base::OnceCallback<void(const std::string&, const gfx::Image&)> capture_callback_;
};

}  // namespace tooltip
```

### `LocalStorageManager` (New Component)

```cpp
namespace tooltip {

class LocalStorageManager {
 public:
  // Stores a Base64 encoded image associated with an element identifier.
  void StoreImage(const std::string& element_identifier, const std::string& base64_image);

  // Retrieves a Base64 encoded image for a given element identifier.
  // Returns an empty string if not found.
  std::string RetrieveImage(const std::string& element_identifier);

  // Clears all stored images.
  void ClearStorage();
};

}  // namespace tooltip
```

### `TooltipUIController` (New Component)

```cpp
namespace tooltip {

class TooltipUIController {
 public:
  // Displays a tooltip with the given Base64 image at the specified screen coordinates.
  void DisplayTooltip(const std::string& base64_image, const gfx::Point& screen_point);

  // Hides the currently displayed tooltip.
  void HideTooltip();
};

}  // namespace tooltip
```

## 4. Detailed Design

### 4.1. Element Detection and Identification
*   **Extension of `ElementDetector`**: The `StartDetection` method will be modified to return not only the bounding boxes but also unique identifiers for each interactive element. These identifiers could be a hash of the element's tag name, attributes (id, class, href, etc.), and its position in the DOM to ensure uniqueness and persistence across minor page changes.
*   **JavaScript Injection**: `ElementDetector` will inject JavaScript into the renderer process to traverse the DOM, identify elements matching `kInteractiveTags` and `kInteractiveAttributes`, calculate their `getBoundingClientRect()`, and generate unique identifiers. The results will be serialized as JSON and returned to the browser process.

### 4.2. Screenshot Capture and Base64 Encoding
*   **Extension of `ScreenshotCapture`**: The `Capture` method will be updated to accept an `element_identifier`. After capturing the `gfx::Image`, it will be converted to a `SkBitmap`, then encoded into a PNG or JPEG format, and finally Base64 encoded. This Base64 string will be passed back via the callback along with the `element_identifier`.
*   **Image Format**: PNG will be preferred for quality, but JPEG could be an option for smaller file sizes if performance becomes an issue. The choice will be configurable.

### 4.3. Local Storage Management
*   **`LocalStorageManager` Implementation**: This component will use Chromium's internal storage mechanisms (e.g., `leveldb` or `PrefService` for simpler key-value storage) to persist the `element_identifier` to Base64 image mappings. This ensures that screenshots are stored client-side and persist across browser sessions.
*   **Storage Key**: The `element_identifier` will serve as the key for storing and retrieving images.
*   **Cache Management**: A simple cache invalidation strategy will be implemented (e.g., clear cache on browser restart, or based on a time-to-live for screenshots) to prevent stale images and manage storage space.

### 4.4. Tooltip Rendering and Hover Interaction
*   **`TooltipUIController` Implementation**: This component will be responsible for creating and managing the visual display of the tooltips. It will likely involve creating a custom `views::Widget` or similar UI element in Chromium's UI framework.
*   **Hover Detection**: The `TooltipManagerService` will register a mouse event listener (e.g., `aura::EnvObserver` or similar platform-specific mechanism) to detect hover events. When a mouse hovers over an area, `TooltipManagerService` will query `ElementDetector` to see if it corresponds to a known interactive element.
*   **Display Logic**: If a match is found, `TooltipManagerService` will retrieve the corresponding Base64 image from `LocalStorageManager` and pass it to `TooltipUIController` to display the thumbnail at the appropriate screen coordinates.
*   **Performance**: The tooltip display should be lightweight and performant to avoid UI lag. The Base64 image decoding and rendering should be optimized.

### 4.5. Spider Button Integration
*   **UI Integration**: The 


Spider button in the top right of the browser will trigger the `StartCrawlingAndCapture` method of the `TooltipManagerService` with `proactive = false`. This will initiate the crawling and screenshot capture process on demand.

## 5. Build System Integration (GN)

To integrate the new C++ library, modifications will be required in the `BUILD.gn` files within the `tooltip_components` directory and potentially in `chrome/browser`.

*   **`tooltip_components/BUILD.gn`**: Add a new `source_set` target for the `tooltip_manager_service` and `local_storage_manager` components. Update existing `source_set` targets for `element_detector` and `screenshot_capture` to include any new files or dependencies.
*   **Dependencies**: Ensure that the new targets correctly declare their dependencies on other Chromium modules (e.g., `content`, `ui/gfx`, `base`, `skia`).
*   **Visibility**: Configure visibility rules to allow other Chromium components to depend on our new tooltip library.

## 6. Future Considerations

*   **Accessibility**: Ensure the tooltip system is accessible to users with disabilities.
*   **Internationalization**: Support for multiple languages for any UI elements introduced.
*   **Configuration**: Provide options for users to enable/disable the tooltip system, clear cached images, or adjust capture settings.
*   **Performance Optimization**: Continuously monitor and optimize the performance of screenshot capture, image encoding, storage, and tooltip rendering.
*   **Error Handling**: Robust error handling for all operations, especially during JavaScript execution and image processing.

## 7. Conclusion
This design provides a comprehensive plan for building the C++ library for the `ToolTip_Companion_Browser`. By extending existing components and introducing new services, we can create a powerful and intuitive tooltip system that enhances user experience by providing visual context for interactive elements. The modular design allows for future enhancements and maintainability within the Chromium ecosystem.

