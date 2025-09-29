// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_AI_INTEGRATION_H_
#define CHROME_BROWSER_TOOLTIP_AI_INTEGRATION_H_

#include <memory>
#include <string>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/tooltip/tooltip_service.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "ui/gfx/image/image.h"

namespace tooltip {

// AI configuration structure
struct AIConfig {
  bool enabled = false;
  std::string provider = "openai";
  std::string model = "gpt-3.5-turbo";
  int max_tokens = 150;
  double temperature = 0.7;
  std::string api_key;
  
  AIConfig();
};

// Integrates with AI providers for element description
class AIIntegration {
 public:
  AIIntegration();
  ~AIIntegration();

  // Initialize the AI integration
  void Initialize();

  // Shutdown the AI integration
  void Shutdown();

  // Analyze an element and get description
  void AnalyzeElement(const ElementInfo& element_info,
                     const gfx::Image& screenshot,
                     base::OnceCallback<void(const AIResponse&)> callback);

  // Set AI configuration
  void SetConfiguration(const AIConfig& config);

  // Get current AI configuration
  AIConfig GetConfiguration() const;

  // Get description for an element
  void GetDescription(const ElementInfo& element_info,
                     const gfx::Image& screenshot,
                     base::OnceCallback<void(const AIResponse&)> callback);

  // Test API key for a provider
  void TestAPIKey(const std::string& provider,
                 const std::string& api_key,
                 base::OnceCallback<void(bool, const std::string&)> callback);

  // Get available providers
  std::vector<std::string> GetAvailableProviders() const;

  // Check if provider is configured
  bool IsProviderConfigured(const std::string& provider) const;

 private:
  // Make API request to OpenAI
  void RequestOpenAI(const ElementInfo& element_info,
                    const gfx::Image& screenshot,
                    const std::string& api_key,
                    base::OnceCallback<void(const AIResponse&)> callback);

  // Make API request to Google Gemini
  void RequestGemini(const ElementInfo& element_info,
                    const gfx::Image& screenshot,
                    const std::string& api_key,
                    base::OnceCallback<void(const AIResponse&)> callback);

  // Make API request to Anthropic Claude
  void RequestAnthropic(const ElementInfo& element_info,
                       const gfx::Image& screenshot,
                       const std::string& api_key,
                       base::OnceCallback<void(const AIResponse&)> callback);

  // Process API response
  void ProcessResponse(const std::string& provider,
                      const std::string& response_data,
                      base::OnceCallback<void(const AIResponse&)> callback);

  // Convert image to base64
  std::string ImageToBase64(const gfx::Image& image);

  // Create prompt for AI
  std::string CreatePrompt(const ElementInfo& element_info);

  // Generate mock description for testing
  std::string GenerateMockDescription(const ElementInfo& element_info);

  // Generate mock actions for testing
  std::vector<std::string> GenerateMockActions(const ElementInfo& element_info);

  // Network traffic annotation
  static const net::NetworkTrafficAnnotationTag kTrafficAnnotation;

  // AI configuration
  AIConfig ai_config_;

  bool initialized_;
  base::WeakPtrFactory<AIIntegration> weak_factory_{this};

  AIIntegration(const AIIntegration&) = delete;
  AIIntegration& operator=(const AIIntegration&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_AI_INTEGRATION_H_

