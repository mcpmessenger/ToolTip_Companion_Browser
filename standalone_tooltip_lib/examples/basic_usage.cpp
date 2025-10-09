#include "navigrab_tooltip.h"
#include <iostream>
#include <chrono>

using namespace NaviGrabTooltip;

int main() {
    std::cout << "NaviGrab ToolTip Library - Basic Usage Example" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    // Create library instance
    auto lib = CreateTooltipLib();
    if (!lib) {
        std::cerr << "Failed to create NaviGrab ToolTip library" << std::endl;
        return 1;
    }
    
    // Configure library
    Config config;
    config.storage_path = "./tooltip_storage";
    config.enable_dark_mode = true;
    config.max_cache_size_mb = 50;
    config.compression_quality = 90;
    
    // Initialize library
    if (!lib->Initialize(config)) {
        std::cerr << "Failed to initialize NaviGrab ToolTip library" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Library initialized successfully" << std::endl;
    
    // Create a sample element
    ElementInfo element;
    element.selector = "#my-button";
    element.tag_name = "button";
    element.text_content = "Click Me";
    element.x = 100;
    element.y = 100;
    element.width = 120;
    element.height = 40;
    element.attributes["class"] = "btn btn-primary";
    element.attributes["id"] = "my-button";
    
    std::cout << "✓ Created sample element: " << element.tag_name << " with text '" << element.text_content << "'" << std::endl;
    
    // Set up callbacks
    lib->SetElementClickCallback([](const ElementInfo& elem) {
        std::cout << "Element clicked: " << elem.tag_name << " - " << elem.text_content << std::endl;
    });
    
    lib->SetScreenshotCallback([](const ScreenshotResult& result) {
        if (result.success) {
            std::cout << "Screenshot captured: " << result.image_data.size() << " bytes" << std::endl;
        } else {
            std::cerr << "Screenshot failed: " << result.error_message << std::endl;
        }
    });
    
    // Capture element screenshot
    std::cout << "Capturing element screenshot..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    ScreenshotResult result = lib->CaptureElement(element);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    if (result.success) {
        std::cout << "✓ Screenshot captured successfully in " << duration.count() << "ms" << std::endl;
        std::cout << "  Image size: " << result.image_data.size() << " bytes" << std::endl;
        std::cout << "  Storage key: " << result.storage_key << std::endl;
        
        // Store the image
        if (lib->StoreImage(result.storage_key, result.image_data)) {
            std::cout << "✓ Image stored successfully" << std::endl;
        } else {
            std::cerr << "✗ Failed to store image" << std::endl;
        }
    } else {
        std::cerr << "✗ Screenshot failed: " << result.error_message << std::endl;
    }
    
    // Create tooltip info
    TooltipInfo tooltip;
    tooltip.title = "Interactive Button";
    tooltip.description = "This is a clickable button element with primary styling.";
    tooltip.screenshot = result.image_data;
    tooltip.x = element.x + element.width + 10;
    tooltip.y = element.y;
    tooltip.dark_mode = true;
    
    // Show tooltip
    std::cout << "Showing tooltip..." << std::endl;
    lib->ShowTooltip(tooltip);
    std::cout << "✓ Tooltip displayed" << std::endl;
    
    // Simulate some interaction
    std::cout << "\nSimulating user interaction..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Update tooltip
    tooltip.description = "Button clicked! This tooltip has been updated.";
    lib->UpdateTooltip(tooltip);
    std::cout << "✓ Tooltip updated" << std::endl;
    
    // Wait a bit more
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Hide tooltip
    lib->HideTooltip();
    std::cout << "✓ Tooltip hidden" << std::endl;
    
    // Test image retrieval
    if (result.success && !result.storage_key.empty()) {
        std::cout << "Testing image retrieval..." << std::endl;
        auto retrieved_image = lib->GetImage(result.storage_key);
        if (!retrieved_image.empty()) {
            std::cout << "✓ Image retrieved successfully: " << retrieved_image.size() << " bytes" << std::endl;
        } else {
            std::cerr << "✗ Failed to retrieve image" << std::endl;
        }
    }
    
    // Test element detection
    std::cout << "\nTesting element detection..." << std::endl;
    auto interactive_elements = lib->DetectInteractiveElements();
    std::cout << "Found " << interactive_elements.size() << " interactive elements" << std::endl;
    
    auto form_elements = lib->DetectFormElements();
    std::cout << "Found " << form_elements.size() << " form elements" << std::endl;
    
    auto nav_elements = lib->DetectNavigationElements();
    std::cout << "Found " << nav_elements.size() << " navigation elements" << std::endl;
    
    // Shutdown
    std::cout << "\nShutting down library..." << std::endl;
    lib->Shutdown();
    std::cout << "✓ Library shutdown complete" << std::endl;
    
    std::cout << "\n🎉 Basic usage example completed successfully!" << std::endl;
    return 0;
}
