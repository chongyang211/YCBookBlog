const KEY = 'jsfeed:cache:v1';

/**
 * 整体形状：{ [url]: { ts, data } }
 */
export function readAll() {
  try {
    return JSON.parse(localStorage.getItem(KEY) || '{}');
  } catch {
    return {};
  }
}

export function writeOne(url, data) {
  try {
    const all = readAll();
    all[url] = { ts: Date.now(), data };
    localStorage.setItem(KEY, JSON.stringify(all));
  } catch (err) {
    console.warn('[cache] write fail:', err.message);
  }
}

export function readOne(url, maxAge = Infinity) {
  const all = readAll();
  const entry = all[url];
  if (!entry) return null;
  if (Date.now() - entry.ts > maxAge) return null;
  return entry.data;
}
