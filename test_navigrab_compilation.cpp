// Test file to validate NaviGrab compilation
#include "ScreenshotTooltipLibrary/src/navigrab/navigrab_core.h"
#include <iostream>

int main() {
    std::cout << "Testing NaviGrab compilation..." << std::endl;
    
    // Test factory functions
    auto web_automation = navigrab::CreateWebAutomation();
    auto browser = navigrab::CreateBrowser();
    auto page = navigrab::CreatePage();
    auto screenshot_capture = navigrab::CreateScreenshotCapture();
    auto locator = navigrab::CreateLocator();
    auto image_storage = navigrab::CreateImageStorage();
    auto tooltip_integration = navigrab::CreateTooltipIntegration();
    
    // Test NaviGrab core initialization
    navigrab::NaviGrabCore& core = navigrab::NaviGrabCore::GetInstance();
    if (core.Initialize()) {
        std::cout << "NaviGrab core initialized successfully!" << std::endl;
    }
    
    // Test WebAutomation methods
    if (web_automation) {
        std::string script_result = web_automation->ExecuteScript("console.log('test');");
        std::cout << "Script execution result: " << script_result << std::endl;
        
        // Test element interaction
        web_automation->ClickElement("button");
        web_automation->TypeText("input", "test text");
        web_automation->HoverElement("div");
        
        // Test content extraction
        std::string element_text = web_automation->GetElementText("p");
        std::string element_attr = web_automation->GetElementAttribute("a", "href");
        
        std::cout << "Element text: " << element_text << std::endl;
        std::cout << "Element attribute: " << element_attr << std::endl;
    }
    
    // Test ScreenshotCapture memory methods
    if (screenshot_capture) {
        std::vector<uint8_t> page_data = screenshot_capture->CapturePageData();
        std::vector<uint8_t> element_data = screenshot_capture->CaptureElementData("div");
        std::vector<uint8_t> thumbnail = screenshot_capture->GenerateThumbnail(page_data, 200, 150);
        
        std::cout << "Page data size: " << page_data.size() << " bytes" << std::endl;
        std::cout << "Element data size: " << element_data.size() << " bytes" << std::endl;
        std::cout << "Thumbnail size: " << thumbnail.size() << " bytes" << std::endl;
    }
    
    // Test ImageStorage
    if (image_storage) {
        image_storage->Initialize("./test_storage");
        
        std::vector<uint8_t> test_image = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        image_storage->StoreImage("test_key", test_image);
        
        std::vector<uint8_t> retrieved = image_storage->GetImage("test_key");
        std::cout << "Stored and retrieved image size: " << retrieved.size() << " bytes" << std::endl;
    }
    
    // Test TooltipIntegration
    if (tooltip_integration) {
        tooltip_integration->ShowTooltip("div", "Test tooltip content");
        tooltip_integration->SetDarkMode(true);
        tooltip_integration->SetTooltipStyle("modern");
        
        std::string element_info = tooltip_integration->ExtractElementInfo("button");
        std::vector<uint8_t> screenshot = tooltip_integration->CaptureElementScreenshot("div");
        
        std::cout << "Element info: " << element_info << std::endl;
        std::cout << "Screenshot size: " << screenshot.size() << " bytes" << std::endl;
    }
    
    std::cout << "All NaviGrab tests completed successfully!" << std::endl;
    return 0;
}

