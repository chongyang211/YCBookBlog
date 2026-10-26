import { ParseError } from './errors.js';

/** 入口：自动识别格式 */
export async function parse(url, text) {
  const head = text.trimStart().slice(0, 200);
  let module;
  // ⭐ 动态 import：只加载需要的解析器（按需加载插件）
  if (head.includes('<?xml') || head.includes('<rss')) {
    module = await import('./parsers/rss.js');
  } else if (head.includes('<feed')) {
    module = await import('./parsers/atom.js');
  } else if (head.startsWith('{')) {
    module = await import('./parsers/json.js');
  } else {
    throw new ParseError(url, new Error('unknown feed format'));
  }
  try {
    return module.parse(text);
  } catch (raw) {
    throw new ParseError(url, raw);
  }
}
