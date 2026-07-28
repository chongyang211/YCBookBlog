/**
 * sync-content.js
 * 把 packages/website/ 下所有 md 文件解析为 JSON 数据，供小程序 CloudBase 读取。
 *
 * 产物:
 *   packages/miniapp/static/catalog.json  — 轻量目录索引（随小程序打包）
 *   packages/miniapp/static/chapters/     — 每篇独立 JSON（上传到 CloudBase 云存储）
 *
 * 用法: node packages/miniapp/scripts/sync-content.js
 */

const fs = require('fs');
const path = require('path');

const DOCS_DIR = path.resolve(__dirname, '../../website');
const OUT_DIR = path.resolve(__dirname, '../static');
const CHAPTERS_DIR = path.join(OUT_DIR, 'chapters');

// 排除的文件名（README / 目录页等非正文）
const SKIP_FILES = ['README.md', '00.README.md', 'index.md'];

/**
 * 解析 YAML-like front-matter（两个 --- 之间的部分）
 * 返回 { meta, body }
 */
function parseFrontMatter(content) {
  const lines = content.split('\n');
  const meta = {};
  let bodyStart = 0;

  if (lines[0]?.trim() === '---') {
    let i = 1;
    let currentKey = null;
    let inList = false;

    for (; i < lines.length; i++) {
      const line = lines[i];
      if (line.trim() === '---') { bodyStart = i + 1; break; }

      // 列表项续行（以 "  -" 开头）
      if (inList && /^\s{2,}-/.test(line)) {
        meta[currentKey].push(line.trim().replace(/^\s*-\s*/, ''));
        continue;
      }

      const kv = /^(\w[\w-]*)\s*:\s*(.*)$/.exec(line);
      if (kv) {
        inList = false;
        const key = kv[1];
        let val = kv[2].trim();

        if (val === '') {
          // 多行数组的开头，例如 "categories:" 后跟 "  - xxx"
          if (['categories', 'tags'].includes(key)) {
            meta[key] = [];
            currentKey = key;
            inList = true;
          } else {
            // 对象类型字段，暂不展开处理
            currentKey = null;
          }
        } else {
          // 单行数组 [a, b] 格式
          if (/^\[.+\]$/.test(val)) {
            val = val.slice(1, -1).split(',').map(s => s.trim());
          }
          meta[key] = val;
          currentKey = null;
        }
      }
    }

    bodyStart = Math.max(bodyStart, 0);
  }

  // 正文部分，跳过首行空行
  let body = lines.slice(bodyStart).join('\n').trim();

  return { meta, body };
}

/**
 * 把 Markdown 正文转成纯文本摘要（取前三行有内容的行）
 */
function extractSummary(body, maxLen = 200) {
  const lines = body.split('\n');
  const meaningful = [];
  const noisePatterns = [
    /^[-\[]/,           // 以 - 或 [ 开头（目录行）
    /目录/,              // 含"目录"二字
    /^#/,               // 标题行（正文标题已在 front-matter 中）
    /^\d+\.\d*\s/,      // 数字列表项
    /`{3}/,             // 代码块标记
    /^\|/,              // 表格行
    /^-{3,}$/,          // 分割线
    /^>{1,2}\s*$/,      // 空引用
    /^©/,               // 版权声明
  ];

  for (const line of lines) {
    const original = line.trim();
    if (!original) continue;

    // 跳过噪声行
    if (noisePatterns.some(p => p.test(original))) continue;

    const t = original
      .replace(/^#+\s*/, '')
      .replace(/^>\s*/, '')
      .replace(/\*\*/g, '')
      .replace(/`/g, '')
      .replace(/\[([^\]]+)\]\([^)]+\)/g, '$1')
      .trim();

    if (t.length > 0) meaningful.push(t);
    if (meaningful.length >= 3) break;
  }
  if (meaningful.length === 0) return '';
  const joined = meaningful.join('。');
  return joined.length > maxLen ? joined.slice(0, maxLen) + '…' : joined;
}

/**
 * 从文件路径提取分类信息
 * 例如 packages/website/13.书籍/02.小人物的进修/01.xxx.md → ['书籍', '小人物的进修']
 */
