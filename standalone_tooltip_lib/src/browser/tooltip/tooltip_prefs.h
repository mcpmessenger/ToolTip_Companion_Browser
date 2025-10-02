// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_TOOLTIP_PREFS_H_
#define CHROME_BROWSER_TOOLTIP_TOOLTIP_PREFS_H_

#include <string>

#include "base/check.h"
#include "base/values.h"
#include "components/prefs/pref_service.h"

class PrefRegistrySimple;

namespace tooltip {

// Manages tooltip-related preferences
class TooltipPrefs {
 public:
  TooltipPrefs();
  ~TooltipPrefs();

  // Initialize preferences
  void Initialize();

  // Register preference defaults
  static void RegisterProfilePrefs(PrefRegistrySimple* registry);

  // Tooltip behavior settings
  bool GetAutoCapture() const;
  void SetAutoCapture(bool enabled);

  bool GetDarkMode() const;
  void SetDarkMode(bool enabled);

  int GetTooltipDelay() const;
  void SetTooltipDelay(int delay_ms);

  bool GetVoiceEnabled() const;
  void SetVoiceEnabled(bool enabled);

  // AI provider settings
  std::string GetPreferredAIProvider() const;
  void SetPreferredAIProvider(const std::string& provider);

  std::string GetOpenAIAPIKey() const;
  void SetOpenAIAPIKey(const std::string& key);

  std::string GetGeminiAPIKey() const;
  void SetGeminiAPIKey(const std::string& key);

  std::string GetAnthropicAPIKey() const;
  void SetAnthropicAPIKey(const std::string& key);

  // Advanced settings
  bool GetDebugMode() const;
  void SetDebugMode(bool enabled);

  int GetMaxScreenshotSize() const;
  void SetMaxScreenshotSize(int size);

  int GetCacheSize() const;
  void SetCacheSize(int size);

 private:
  // Get the pref service
  PrefService* GetPrefService() const;

  // Preference keys
  static const char kAutoCapture[];
  static const char kDarkMode[];
  static const char kTooltipDelay[];
  static const char kVoiceEnabled[];
  static const char kPreferredAIProvider[];
  static const char kOpenAIAPIKey[];
  static const char kGeminiAPIKey[];
  static const char kAnthropicAPIKey[];
  static const char kDebugMode[];
  static const char kMaxScreenshotSize[];
  static const char kCacheSize[];

  TooltipPrefs(const TooltipPrefs&) = delete;
  TooltipPrefs& operator=(const TooltipPrefs&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_TOOLTIP_PREFS_H_
