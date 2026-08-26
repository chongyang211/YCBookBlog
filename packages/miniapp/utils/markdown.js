/**
 * markdown.js — 简易 Markdown → WXML rich-text 转换器
 *
 * 微信小程序的 rich-text 组件只支持 HTML 子集，
 * 因此这里把 md 正文转成 HTML 片段。
 */

/**
 * 将 Markdown 正文转为 HTML 字符串
 */
function mdToHtml(md) {
  if (!md) return '';

  const lines = md.split('\n');
  const html = [];
  let inCodeBlock = false;
  let codeLang = '';
  let codeLines = [];

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    // 代码块
    if (/^```/.test(line.trim())) {
      if (inCodeBlock) {
        // 结束代码块
        html.push(renderCodeBlock(codeLines.join('\n'), codeLang));
        codeLines = [];
        inCodeBlock = false;
      } else {
        // 开始代码块
        inCodeBlock = true;
        codeLang = line.trim().replace(/^```/, '').trim();
      }
      continue;
    }

    if (inCodeBlock) {
      codeLines.push(line);
      continue;
    }

    // 空行
    if (!line.trim()) continue;

    // 表格：连续以 | 开头的行
    if (line.trim().startsWith('|')) {
      const tableLines = [line];
      while (i + 1 < lines.length && lines[i + 1].trim().startsWith('|')) {
        tableLines.push(lines[++i]);
      }
      const tableHtml = renderTable(tableLines);
      html.push(tableHtml || `<p style="margin:8rpx 0;">${inlineMd(line.trim())}</p>`);
      continue;
    }

    // H1 - H3
    if (/^###\s/.test(line)) {
      html.push(`<h3 style="color:#6c5ce7;margin:32rpx 0 12rpx;font-size:34rpx;">${escapeHtml(line.replace(/^###\s*/, ''))}</h3>`);
      continue;
    }
    if (/^##\s/.test(line)) {
      html.push(`<h2 style="color:#ff6b6b;margin:36rpx 0 14rpx;font-size:36rpx;">${escapeHtml(line.replace(/^##\s*/, ''))}</h2>`);
      continue;
    }
    if (/^#\s/.test(line)) {
      html.push(`<h1 style="color:#fff;margin:40rpx 0 16rpx;font-size:40rpx;">${escapeHtml(line.replace(/^#\s*/, ''))}</h1>`);
      continue;
    }

    // 引用
    if (/^>\s/.test(line)) {
      html.push(`<blockquote style="border-left:4rpx solid #6c5ce7;padding:12rpx 24rpx;margin:16rpx 0;background:rgba(108,92,231,0.08);color:#a0a0b8;font-size:28rpx;">${inlineMd(line.replace(/^>\s*/, ''))}</blockquote>`);
      continue;
    }

    // 分割线
    if (/^---/.test(line.trim())) {
      html.push('<hr style="border:0;border-top:1rpx solid rgba(255,255,255,0.1);margin:32rpx 0;" />');
      continue;
    }

    // 无序列表 - 或 *
    if (/^[\-\*]\s/.test(line)) {
      html.push(`<p style="margin:8rpx 0;padding-left:16rpx;">• ${inlineMd(line.replace(/^[\-\*]\s*/, ''))}</p>`);
      continue;
    }

    // 数字列表 1. 2. 等
    if (/^\d+\.\s/.test(line)) {
      html.push(`<p style="margin:8rpx 0;">${inlineMd(line)}</p>`);
      continue;
    }

    // 普通段落
    html.push(`<p style="margin:8rpx 0;">${inlineMd(line)}</p>`);
  }

  // 未闭合的代码块
  if (inCodeBlock && codeLines.length > 0) {
    html.push(renderCodeBlock(codeLines.join('\n'), codeLang));
  }

  return html.join('\n');
}

/** 行内 Markdown 转换 */
function inlineMd(text) {
  let t = escapeHtml(text);
  // 加粗 **xxx**
  t = t.replace(/\*\*(.+?)\*\*/g, '<strong style="color:#fff;">$1</strong>');
  // 行内代码 `xxx`
  t = t.replace(/`([^`]+)`/g, '<code style="background:rgba(108,92,231,0.15);color:#6c5ce7;padding:2rpx 8rpx;border-radius:4rpx;font-size:26rpx;">$1</code>');
  // 链接 [text](url)
  t = t.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<span style="color:#00ced1;text-decoration:underline;">$1</span>');
  return t;
}

/** 代码块渲染 */
function renderCodeBlock(code, lang) {
  const escaped = escapeHtml(code);
  return `<pre style="background:rgba(0,0,0,0.3);border-radius:8rpx;padding:24rpx;margin:16rpx 0;overflow-x:scroll;font-size:24rpx;line-height:1.6;color:#a0a0b8;">${escaped}</pre>`;
}

/** 表格渲染（GitHub 风格 | a | b | 表格） */
function renderTable(lines) {
  if (lines.length < 2) return null;

  const rows = lines.map(parseTableRow);
  // 第二行必须是分隔行 |---|---|
  const sep = rows[1];
  if (!sep.every(c => /^:?-{1,}:?$/.test(c.trim()))) return null;

  const headerHtml = rows[0]
    .map(c => `<th style="color:#fff;font-weight:600;text-align:left;padding:12rpx 16rpx;border-bottom:1rpx solid rgba(255,255,255,0.15);">${inlineMd(c.trim())}</th>`)
    .join('');

  const bodyHtml = rows.slice(2)
    .map(row => `<tr>${row.map(c => `<td style="color:#d0d0d0;padding:12rpx 16rpx;border-bottom:1rpx solid rgba(255,255,255,0.06);">${inlineMd(c.trim())}</td>`).join('')}</tr>`)
    .join('');

  return `<table style="width:100%;border-collapse:collapse;margin:20rpx 0;font-size:26rpx;"><thead><tr>${headerHtml}</tr></thead><tbody>${bodyHtml}</tbody></table>`;
}

/** 解析表格行：把 "| a | b |" 拆成 ["a", "b"] */
function parseTableRow(line) {
  let l = line.trim();
  if (l.startsWith('|')) l = l.slice(1);
  if (l.endsWith('|')) l = l.slice(0, -1);
  return l.split('|');
}

/** HTML 转义 */
function escapeHtml(str) {
  return str
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

module.exports = { mdToHtml };
