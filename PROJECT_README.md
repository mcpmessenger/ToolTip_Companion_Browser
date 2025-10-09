# ToolTip Companion Browser - Project Status and How to Run

## Current Status

- SpiderButton is integrated into the Chromium toolbar and visible in builds from `out/Default`.
- Clicking Spider triggers a demo crawl and now captures a Navigrab-based viewport image; hovering the Spider shows a preview window.
- DarkModeButton remains disabled (compile issues previously resolved, but not enabled to focus on Spider flow).
- Key icon (API key settings) planned: set Google Gemini, OpenAI, or Anthropic keys.

## How to Launch the Custom Build (Windows PowerShell)

```powershell
& 'C:\chromium\src\src\out\Default\chrome.exe' --no-sandbox --disable-gpu --user-data-dir='C:\chromium\src\src\out\Default\UserData_SpiderTest'
```

If you prefer to change directory first:

```powershell
cd C:\chromium\src\src\out\Default
./chrome.exe --no-sandbox --disable-gpu --user-data-dir=./UserData_SpiderTest
```

Note: `initialexe\chrome.exe` may fail with a side-by-side error on some machines; use `out/Default/chrome.exe` as above.

## How to Test the Spider

1. Launch with the command above.
2. In the toolbar, click the Spider (🕷️) to start a short demo crawl; it stops automatically after a couple seconds.
3. Hover over the Spider; a small image preview should appear (Navigrab capture). If not, click Spider again, wait a second, then hover.

## API Key Setup (Planned Key Icon)

A Key icon will appear in the toolbar to manage AI API keys used for enhanced crawling and tooltip generation. Until the Key icon UI lands, you can preconfigure keys using either environment variables or a local config file.

### Option 1: Environment Variables

Set these in PowerShell before launching the browser:

```powershell
$env:GOOGLE_GEMINI_API_KEY = 'your-gemini-key'
$env:OPENAI_API_KEY = 'your-openai-key'
$env:ANTHROPIC_API_KEY = 'your-anthropic-key'
```

These values will be read by the app when the Key icon feature is enabled.

### Option 2: Local Config File

Create a file at `C:\chromium\src\src\out\Default\ai_keys.json`:

```json
{
  "google_gemini_api_key": "your-gemini-key",
  "openai_api_key": "your-openai-key",
  "anthropic_api_key": "your-anthropic-key"
}
```

The Key icon UI will allow editing and storing these securely (local profile scope).

## What Works

- Toolbar integration for SpiderButton with fixed metadata and accessibility.
- Navigrab memory capture wired for a preview image on hover (with window snapshot fallback).
- Incremental rebuilds (`autoninja -C out/Default chrome`) succeed after code changes.

## Next Steps

- Wire SpiderButton to real per-element crawl: click each discovered element, capture screenshot, map preview to hover.
- Add Key icon to toolbar to manage AI keys; read from env vars and `ai_keys.json` fallback.
- Re-enable and polish DarkModeButton.
- Persist captured previews and show richer tooltip content.

## Troubleshooting

- If builds fail with permission denied while linking, close all custom Chromium windows and rebuild.
- If you see side-by-side errors, ensure you launch `out/Default/chrome.exe` (not `initialexe\chrome.exe`).


