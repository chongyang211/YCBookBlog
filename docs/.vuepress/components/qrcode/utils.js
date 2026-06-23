/* ============================================================
 * QrcodeToolbox 工具函数
 *  - loadScript 白名单：仅允许 jsdelivr / unpkg 指定 npm 包
 *  - 模板生成器：WiFi / vCard / SMS / Mailto / Geo / 日历
 *  - 浏览器侧执行，全部数据不出本地
 * ============================================================ */

const CDN_WHITELIST = [
  'https://cdn.jsdelivr.net/npm/qrcode@1.5.3/build/qrcode.min.js',
  'https://cdn.jsdelivr.net/npm/jsqr@1.4.0/dist/jsQR.js',
  'https://cdn.jsdelivr.net/npm/jsbarcode@3.11.6/dist/JsBarcode.all.min.js'
];

const _loaded = new Map();

/** 白名单受控的脚本懒加载 */
export function loadScript(url) {
  if (!CDN_WHITELIST.includes(url)) {
    return Promise.reject(new Error('该脚本不在白名单内：' + url));
  }
  if (_loaded.has(url)) return _loaded.get(url);
  const p = new Promise((resolve, reject) => {
    const s = document.createElement('script');
    s.src = url;
    s.async = true;
    s.crossOrigin = 'anonymous';
    s.onload = () => resolve();
    s.onerror = () => {
      _loaded.delete(url);
      reject(new Error('加载失败：' + url));
    };
    document.head.appendChild(s);
  });
  _loaded.set(url, p);
  return p;
}

export const SCRIPTS = {
  qrcode: CDN_WHITELIST[0],
  jsqr: CDN_WHITELIST[1],
  jsbarcode: CDN_WHITELIST[2]
};

/* ---------- 模板生成 ---------- */

