// Minimal Key button to open/edit AI API keys config.

#ifndef CHROME_BROWSER_UI_VIEWS_TOOLBAR_KEY_BUTTON_H_
#define CHROME_BROWSER_UI_VIEWS_TOOLBAR_KEY_BUTTON_H_

#include "base/memory/raw_ptr.h"
#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/controls/button/label_button.h"

class BrowserView;

class KeyButton : public views::LabelButton {
 public:
  METADATA_HEADER(KeyButton, views::LabelButton)

 public:

  explicit KeyButton(BrowserView* browser_view);
  KeyButton(const KeyButton&) = delete;
  KeyButton& operator=(const KeyButton&) = delete;
  ~KeyButton() override;

 private:
  void OnPressed(const ui::Event& event);
  void EnsureConfigAndOpen();

  raw_ptr<BrowserView> browser_view_ = nullptr;
};

#endif  // CHROME_BROWSER_UI_VIEWS_TOOLBAR_KEY_BUTTON_H_


