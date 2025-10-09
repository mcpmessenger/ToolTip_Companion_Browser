#ifndef NAVIGRAB_TOOLTIP_H
#define NAVIGRAB_TOOLTIP_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace NaviGrabTooltip {

// Forward declarations
class WebAutomation;
class Browser;
class Page;
class ScreenshotCapture;
class ImageStorage;
class TooltipIntegration;

// Core data structures
struct ElementInfo {
    std::string selector;
    std::string tag_name;
    std::string text_content;
    std::map<std::string, std::string> attributes;
    int x, y, width, height;
    
    ElementInfo() : x(0), y(0), width(0), height(0) {}
};

struct ScreenshotResult {
    bool success;
    std::string error_message;
    std::vector<uint8_t> image_data;
    std::string storage_key;
    int64_t capture_time_ms;
    
    ScreenshotResult() : success(false), capture_time_ms(0) {}
};

struct TooltipInfo {
    std::string title;
    std::string description;
    std::vector<uint8_t> screenshot;
    int x, y;
    bool dark_mode;
    
    TooltipInfo() : x(0), y(0), dark_mode(false) {}
};

// Configuration
struct Config {
    std::string storage_path;
    size_t max_cache_size_mb;
    bool enable_dark_mode;
    bool enable_animations;
    std::string image_format;
    int compression_quality;
    
    Config() : max_cache_size_mb(100), enable_dark_mode(false), 
               enable_animations(true), image_format("png"), compression_quality(85) {}
};

// Main library class
class NaviGrabTooltipLib {
public:
    NaviGrabTooltipLib();
    ~NaviGrabTooltipLib();
    
    // Initialization
    bool Initialize(const Config& config);
    void Shutdown();
    bool IsInitialized() const;
    
    // Screenshot operations
    ScreenshotResult CaptureElement(const ElementInfo& element);
    ScreenshotResult CaptureViewport();
    ScreenshotResult CaptureFullPage();
    
    // Image management
    bool StoreImage(const std::string& key, const std::vector<uint8_t>& image_data);
    std::vector<uint8_t> GetImage(const std::string& key);
    bool DeleteImage(const std::string& key);
    bool ImageExists(const std::string& key);
    
    // Tooltip display
    void ShowTooltip(const TooltipInfo& info);
    void HideTooltip();
    void UpdateTooltip(const TooltipInfo& info);
    
    // Settings
    void SetDarkMode(bool enabled);
    void SetStoragePath(const std::string& path);
    void SetMaxCacheSize(size_t size);
    
    // Web automation (if available)
    std::unique_ptr<WebAutomation> CreateWebAutomation();
    std::unique_ptr<Browser> CreateBrowser();
    std::unique_ptr<Page> CreatePage();
    std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
    
    // Element detection
    std::vector<ElementInfo> DetectInteractiveElements();
    std::vector<ElementInfo> DetectFormElements();
    std::vector<ElementInfo> DetectNavigationElements();
    
    // Callbacks
    using ElementClickCallback = std::function<void(const ElementInfo&)>;
    using ScreenshotCallback = std::function<void(const ScreenshotResult&)>;
    
    void SetElementClickCallback(ElementClickCallback callback);
    void SetScreenshotCallback(ScreenshotCallback callback);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Factory functions
std::unique_ptr<NaviGrabTooltipLib> CreateTooltipLib();
std::unique_ptr<WebAutomation> CreateWebAutomation();
std::unique_ptr<Browser> CreateBrowser();
std::unique_ptr<Page> CreatePage();
std::unique_ptr<ScreenshotCapture> CreateScreenshotCapture();
std::unique_ptr<ImageStorage> CreateImageStorage();
std::unique_ptr<TooltipIntegration> CreateTooltipIntegration();

} // namespace NaviGrabTooltip

#endif // NAVIGRAB_TOOLTIP_H
