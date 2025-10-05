document.addEventListener('DOMContentLoaded', async () => {
  try {
    const { openaiApiKey } = await chrome.storage.local.get('openaiApiKey');
    if (openaiApiKey) {
      document.getElementById('openai').value = openaiApiKey;
    }
  } catch (_) {}
});

document.getElementById('save').addEventListener('click', async () => {
  const key = document.getElementById('openai').value.trim();
  await chrome.storage.local.set({ openaiApiKey: key });
  const status = document.getElementById('status');
  status.textContent = 'Saved';
  setTimeout(() => (status.textContent = ''), 1500);
});