/** 转义 vCard / WiFi 内容里的特殊字符 */
function escWifi(s) {
  return String(s == null ? '' : s).replace(/([\\;,:"])/g, '\\$1');
}
function escVcard(s) {
  return String(s == null ? '' : s).replace(/([\\;,])/g, '\\$1').replace(/\n/g, '\\n');
}

export function buildWifi({ ssid, password, encryption = 'WPA', hidden = false }) {
  if (!ssid) throw new Error('SSID 不能为空');
  const T = encryption === 'nopass' ? 'nopass' : encryption;
  const P = T === 'nopass' ? '' : `P:${escWifi(password)};`;
  return `WIFI:T:${T};S:${escWifi(ssid)};${P}${hidden ? 'H:true;' : ''};`;
}

export function buildVcard(v) {
  const lines = ['BEGIN:VCARD', 'VERSION:3.0'];
  const name = `${escVcard(v.lastName || '')};${escVcard(v.firstName || '')};;;`;
  lines.push(`N:${name}`);
  lines.push(`FN:${escVcard((v.firstName || '') + ' ' + (v.lastName || '')).trim()}`);
  if (v.org) lines.push(`ORG:${escVcard(v.org)}`);
  if (v.title) lines.push(`TITLE:${escVcard(v.title)}`);
  if (v.phone) lines.push(`TEL;TYPE=CELL:${escVcard(v.phone)}`);
  if (v.email) lines.push(`EMAIL:${escVcard(v.email)}`);
  if (v.url) lines.push(`URL:${escVcard(v.url)}`);
  if (v.address) lines.push(`ADR:;;${escVcard(v.address)};;;;`);
  if (v.note) lines.push(`NOTE:${escVcard(v.note)}`);
  lines.push('END:VCARD');
  return lines.join('\n');
}

export function buildSms({ phone, body }) {
  return `SMSTO:${phone || ''}:${body || ''}`;
}

export function buildMailto({ to, subject, body }) {
  const q = [];
  if (subject) q.push('subject=' + encodeURIComponent(subject));
  if (body) q.push('body=' + encodeURIComponent(body));
  return `mailto:${to || ''}${q.length ? '?' + q.join('&') : ''}`;
}

export function buildGeo({ lat, lng }) {
  return `geo:${Number(lat) || 0},${Number(lng) || 0}`;
}

/** 复制文本（兼容 fallback） */
export async function copyText(text) {
  try {
    if (navigator.clipboard && window.isSecureContext) {
      await navigator.clipboard.writeText(text);
      return true;
    }
  } catch (e) {}
  const ta = document.createElement('textarea');
  ta.value = text;
  ta.style.position = 'fixed';
  ta.style.left = '-9999px';
  document.body.appendChild(ta);
  ta.select();
  let ok = false;
  try { ok = document.execCommand('copy'); } catch (e) {}
  document.body.removeChild(ta);
  return ok;
}

/** Canvas 下载为 PNG */
export function downloadCanvas(canvas, filename = 'qrcode.png') {
  const url = canvas.toDataURL('image/png');
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
}

/** 把 SVG 节点 / DOM 元素导出（用于条形码） */
export function downloadSvgAsPng(svgEl, filename = 'barcode.png') {
  const xml = new XMLSerializer().serializeToString(svgEl);
  const svg64 = btoa(unescape(encodeURIComponent(xml)));
  const img = new Image();
  img.onload = () => {
    const c = document.createElement('canvas');
    c.width = img.width || svgEl.clientWidth || 300;
    c.height = img.height || svgEl.clientHeight || 100;
    const ctx = c.getContext('2d');
    ctx.fillStyle = '#fff';
    ctx.fillRect(0, 0, c.width, c.height);
    ctx.drawImage(img, 0, 0);
    downloadCanvas(c, filename);
  };
  img.src = 'data:image/svg+xml;base64,' + svg64;
}

/** 文件→ImageData（识别用） */
export function fileToImageData(file) {
  return new Promise((resolve, reject) => {
    const fr = new FileReader();
    fr.onload = () => {
      const img = new Image();
      img.onload = () => {
        const c = document.createElement('canvas');
        c.width = img.naturalWidth;
        c.height = img.naturalHeight;
        const ctx = c.getContext('2d');
        ctx.drawImage(img, 0, 0);
        try {
          resolve(ctx.getImageData(0, 0, c.width, c.height));
        } catch (e) { reject(e); }
      };
      img.onerror = () => reject(new Error('图片解码失败'));
      img.src = fr.result;
    };
    fr.onerror = () => reject(new Error('文件读取失败'));
    fr.readAsDataURL(file);
  });
}

/** 把 Logo 叠加到 QR Canvas 中央，自动留白 */
export function drawLogoOnCanvas(canvas, logoImg, { ratio = 0.22, radius = 6, padding = 6 } = {}) {
  const ctx = canvas.getContext('2d');
  const size = Math.min(canvas.width, canvas.height) * ratio;
  const x = (canvas.width - size) / 2;
  const y = (canvas.height - size) / 2;
  // 白底
  ctx.save();
  roundRect(ctx, x - padding, y - padding, size + padding * 2, size + padding * 2, radius);
  ctx.fillStyle = '#fff';
  ctx.fill();
  ctx.restore();
  // Logo
  ctx.save();
  roundRect(ctx, x, y, size, size, Math.max(0, radius - 2));
  ctx.clip();
  ctx.drawImage(logoImg, x, y, size, size);
  ctx.restore();
}

function roundRect(ctx, x, y, w, h, r) {
  ctx.beginPath();
  ctx.moveTo(x + r, y);
  ctx.lineTo(x + w - r, y);
  ctx.quadraticCurveTo(x + w, y, x + w, y + r);
  ctx.lineTo(x + w, y + h - r);
  ctx.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
  ctx.lineTo(x + r, y + h);
  ctx.quadraticCurveTo(x, y + h, x, y + h - r);
  ctx.lineTo(x, y + r);
  ctx.quadraticCurveTo(x, y, x + r, y);
  ctx.closePath();
}
