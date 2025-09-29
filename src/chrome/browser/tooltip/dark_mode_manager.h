// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_TOOLTIP_DARK_MODE_MANAGER_H_
#define CHROME_BROWSER_TOOLTIP_DARK_MODE_MANAGER_H_

#include "base/check.h"
#include "base/memory/singleton.h"
#include "content/public/browser/web_contents.h"

namespace tooltip {

// Simple dark mode manager for tooltip system
class DarkModeManager {
 public:
  // Get the singleton instance
  static DarkModeManager* GetInstance();

  // Initialize the dark mode manager
  void Initialize();

  // Check if dark mode is currently enabled
  bool IsDarkModeEnabled() const;

  // Enable or disable dark mode
  void SetDarkModeEnabled(bool enabled);

  // Get the dark mode CSS to inject
  std::string GetDarkModeCSS() const;

  // Apply dark mode to a specific WebContents
  void ApplyDarkModeToWebContents(content::WebContents* web_contents);

 private:
  friend struct base::DefaultSingletonTraits<DarkModeManager>;

  DarkModeManager();
  ~DarkModeManager();

  // Generate the dark mode CSS
  void GenerateDarkModeCSS();

  // Current dark mode state
  bool is_dark_mode_enabled_;

  // Generated CSS for dark mode
  std::string dark_mode_css_;

  DarkModeManager(const DarkModeManager&) = delete;
  DarkModeManager& operator=(const DarkModeManager&) = delete;
};

}  // namespace tooltip

#endif  // CHROME_BROWSER_TOOLTIP_DARK_MODE_MANAGER_H_