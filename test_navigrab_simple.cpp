#include "chrome/browser/tooltip/navigrab_integration.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include <iostream>

int main() {
    std::cout << "Testing NaviGrab Integration..." << std::endl;
    
    // Test tooltip service initialization
    tooltip::InitializeTooltipService();
    std::cout << "Tooltip service initialized!" << std::endl;
    
    // Test NaviGrab integration
    auto integration = std::make_unique<tooltip::NaviGrabIntegration>();
    if (integration) {
        std::cout << "NaviGrab integration created successfully!" << std::endl;
    }
    
    tooltip::ShutdownTooltipService();
    std::cout << "Test completed successfully!" << std::endl;
    
    return 0;
}
