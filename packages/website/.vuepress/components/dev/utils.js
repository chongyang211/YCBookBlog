/* ============================================================
 * DevToolbox 工具函数（纯前端，无外部依赖）
 * ============================================================ */

/* ---------- UUID / 短 ID ---------- */

/** RFC 4122 v4 UUID，优先用 crypto.randomUUID */
export function uuidV4() {
  if (typeof crypto !== 'undefined' && crypto.randomUUID) {
    return crypto.randomUUID();
  }
  const b = new Uint8Array(16);
  crypto.getRandomValues(b);
  b[6] = (b[6] & 0x0f) | 0x40;
  b[8] = (b[8] & 0x3f) | 0x80;
  const h = Array.from(b, x => x.toString(16).padStart(2, '0')).join('');
  return `${h.slice(0,8)}-${h.slice(8,12)}-${h.slice(12,16)}-${h.slice(16,20)}-${h.slice(20)}`;
}

/** NanoID 风格（URL 安全字符集） */
const NANO_ALPHABET = 'useandom-26T198340PX75pxJACKVERYMINDBUSHWOLF_GQZbfghjklqvwyzrict';
export function nanoId(size = 21) {
  const arr = new Uint8Array(size);
  crypto.getRandomValues(arr);
  let id = '';
  for (let i = 0; i < size; i++) id += NANO_ALPHABET[arr[i] & 63];
  return id;
}

/** 短 ID：基于时间戳 + 随机 */
export function shortId(len = 8) {
  const alphabet = 'ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789';
  const arr = new Uint8Array(len);
  crypto.getRandomValues(arr);
  let s = '';
  for (let i = 0; i < len; i++) s += alphabet[arr[i] % alphabet.length];
  return s;
}

/* ---------- 随机密码 ---------- */