function extractCategoryFromPath(filePath, baseDir) {
  const rel = path.relative(baseDir, filePath);
  const parts = rel.split(path.sep);
  // 去掉最后两段（数字目录 + 文件名）
  const cats = [];
  for (let i = 0; i < parts.length - 1; i++) {
    const p = parts[i];
    // 去掉数字前缀和特殊目录
    const clean = p.replace(/^\d+\.?\s*/, '');
    if (clean && !['@pages', '.vuepress', 'static'].includes(clean)) {
      cats.push(clean);
    }
  }
  return cats;
}

/**
 * 遍历所有 md 文件
 */
function walkMd(dir) {
  const results = [];
  const entries = fs.readdirSync(dir, { withFileTypes: true });

  for (const entry of entries) {
    const fullPath = path.join(dir, entry.name);
    if (entry.isDirectory()) {
      results.push(...walkMd(fullPath));
    } else if (entry.name.endsWith('.md') && !SKIP_FILES.includes(entry.name)) {
      results.push(fullPath);
    }
  }

  return results;
}

// ==================== 主流程 ====================

console.log('🔍 扫描 md 文件…');
const mdFiles = walkMd(DOCS_DIR);
console.log(`   共找到 ${mdFiles.length} 篇`);

// 生成唯一 ID
function generateId(filePath) {
  const rel = path.relative(DOCS_DIR, filePath);
  return rel.replace(/[\/\\]/g, '--').replace(/\.md$/, '');
}

const catalog = [];
fs.mkdirSync(CHAPTERS_DIR, { recursive: true });

let chapterIndex = 0;

for (const filePath of mdFiles) {
  const raw = fs.readFileSync(filePath, 'utf-8');
  const { meta, body } = parseFrontMatter(raw);
  const id = generateId(filePath);

  const chapter = {
    id,
    title: meta.title || path.basename(filePath, '.md'),
    date: meta.date || '',
    permalink: meta.permalink || '',
    categories: meta.categories || extractCategoryFromPath(filePath, DOCS_DIR),
    tags: Array.isArray(meta.tags) ? meta.tags.filter(Boolean) : [],
    author: meta.author || { name: '杨充', link: 'https://yccoding.com/' },
    summary: extractSummary(body),
    contentLength: body.length,
    // 正文不放在 catalog 里，独立存储
  };

  // 写入独立 JSON（上传到 CloudBase 云存储）
  const chapterFile = path.join(CHAPTERS_DIR, `${id}.json`);
  const chapterFull = { ...chapter, body };
  fs.writeFileSync(chapterFile, JSON.stringify(chapterFull, null, 2), 'utf-8');

  chapterIndex++;
  if (chapterIndex % 200 === 0) {
    console.log(`   已处理 ${chapterIndex}/${mdFiles.length}`);
  }
}

// 按分类分组后排序
const sorted = mdFiles.map(fp => {
  const rel = path.relative(DOCS_DIR, fp);
  return { path: rel, id: generateId(fp) };
});

// 生成轻量目录（不包含正文）
const catalogItems = mdFiles
  .map(fp => {
    const raw = fs.readFileSync(fp, 'utf-8');
    const { meta, body } = parseFrontMatter(raw);
    const id = generateId(fp);
    return {
      id,
      title: meta.title || path.basename(fp, '.md'),
      date: meta.date || '',
      permalink: meta.permalink || '',
      categories: meta.categories || extractCategoryFromPath(fp, DOCS_DIR),
      tags: Array.isArray(meta.tags) ? meta.tags.filter(Boolean) : [],
      author: meta.author || { name: '杨充', link: 'https://yccoding.com/' },
      summary: extractSummary(body),
    };
  })
  .sort((a, b) => (a.date > b.date ? -1 : 1));

fs.writeFileSync(
  path.join(OUT_DIR, 'catalog.json'),
  JSON.stringify(catalogItems, null, 2),
  'utf-8'
);

console.log(`\n✅ 完成！`);
console.log(`   目录索引: miniapp/static/catalog.json (${catalogItems.length} 条)`);
console.log(`   章节文件: miniapp/static/chapters/ (${catalogItems.length} 个)`);
console.log(`   总大小: ${(fs.statSync(path.join(OUT_DIR, 'catalog.json')).size / 1024).toFixed(1)} KB（目录）`);

// 统计 chapters 总大小
let chapterSize = 0;
const chapterFiles = fs.readdirSync(CHAPTERS_DIR);
for (const f of chapterFiles) {
  chapterSize += fs.statSync(path.join(CHAPTERS_DIR, f)).size;
}
console.log(`           ${(chapterSize / 1024 / 1024).toFixed(1)} MB（全部章节）`);
