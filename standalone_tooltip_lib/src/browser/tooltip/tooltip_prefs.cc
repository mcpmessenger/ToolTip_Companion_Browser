// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/tooltip/tooltip_prefs.h"

#include "base/values.h"
#include "chrome/common/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace tooltip {

// Preference keys
const char TooltipPrefs::kAutoCapture[] = "tooltip.auto_capture";
const char TooltipPrefs::kDarkMode[] = "tooltip.dark_mode";
const char TooltipPrefs::kTooltipDelay[] = "tooltip.delay";
const char TooltipPrefs::kVoiceEnabled[] = "tooltip.voice_enabled";
const char TooltipPrefs::kPreferredAIProvider[] = "tooltip.preferred_ai_provider";
const char TooltipPrefs::kOpenAIAPIKey[] = "tooltip.openai_api_key";
const char TooltipPrefs::kGeminiAPIKey[] = "tooltip.gemini_api_key";
const char TooltipPrefs::kAnthropicAPIKey[] = "tooltip.anthropic_api_key";
const char TooltipPrefs::kDebugMode[] = "tooltip.debug_mode";
const char TooltipPrefs::kMaxScreenshotSize[] = "tooltip.max_screenshot_size";
const char TooltipPrefs::kCacheSize[] = "tooltip.cache_size";

TooltipPrefs::TooltipPrefs() = default;
TooltipPrefs::~TooltipPrefs() = default;

void TooltipPrefs::Initialize() {
  // Initialize with default values if needed
  // This will be called when the service starts
}

void TooltipPrefs::RegisterProfilePrefs(PrefRegistrySimple* registry) {
  // Register all tooltip preferences with default values
  registry->RegisterBooleanPref(kAutoCapture, true);
  registry->RegisterBooleanPref(kDarkMode, false);
  registry->RegisterIntegerPref(kTooltipDelay, 500);
  registry->RegisterBooleanPref(kVoiceEnabled, false);
  registry->RegisterStringPref(kPreferredAIProvider, "openai");
  registry->RegisterStringPref(kOpenAIAPIKey, "");
  registry->RegisterStringPref(kGeminiAPIKey, "");
  registry->RegisterStringPref(kAnthropicAPIKey, "");
  registry->RegisterBooleanPref(kDebugMode, false);
  registry->RegisterIntegerPref(kMaxScreenshotSize, 1024);
  registry->RegisterIntegerPref(kCacheSize, 100);
}

PrefService* TooltipPrefs::GetPrefService() const {
  // TODO: Get the actual pref service from the browser context
  // For now, return nullptr - this will be implemented when we integrate
  // with the browser's preference system
  return nullptr;
}

// Tooltip behavior settings
bool TooltipPrefs::GetAutoCapture() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return true; // Default value
  return prefs->GetBoolean(kAutoCapture);
}

void TooltipPrefs::SetAutoCapture(bool enabled) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetBoolean(kAutoCapture, enabled);
  }
}

bool TooltipPrefs::GetDarkMode() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return false; // Default value
  return prefs->GetBoolean(kDarkMode);
}

void TooltipPrefs::SetDarkMode(bool enabled) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetBoolean(kDarkMode, enabled);
  }
}

int TooltipPrefs::GetTooltipDelay() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return 500; // Default value
  return prefs->GetInteger(kTooltipDelay);
}

void TooltipPrefs::SetTooltipDelay(int delay_ms) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetInteger(kTooltipDelay, delay_ms);
  }
}

bool TooltipPrefs::GetVoiceEnabled() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return false; // Default value
  return prefs->GetBoolean(kVoiceEnabled);
}

void TooltipPrefs::SetVoiceEnabled(bool enabled) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetBoolean(kVoiceEnabled, enabled);
  }
}

// AI provider settings
std::string TooltipPrefs::GetPreferredAIProvider() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return "openai"; // Default value
  return prefs->GetString(kPreferredAIProvider);
}

void TooltipPrefs::SetPreferredAIProvider(const std::string& provider) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetString(kPreferredAIProvider, provider);
  }
}

std::string TooltipPrefs::GetOpenAIAPIKey() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return ""; // Default value
  return prefs->GetString(kOpenAIAPIKey);
}

void TooltipPrefs::SetOpenAIAPIKey(const std::string& key) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetString(kOpenAIAPIKey, key);
  }
}

std::string TooltipPrefs::GetGeminiAPIKey() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return ""; // Default value
  return prefs->GetString(kGeminiAPIKey);
}

void TooltipPrefs::SetGeminiAPIKey(const std::string& key) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetString(kGeminiAPIKey, key);
  }
}

std::string TooltipPrefs::GetAnthropicAPIKey() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return ""; // Default value
  return prefs->GetString(kAnthropicAPIKey);
}

void TooltipPrefs::SetAnthropicAPIKey(const std::string& key) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetString(kAnthropicAPIKey, key);
  }
}

// Advanced settings
bool TooltipPrefs::GetDebugMode() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return false; // Default value
  return prefs->GetBoolean(kDebugMode);
}

void TooltipPrefs::SetDebugMode(bool enabled) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetBoolean(kDebugMode, enabled);
  }
}

int TooltipPrefs::GetMaxScreenshotSize() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return 1024; // Default value
  return prefs->GetInteger(kMaxScreenshotSize);
}

void TooltipPrefs::SetMaxScreenshotSize(int size) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetInteger(kMaxScreenshotSize, size);
  }
}

int TooltipPrefs::GetCacheSize() const {
  PrefService* prefs = GetPrefService();
  if (!prefs) return 100; // Default value
  return prefs->GetInteger(kCacheSize);
}

void TooltipPrefs::SetCacheSize(int size) {
  PrefService* prefs = GetPrefService();
  if (prefs) {
    prefs->SetInteger(kCacheSize, size);
  }
}

}  // namespace tooltip