const CHARSET = {
  lower: 'abcdefghijklmnopqrstuvwxyz',
  upper: 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
  number: '0123456789',
  symbol: '!@#$%^&*()-_=+[]{};:,.<>?/'
};
const AMBIGUOUS = /[O0Il1|`'"\\]/g;

export function genPassword({ length = 16, lower = true, upper = true, number = true, symbol = true, excludeAmbiguous = false }) {
  let pool = '';
  const picks = [];
  if (lower)  { pool += CHARSET.lower;  picks.push(CHARSET.lower); }
  if (upper)  { pool += CHARSET.upper;  picks.push(CHARSET.upper); }
  if (number) { pool += CHARSET.number; picks.push(CHARSET.number); }
  if (symbol) { pool += CHARSET.symbol; picks.push(CHARSET.symbol); }
  if (excludeAmbiguous) {
    pool = pool.replace(AMBIGUOUS, '');
    for (let i = 0; i < picks.length; i++) picks[i] = picks[i].replace(AMBIGUOUS, '');
  }
  if (!pool) throw new Error('请至少选择一类字符');
  if (length < picks.length) throw new Error('长度不能少于已选字符类型数');
  const arr = new Uint32Array(length);
  crypto.getRandomValues(arr);
  const chars = [];
  // 确保每类至少一个
  for (let i = 0; i < picks.length; i++) {
    const r = new Uint32Array(1); crypto.getRandomValues(r);
    chars.push(picks[i][r[0] % picks[i].length]);
  }
  for (let i = picks.length; i < length; i++) {
    chars.push(pool[arr[i] % pool.length]);
  }
  // Fisher–Yates 洗牌
  for (let i = chars.length - 1; i > 0; i--) {
    const r = new Uint32Array(1); crypto.getRandomValues(r);
    const j = r[0] % (i + 1);
    [chars[i], chars[j]] = [chars[j], chars[i]];
  }
  return chars.join('');
}

/** 简易强度评估 0~4 + 描述 */
export function passwordStrength(pw) {
  if (!pw) return { score: 0, label: '空', cls: '' };
  let score = 0;
  if (pw.length >= 8) score++;
  if (pw.length >= 12) score++;
  let kinds = 0;
  if (/[a-z]/.test(pw)) kinds++;
  if (/[A-Z]/.test(pw)) kinds++;
  if (/[0-9]/.test(pw)) kinds++;
  if (/[^a-zA-Z0-9]/.test(pw)) kinds++;
  if (kinds >= 3) score++;
  if (kinds === 4 && pw.length >= 16) score++;
  const map = [
    { label: '极弱', cls: 'dv-strength-weak',    pct: 20 },
    { label: '弱',   cls: 'dv-strength-weak',    pct: 35 },
    { label: '中',   cls: 'dv-strength-medium',  pct: 55 },
    { label: '强',   cls: 'dv-strength-strong',  pct: 80 },
    { label: '极强', cls: 'dv-strength-vstrong', pct: 100 }
  ];
  return { score, ...map[Math.min(score, 4)] };
}

/* ---------- 进制转换 ---------- */

/**
 * 任意进制 -> BigInt（base 2~36）
 * 支持负数与小写
 */
function parseBig(str, base) {
  const s = String(str).trim();
  if (!s) throw new Error('请输入数值');
  const neg = s.startsWith('-');
  const body = (neg ? s.slice(1) : s).toLowerCase();
  if (!body) throw new Error('请输入数值');
  const digits = '0123456789abcdefghijklmnopqrstuvwxyz'.slice(0, base);
  let n = 0n;
  const B = BigInt(base);
  for (const ch of body) {
    const i = digits.indexOf(ch);
    if (i < 0) throw new Error(`非法字符 "${ch}"（${base} 进制）`);
    n = n * B + BigInt(i);
  }
  return neg ? -n : n;
}

function bigToBase(n, base) {
  if (n === 0n) return '0';
  const neg = n < 0n;
  let x = neg ? -n : n;
  const digits = '0123456789abcdefghijklmnopqrstuvwxyz';
  const B = BigInt(base);
  let s = '';
  while (x > 0n) {
    s = digits[Number(x % B)] + s;
    x = x / B;
  }
  return neg ? '-' + s : s;
}

export function convertBase(value, fromBase, toBase) {
  const n = parseBig(value, fromBase);
  return bigToBase(n, toBase);
}

/* ---------- 单位换算 ---------- */

/** 各单位以括号内基准换算的乘数 */
export const UNIT_GROUPS = {
  length: {
    name: '长度（基准：米 m）',
    units: [
      { id: 'mm', label: '毫米 mm', factor: 0.001 },
      { id: 'cm', label: '厘米 cm', factor: 0.01 },
      { id: 'm',  label: '米 m',    factor: 1 },
      { id: 'km', label: '千米 km', factor: 1000 },
      { id: 'in', label: '英寸 in', factor: 0.0254 },
      { id: 'ft', label: '英尺 ft', factor: 0.3048 },
      { id: 'yd', label: '码 yd',   factor: 0.9144 },
      { id: 'mi', label: '英里 mi', factor: 1609.344 }
    ]
  },
  weight: {
    name: '质量（基准：千克 kg）',
    units: [
      { id: 'mg', label: '毫克 mg', factor: 1e-6 },
      { id: 'g',  label: '克 g',    factor: 0.001 },
      { id: 'kg', label: '千克 kg', factor: 1 },
      { id: 't',  label: '吨 t',    factor: 1000 },
      { id: 'oz', label: '盎司 oz', factor: 0.02834952 },
      { id: 'lb', label: '磅 lb',   factor: 0.45359237 },
      { id: 'jin',label: '斤',      factor: 0.5 }
    ]
  },
  area: {
    name: '面积（基准：平方米 m²）',
    units: [
      { id: 'mm2', label: '平方毫米 mm²', factor: 1e-6 },
      { id: 'cm2', label: '平方厘米 cm²', factor: 1e-4 },
      { id: 'm2',  label: '平方米 m²',    factor: 1 },
      { id: 'km2', label: '平方千米 km²', factor: 1e6 },
      { id: 'ha',  label: '公顷 ha',      factor: 1e4 },
      { id: 'mu',  label: '亩',           factor: 666.6667 },
      { id: 'ac',  label: '英亩 ac',      factor: 4046.8564 }
    ]
  },
  storage: {
    name: '数据（基准：字节 B，IEC 1024 进制）',
    units: [
      { id: 'b',   label: '位 bit',   factor: 0.125 },
      { id: 'B',   label: '字节 B',    factor: 1 },
      { id: 'KiB', label: 'KiB',      factor: 1024 },
      { id: 'MiB', label: 'MiB',      factor: 1024 ** 2 },
      { id: 'GiB', label: 'GiB',      factor: 1024 ** 3 },
      { id: 'TiB', label: 'TiB',      factor: 1024 ** 4 },
      { id: 'KB',  label: 'KB (1e3)', factor: 1000 },
      { id: 'MB',  label: 'MB (1e6)', factor: 1e6 },
      { id: 'GB',  label: 'GB (1e9)', factor: 1e9 }
    ]
  },
  time: {
    name: '时间（基准：秒 s）',
    units: [
      { id: 'ms',  label: '毫秒 ms', factor: 0.001 },
      { id: 's',   label: '秒 s',    factor: 1 },
      { id: 'min', label: '分 min',  factor: 60 },
      { id: 'h',   label: '小时 h',  factor: 3600 },
      { id: 'd',   label: '天 d',    factor: 86400 },
      { id: 'wk',  label: '周',      factor: 604800 }
    ]
  },
  temp: {
    name: '温度（特殊：使用换算函数）',
    units: [
      { id: 'C', label: '摄氏度 °C' },
      { id: 'F', label: '华氏度 °F' },
      { id: 'K', label: '开氏度 K' }
    ]
  }
};

export function convertUnit(group, fromId, toId, value) {
  const v = Number(value);
  if (!Number.isFinite(v)) throw new Error('请输入有效数字');
  if (group === 'temp') return convertTemp(fromId, toId, v);
  const list = UNIT_GROUPS[group].units;
  const from = list.find(u => u.id === fromId);
  const to = list.find(u => u.id === toId);
  if (!from || !to) throw new Error('未知单位');
  return (v * from.factor) / to.factor;
}

function convertTemp(from, to, v) {
  // -> C
  let c;
  if (from === 'C') c = v;
  else if (from === 'F') c = (v - 32) * 5 / 9;
  else c = v - 273.15; // K
  if (to === 'C') return c;
  if (to === 'F') return c * 9 / 5 + 32;
  return c + 273.15;
}

/* ---------- CSV ↔ JSON ---------- */

/**
 * RFC 4180 简易解析：
 *  - 支持 " 包裹字段
 *  - 字段内 "" → "
 *  - 字段内 \n / 分隔符 合法（在引号内）
 *  - 行尾 \n / \r\n
 */
export function parseCsv(text, delimiter = ',') {
  if (text == null) return [];
  const rows = [];
  let row = [], cell = '', i = 0, inQ = false;
  const s = String(text);
  const n = s.length;
  while (i < n) {
    const ch = s[i];
    if (inQ) {
      if (ch === '"') {
        if (s[i + 1] === '"') { cell += '"'; i += 2; continue; }
        inQ = false; i++; continue;
      }
      cell += ch; i++; continue;
    }
    if (ch === '"') { inQ = true; i++; continue; }
    if (ch === delimiter) { row.push(cell); cell = ''; i++; continue; }
    if (ch === '\r') { i++; continue; }
    if (ch === '\n') { row.push(cell); rows.push(row); row = []; cell = ''; i++; continue; }
    cell += ch; i++;
  }
  // 末尾
  if (cell !== '' || row.length) { row.push(cell); rows.push(row); }
  return rows;
}

export function csvToJson(text, { delimiter = ',', headerFirstRow = true } = {}) {
  const rows = parseCsv(text, delimiter).filter(r => r.length && !(r.length === 1 && r[0] === ''));
  if (!rows.length) return [];
  if (!headerFirstRow) return rows.map(r => r.slice());
  const head = rows[0];
  return rows.slice(1).map(r => {
    const obj = {};
    head.forEach((k, i) => { obj[k] = r[i] != null ? r[i] : ''; });
    return obj;
  });
}

function csvEscape(v, delimiter) {
  const s = v == null ? '' : String(v);
  if (s.includes('"') || s.includes(delimiter) || s.includes('\n') || s.includes('\r')) {
    return '"' + s.replace(/"/g, '""') + '"';
  }
  return s;
}

export function jsonToCsv(json, { delimiter = ',' } = {}) {
  let data = json;
  if (typeof json === 'string') data = JSON.parse(json);
  if (!Array.isArray(data)) throw new Error('JSON 必须是数组');
  if (!data.length) return '';
  // 收集所有 key（按首次出现顺序）
  const keys = [];
  const seen = new Set();
  for (const row of data) {
    if (row && typeof row === 'object' && !Array.isArray(row)) {
      for (const k of Object.keys(row)) {
        if (!seen.has(k)) { seen.add(k); keys.push(k); }
      }
    }
  }
  const lines = [];
  if (keys.length) {
    lines.push(keys.map(k => csvEscape(k, delimiter)).join(delimiter));
    for (const row of data) {
      lines.push(keys.map(k => csvEscape(row && row[k], delimiter)).join(delimiter));
    }
  } else {
    // 数组的数组
    for (const row of data) {
      if (Array.isArray(row)) lines.push(row.map(c => csvEscape(c, delimiter)).join(delimiter));
      else lines.push(csvEscape(row, delimiter));
    }
  }
  return lines.join('\n');
}

/* ---------- 通用 ---------- */

export async function copyText(text) {
  try {
    if (navigator.clipboard && window.isSecureContext) {
      await navigator.clipboard.writeText(text);
      return true;
    }
  } catch (e) {}
  const ta = document.createElement('textarea');
  ta.value = text;
  ta.style.position = 'fixed'; ta.style.left = '-9999px';
  document.body.appendChild(ta);
  ta.select();
  let ok = false;
  try { ok = document.execCommand('copy'); } catch (e) {}
  document.body.removeChild(ta);
  return ok;
}

export function downloadText(text, filename, mime = 'text/plain;charset=utf-8') {
  const blob = new Blob([text], { type: mime });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url; a.download = filename;
  document.body.appendChild(a); a.click();
  document.body.removeChild(a);
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
