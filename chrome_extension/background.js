/*
 Minimal MV3 background service worker
 - Toolbar click starts a fresh crawl on the active tab
 - While crawling, the toolbar icon animates (generated frames)
 - Stores a minimal result and uses options page for OpenAI API key
*/

const ANIMATION_FRAME_COUNT = 12;
const ANIMATION_INTERVAL_MS = 100;
const ICON_SIZES = [16, 32];

let isCrawling = false;
let animationIntervalId = null;
let currentFrameIndex = 0;
let precomputedFrames = null; // Array<{ imageDataBySize: Record<number, ImageData> }>

chrome.runtime.onInstalled.addListener(() => {
  // Set an initial static icon
  setStaticIcon();
});

chrome.action.onClicked.addListener(async (tab) => {
  if (isCrawling) {
    // Ignore clicks while in-flight to keep UX simple
    return;
  }

  try {
    isCrawling = true;
    await ensureFramesPrepared();
    startIconAnimation();

    // Trigger a fresh crawl on the active tab
    const activeTab = await getActiveTab();
    if (!activeTab || !activeTab.id) {
      throw new Error('No active tab to crawl');
    }

    const [{ result }] = await chrome.scripting.executeScript({
      target: { tabId: activeTab.id },
      world: 'MAIN',
      func: () => {
        const now = new Date().toISOString();
        const anchors = Array.from(document.querySelectorAll('a[href]'));
        const links = anchors.slice(0, 200).map((a) => ({
          href: a.href,
          text: (a.textContent || '').trim().slice(0, 200)
        }));
        return {
          crawledAt: now,
          url: location.href,
          title: document.title,
          linkCount: links.length,
          links
        };
      }
    });

    await chrome.storage.local.set({
      lastCrawl: {
        tabId: tab?.id ?? null,
        windowId: tab?.windowId ?? null,
        result
      }
    });
  } catch (error) {
    // Best-effort logging
    try { console.error('Crawl error', error); } catch (_) {}
  } finally {
    stopIconAnimation();
    setStaticIcon();
    isCrawling = false;
  }
});

async function getActiveTab() {
  const [active] = await chrome.tabs.query({ active: true, currentWindow: true });
  return active;
}

async function ensureFramesPrepared() {
  if (precomputedFrames) return;
  precomputedFrames = [];
  for (let i = 0; i < ANIMATION_FRAME_COUNT; i++) {
    precomputedFrames.push({ imageDataBySize: await generateSpiderFrame(i, ANIMATION_FRAME_COUNT) });
  }
}

function startIconAnimation() {
  if (animationIntervalId) return;
  currentFrameIndex = 0;
  animationIntervalId = setInterval(() => {
    const frame = precomputedFrames[currentFrameIndex];
    if (frame) {
      chrome.action.setIcon({ imageData: frame.imageDataBySize }).catch(() => {});
    }
    currentFrameIndex = (currentFrameIndex + 1) % precomputedFrames.length;
  }, ANIMATION_INTERVAL_MS);
}

function stopIconAnimation() {
  if (animationIntervalId) {
    clearInterval(animationIntervalId);
    animationIntervalId = null;
  }
}

function setStaticIcon() {
  // Draw a simple static spider for idle state
  generateSpiderStatic().then((imageDataBySize) => {
    chrome.action.setIcon({ imageData: imageDataBySize }).catch(() => {});
  });
}

async function generateSpiderStatic() {
  const bySize = {};
  for (const size of ICON_SIZES) {
    bySize[size] = await drawSpiderIcon(size, 0, 1, false);
  }
  return bySize;
}

async function generateSpiderFrame(frameIndex, totalFrames) {
  const bySize = {};
  for (const size of ICON_SIZES) {
    bySize[size] = await drawSpiderIcon(size, frameIndex, totalFrames, true);
  }
  return bySize;
}

async function drawSpiderIcon(size, frameIndex, totalFrames, animated) {
  const canvas = new OffscreenCanvas(size, size);
  const ctx = canvas.getContext('2d');
  if (!ctx) throw new Error('2D context unavailable');

  // Background: transparent
  ctx.clearRect(0, 0, size, size);

  const centerX = size / 2;
  const centerY = size / 2;
  const bodyRadius = Math.max(2, Math.floor(size * 0.22));

  // Simple pulse for animation
  const t = animated ? (frameIndex / totalFrames) : 0;
  const pulse = animated ? (0.85 + 0.15 * Math.sin(2 * Math.PI * t)) : 1;

  // Body
  ctx.fillStyle = '#111';
  ctx.beginPath();
  ctx.arc(centerX, centerY, bodyRadius * pulse, 0, Math.PI * 2);
  ctx.fill();

  // Head
  ctx.beginPath();
  ctx.arc(centerX, centerY - bodyRadius * 0.9, bodyRadius * 0.6, 0, Math.PI * 2);
  ctx.fill();

  // Legs (8)
  ctx.strokeStyle = '#111';
  ctx.lineWidth = Math.max(1, Math.floor(size * 0.07));
  const legLength = bodyRadius * 1.4;
  const spread = Math.PI / 3; // spread of legs on each side
  const wobble = animated ? (Math.sin(2 * Math.PI * t) * (Math.PI / 32)) : 0;

  for (let side = -1; side <= 1; side += 2) {
    for (let i = 0; i < 4; i++) {
      const baseAngle = (-spread / 2 + (i / 3) * spread) + wobble * (i + 1) * side;
      const angle = baseAngle + (side < 0 ? Math.PI : 0);
      const x1 = centerX + Math.cos(angle) * (bodyRadius * 0.8);
      const y1 = centerY + Math.sin(angle) * (bodyRadius * 0.8);
      const x2 = centerX + Math.cos(angle) * (bodyRadius * 0.8 + legLength * 0.6);
      const y2 = centerY + Math.sin(angle) * (bodyRadius * 0.8 + legLength * 0.6);
      const x3 = centerX + Math.cos(angle) * (bodyRadius * 0.8 + legLength);
      const y3 = centerY + Math.sin(angle) * (bodyRadius * 0.8 + legLength);

      ctx.beginPath();
      ctx.moveTo(x1, y1);
      ctx.lineTo(x2, y2);
      ctx.lineTo(x3, y3);
      ctx.stroke();
    }
  }

  // Subtle highlight eyes
  ctx.fillStyle = '#e33';
  const eyeOffset = bodyRadius * 0.35;
  const eyeR = Math.max(1, Math.floor(size * 0.07));
  ctx.beginPath();
  ctx.arc(centerX - eyeOffset, centerY - bodyRadius * 0.9, eyeR, 0, Math.PI * 2);
  ctx.arc(centerX + eyeOffset, centerY - bodyRadius * 0.9, eyeR, 0, Math.PI * 2);
  ctx.fill();

  return ctx.getImageData(0, 0, size, size);
}


