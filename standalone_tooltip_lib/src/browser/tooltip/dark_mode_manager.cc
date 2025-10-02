// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "dark_mode_manager.h"

#ifdef STANDALONE_TOOLTIP_BUILD
#include "base/base_stubs.h"
#else
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#endif
#include "chrome/browser/tooltip/tooltip_prefs.h"
#include "content/public/browser/web_contents.h"

namespace tooltip {

// static
DarkModeManager* DarkModeManager::GetInstance() {
  return base::Singleton<DarkModeManager>::get();
}

DarkModeManager::DarkModeManager() : is_dark_mode_enabled_(false) {
  VLOG(1) << "🔧 TOOLTIP: DarkModeManager created.";
}

DarkModeManager::~DarkModeManager() {
  VLOG(1) << "🔧 TOOLTIP: DarkModeManager destroyed.";
}

void DarkModeManager::Initialize() {
  VLOG(1) << "🔧 TOOLTIP: Initializing DarkModeManager...";
  
  // Initialize with preference value
  TooltipPrefs prefs;
  is_dark_mode_enabled_ = prefs.GetDarkMode();
  
  // Generate the dark mode CSS
  GenerateDarkModeCSS();
  
  VLOG(1) << "🔧 TOOLTIP: DarkModeManager initialized. Dark mode: " 
          << (is_dark_mode_enabled_ ? "enabled" : "disabled");
  
  // Log for testing purposes
  LOG(INFO) << "🔧 TOOLTIP: Dark mode manager initialized successfully!";
  LOG(INFO) << "🔧 TOOLTIP: Dark mode status: " << (is_dark_mode_enabled_ ? "ENABLED" : "DISABLED");
}

bool DarkModeManager::IsDarkModeEnabled() const {
  return is_dark_mode_enabled_;
}

void DarkModeManager::SetDarkModeEnabled(bool enabled) {
  if (is_dark_mode_enabled_ == enabled) {
    return; // No change needed
  }

  VLOG(1) << "🔧 TOOLTIP: Setting dark mode to: " << (enabled ? "enabled" : "disabled");
  
  is_dark_mode_enabled_ = enabled;
  
  // Save to preferences
  TooltipPrefs prefs;
  prefs.SetDarkMode(enabled);
  
  VLOG(1) << "🔧 TOOLTIP: Dark mode changed successfully.";
  
  // Log for testing purposes
  LOG(INFO) << "🔧 TOOLTIP: Dark mode toggled to: " << (enabled ? "ENABLED" : "DISABLED");
}

std::string DarkModeManager::GetDarkModeCSS() const {
  return dark_mode_css_;
}

void DarkModeManager::GenerateDarkModeCSS() {
  // Generate a clean grey dark mode CSS without blue/purple tints
  dark_mode_css_ = R"(
    /* Tooltip Dark Mode - Clean Grey Theme */
    * {
      background-color: #2d2d2d !important;
      color: #e0e0e0 !important;
    }
    
    /* Text elements */
    p, span, div, h1, h2, h3, h4, h5, h6, a, li, td, th {
      color: #e0e0e0 !important;
    }
    
    /* Links */
    a {
      color: #4a9eff !important;
    }
    
    a:visited {
      color: #8a6bb8 !important;
    }
    
    a:hover {
      color: #6bb6ff !important;
    }
    
    /* Form elements */
    input, textarea, select, button {
      background-color: #3d3d3d !important;
      color: #e0e0e0 !important;
      border: 1px solid #555 !important;
    }
    
    input:focus, textarea:focus, select:focus {
      border-color: #4a9eff !important;
      outline: none !important;
    }
    
    /* Buttons */
    button {
      background-color: #404040 !important;
      border: 1px solid #555 !important;
    }
    
    button:hover {
      background-color: #4a4a4a !important;
    }
    
    button:active {
      background-color: #353535 !important;
    }
    
    /* Tables */
    table {
      background-color: #2d2d2d !important;
    }
    
    th {
      background-color: #3d3d3d !important;
    }
    
    tr:nth-child(even) {
      background-color: #333 !important;
    }
    
    /* Code blocks */
    code, pre {
      background-color: #1e1e1e !important;
      color: #d4d4d4 !important;
    }
    
    /* Scrollbars */
    ::-webkit-scrollbar {
      background-color: #2d2d2d !important;
    }
    
    ::-webkit-scrollbar-thumb {
      background-color: #555 !important;
    }
    
    ::-webkit-scrollbar-thumb:hover {
      background-color: #666 !important;
    }
    
    /* Images - reduce brightness slightly */
    img {
      opacity: 0.9 !important;
    }
    
    /* Remove any blue/purple tints from existing styles */
    * {
      filter: none !important;
    }
  )";
  
  VLOG(1) << "🔧 TOOLTIP: Generated dark mode CSS (" << dark_mode_css_.length() << " characters)";
}

void DarkModeManager::ApplyDarkModeToWebContents(content::WebContents* web_contents) {
  if (!web_contents) {
    VLOG(1) << "🔧 TOOLTIP: WebContents is null, cannot apply dark mode";
    return;
  }

  if (!is_dark_mode_enabled_) {
    VLOG(1) << "🔧 TOOLTIP: Dark mode is disabled, skipping CSS injection";
    return;
  }

  VLOG(1) << "🔧 TOOLTIP: Applying dark mode to WebContents";

  // Create JavaScript to inject the CSS
  std::string script = R"(
    (function() {
      // Remove any existing dark mode styles
      const existingStyle = document.getElementById('tooltip-dark-mode-style');
      if (existingStyle) {
        existingStyle.remove();
      }

      // Create new style element
      const style = document.createElement('style');
      style.id = 'tooltip-dark-mode-style';
      style.type = 'text/css';
      style.textContent = `)" + dark_mode_css_ + R"(`;

      // Insert the style into the document head
      if (document.head) {
        document.head.appendChild(style);
      } else {
        // Fallback: insert at the beginning of body
        document.body.insertBefore(style, document.body.firstChild);
      }

      console.log('🔧 TOOLTIP: Dark mode CSS injected successfully');
    })();
  )";

  // Execute the JavaScript
  web_contents->GetPrimaryMainFrame()->ExecuteJavaScript(
      base::UTF8ToUTF16(script), base::NullCallback());

  VLOG(1) << "🔧 TOOLTIP: Dark mode CSS injection completed";
}

}  // namespace tooltip