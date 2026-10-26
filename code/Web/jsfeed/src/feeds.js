const KEY = 'jsfeed:feeds:v1';

const DEFAULTS = [
  'https://www.ruanyifeng.com/blog/atom.xml',
  // 自行添加你喜欢的 feed
];

/** 读订阅源列表（首次为默认源） */
export function list() {
  try {
    const saved = JSON.parse(localStorage.getItem(KEY));
    return Array.isArray(saved) && saved.length ? saved : DEFAULTS.slice();
  } catch {
    return DEFAULTS.slice();
  }
}

export function add(url) {
  const all = list();
  if (!all.includes(url)) {
    all.push(url);
    save(all);
  }
  return all;
}

export function remove(url) {
  const all = list().filter((u) => u !== url);
  save(all);
  return all;
}

function save(all) {
  try {
    localStorage.setItem(KEY, JSON.stringify(all));
  } catch (err) {
    console.warn('[feeds] save fail:', err.message);
  }
}
