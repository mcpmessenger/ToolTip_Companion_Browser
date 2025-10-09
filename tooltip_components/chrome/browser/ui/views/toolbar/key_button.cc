#include "chrome/browser/ui/views/toolbar/key_button.h"

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_string_value_serializer.h"
#include "base/values.h"
#include "chrome/browser/profiles/profile.h"
#include "ui/shell_dialogs/select_file_policy.h"
#include "base/win/windows_types.h"
#include "base/win/scoped_handle.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "ui/base/metadata/metadata_impl_macros.h"

KeyButton::KeyButton(BrowserView* browser_view)
    : views::LabelButton(base::BindRepeating(&KeyButton::OnPressed,
                                             base::Unretained(this)),
                         u"🔑"),
      browser_view_(browser_view) {
  LabelButton::SetTooltipText(u"Configure AI API keys");
}

KeyButton::~KeyButton() = default;

void KeyButton::OnPressed(const ui::Event& event) {
  EnsureConfigAndOpen();
}

void KeyButton::EnsureConfigAndOpen() {
  base::FilePath user_data_dir = browser_view_->GetProfile()->GetPath();
  base::FilePath keys_path = user_data_dir.Append(FILE_PATH_LITERAL("ai_keys.json"));

  if (!base::PathExists(keys_path)) {
    base::Value::Dict dict;
    dict.Set("google_gemini_api_key", "");
    dict.Set("openai_api_key", "");
    dict.Set("anthropic_api_key", "");
    std::string json;
    JSONStringValueSerializer ser(&json);
    ser.Serialize(dict);
    base::WriteFile(keys_path, json);
  }

  platform_util::OpenItem(browser_view_->GetProfile(), keys_path);
}

BEGIN_METADATA(KeyButton)
END_METADATA


