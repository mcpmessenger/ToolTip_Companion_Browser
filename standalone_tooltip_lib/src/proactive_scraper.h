#pragma once

#include "navigrab_core.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>

namespace navigrab {

// Scraping depth levels
enum class ScrapingDepth {
    QUICK,      // 50ms - Fast element discovery
    STANDARD,   // 200ms - Standard analysis
    DEEP        // 800ms - Comprehensive analysis
};

// Element information structure
struct ElementInfo {
    std::string selector;
    std::string type;           // "button", "link", "input", etc.
    std::string text;
    std::string url;           // For links
    std::pair<int, int> position;
    std::pair<int, int> size;
    bool is_interactive;
    std::string screenshot_path;
    std::chrono::system_clock::time_point discovered_at;
    
    ElementInfo();
    ~ElementInfo();
    ElementInfo(const ElementInfo& other);
    ElementInfo& operator=(const ElementInfo& other);
};

// Scraping result structure
struct ScrapingResult {
    std::vector<ElementInfo> elements;
    int total_elements;
    int interactive_elements;
    std::chrono::milliseconds duration;
    std::string url;
    bool success;
    std::string error_message;
    
    ScrapingResult();
    ~ScrapingResult();
    ScrapingResult(const ScrapingResult& other);
    ScrapingResult& operator=(const ScrapingResult& other);
};

// Proactive scraper class
class ProactiveScraper {
public:
    ProactiveScraper();
    ~ProactiveScraper();
    
    // Configuration
    void SetScrapingDepth(ScrapingDepth depth);
    void SetCacheEnabled(bool enabled);
    void SetMaxElements(int maxElements);
    void SetScreenshotEnabled(bool enabled);
    
    // Main scraping functions
    ScrapingResult ScrapePage(const std::string& url, ScrapingDepth depth = ScrapingDepth::STANDARD);
    ScrapingResult ScrapePageInstant(const std::string& url);
    
    // Element discovery
    std::vector<ElementInfo> DiscoverElements(const std::string& url);
    std::vector<ElementInfo> DiscoverInteractiveElements(const std::string& url);
    std::vector<ElementInfo> DiscoverButtons(const std::string& url);
    std::vector<ElementInfo> DiscoverLinks(const std::string& url);
    
    // Screenshot capture
    bool CaptureElementScreenshot(const ElementInfo& element);
    bool CaptureAllElementScreenshots(const std::vector<ElementInfo>& elements);
    
    // Caching
    bool IsCached(const std::string& url) const;
    ScrapingResult GetCachedResult(const std::string& url) const;
    void CacheResult(const std::string& url, const ScrapingResult& result);
    void ClearCache();
    size_t GetCacheSize() const;
    
    // Statistics
    int GetTotalElementsDiscovered() const;
    int GetTotalScreenshotsCaptured() const;
    std::chrono::milliseconds GetAverageScrapingTime() const;
    
    // Utility functions
    bool IsElementInteractive(const ElementInfo& element) const;
    std::string GenerateElementSelector(const ElementInfo& element) const;
    std::vector<ElementInfo> FilterElementsByType(const std::vector<ElementInfo>& elements, const std::string& type) const;
    
    // Callbacks for progress reporting
    void SetProgressCallback(std::function<void(int, const std::string&)> callback);
    void SetElementDiscoveredCallback(std::function<void(const ElementInfo&)> callback);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Scraping session for managing multiple pages
class ScrapingSession {
public:
    ScrapingSession();
    ~ScrapingSession();
    
    // Session management
    void StartSession();
    void EndSession();
    bool IsActive() const;
    
    // Add pages to session
    void AddPage(const std::string& url);
    void AddPages(const std::vector<std::string>& urls);
    
    // Execute scraping
    std::vector<ScrapingResult> ScrapeAllPages(ScrapingDepth depth = ScrapingDepth::STANDARD);
    ScrapingResult ScrapeNextPage();
    
    // Session statistics
    int GetTotalPages() const;
    int GetCompletedPages() const;
    int GetRemainingPages() const;
    std::chrono::milliseconds GetSessionDuration() const;
    
    // Progress tracking
    void SetProgressCallback(std::function<void(int, int, const std::string&)> callback);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Utility functions
namespace scraper_utils {
    // Element classification
    bool IsButton(const std::string& selector);
    bool IsLink(const std::string& selector);
    bool IsInput(const std::string& selector);
    bool IsForm(const std::string& selector);
    
    // Selector generation
    std::string GenerateUniqueSelector(const ElementInfo& element);
    std::string GenerateCssSelector(const ElementInfo& element);
    
    // Performance optimization
    std::vector<ElementInfo> OptimizeElementList(const std::vector<ElementInfo>& elements, int maxElements = 100);
    bool ShouldSkipElement(const ElementInfo& element);
    
    // Cache management
    std::string GenerateCacheKey(const std::string& url);
    bool IsCacheValid(const std::chrono::system_clock::time_point& timestamp, std::chrono::minutes maxAge = std::chrono::minutes(30));
}

} // namespace navigrab

