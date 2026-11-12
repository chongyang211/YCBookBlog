#!/usr/bin/env node
/*
 * scripts/normalize-permalinks.cjs —— 全站 permalink 规范化（方案 B）
 *
 * 规则：
 *   1. 修复重复：同一 permalink 被多个文件占用时，保留导航页（00.* / README），
 *      其余文件改配新的六位哈希
 *   2. 补齐缺失：无 permalink 的文件创建 front matter（必要时连 front matter 块）
 *   3. 语义 → 哈希：/pages/<语义>/ 且未被站内其他文件引用的，转为六位哈希；
 *      被引用的语义 permalink 保留（改正文链接的成本 > 收益）
 *   4. 生成 permalink-registry.json（全站占用名单，防未来撞车）
 *
 * 运行：node scripts/normalize-permalinks.cjs [--dry]
 */
const crypto = require('node:crypto');
const fs = require('node:fs');
const path = require('node:path');

const ROOT = path.resolve(__dirname, '..');
const DRY = process.argv.includes('--dry');
const HASH_RE = /^\/pages\/[0-9a-f]{6}\/$/;

/* ---------- 采集 md 文件（排除生成物） ---------- */
function listMd() {
  const out = [];
  const walk = (dir) => {
    for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
      if (e.name === 'node_modules' || e.name === '@pages' || e.name === '.vuepress') continue;
      const full = path.join(dir, e.name);
      if (e.isDirectory()) walk(full);
      else if (e.name.endsWith('.md')) out.push(full);
    }
  };
  walk(ROOT);
  return out;
}

/* ---------- 引用语料补充：.vuepress 配置（JS/TS/JSON/YML） ---------- */
function listCorpusExtra() {
  const out = [];
  const walk = (dir) => {
    if (!fs.existsSync(dir)) return;
    for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
      if (e.name === 'node_modules' || e.name === 'public') continue;
      const full = path.join(dir, e.name);
      if (e.isDirectory()) walk(full);
      else if (/\.(js|ts|json|yml|yaml)$/.test(e.name)) out.push(full);
    }
  };
  walk(path.join(ROOT, '.vuepress'));
  return out;
}

/* ---------- front matter 解析（只关心 permalink 与块边界） ---------- */
function parseFrontMatter(content) {
  if (!content.startsWith('---')) return { hasBlock: false, permalink: null };
  const end = content.indexOf('\n---', 3);
  if (end === -1) return { hasBlock: false, permalink: null };
  const m = content.slice(0, end).match(/^permalink:[ \t]*(\S+)/m);
  return { hasBlock: true, permalink: m ? m[1] : null };
}

/* ==================================================================== */
const mdFiles = listMd().sort();
const corpusFiles = [...mdFiles, ...listCorpusExtra()];
const corpusCache = new Map();                      // file -> content（引用扫描用）
const corpusOf = (f) => {
  if (!corpusCache.has(f)) corpusCache.set(f, fs.readFileSync(f, 'utf8'));
  return corpusCache.get(f);
};
const rel = (f) => path.relative(ROOT, f).split(path.sep).join('/');

/* 1. 解析全部 permalink */
const docs = mdFiles.map((f) => {
  const content = fs.readFileSync(f, 'utf8');
  const { hasBlock, permalink } = parseFrontMatter(content);
  return { file: f, rel: rel(f), content, hasBlock, permalink };
});

const used = new Set();
const byValue = new Map();
for (const d of docs) {
  if (!d.permalink) continue;
  used.add(d.permalink);
  if (!byValue.has(d.permalink)) byValue.set(d.permalink, []);
  byValue.get(d.permalink).push(d);
}

/* 2. 引用计数：字符串在全站语料出现的次数（排除自身文件） */
function refCount(permalink, selfFile) {
  let n = 0;
  for (const f of corpusFiles) {
    if (f === selfFile) continue;
    const c = corpusOf(f);
    let idx = 0;
    while ((idx = c.indexOf(permalink, idx)) !== -1) { n++; idx += permalink.length; }
  }
  return n;
}

/* 3. 哈希生成：md5(相对路径) 前 6 位；撞车加盐重试 */
function makeHash(seed) {
  let salt = 0;
  for (;;) {
    const h = crypto.createHash('md5')
      .update(salt === 0 ? seed : `${seed}#${salt}`)
      .digest('hex').slice(0, 6);
    if (!used.has(`/pages/${h}/`)) return h;
    salt++;
  }
}

/* ==================================================================== */
/* 变更集计算                                                            */
/* ==================================================================== */
const changes = [];
const keepSemantic = [];

/* 3a. 重复组：保留导航页（00.* / README），其余重哈希 */
for (const [, group] of byValue) {
  if (group.length < 2) continue;
  const nav = (d) => (/^(00\.|.*README\.md$)/.test(path.basename(d.file)) ? 0 : 1);
  const sorted = [...group].sort((a, b) => nav(a) - nav(b) || a.rel.localeCompare(b.rel));
  sorted.forEach((d, i) => {
    if (i === 0) return;                       // 首位（导航页）保留原值
    const h = makeHash(`dup:${d.rel}`);
    used.add(`/pages/${h}/`);
    changes.push({ doc: d, from: d.permalink, to: `/pages/${h}/`, reason: '重复冲突' });
  });
}

/* 3b. 缺失：补 permalink（无 front matter 则整块创建） */
for (const d of docs) {
  if (d.permalink) continue;
  const h = makeHash(`missing:${d.rel}`);
  used.add(`/pages/${h}/`);
  changes.push({
    doc: d, from: null, to: `/pages/${h}/`,
    reason: d.hasBlock ? '缺失（已有 front matter，插入 permalink 行）' : '缺失（无 front matter，整块创建）',
  });
}

/* 3c. 语义 → 哈希（未被站内引用才转；根路径结构页不在范围） */
const structural = [];
for (const [p, group] of byValue) {
  if (HASH_RE.test(p)) continue;
  if (!p.startsWith('/pages/')) {                        // /neigong/ 等根路径 = 结构页，保留
    for (const d of group) structural.push({ p, rel: d.rel });
    continue;
  }
  for (const d of group) {
    if (changes.some((c) => c.doc === d)) continue;      // 已被 3a/3b 处理
    const refs = refCount(p, d.file);
    if (refs > 0) { keepSemantic.push({ p, rel: d.rel, refs }); continue; }
    const h = makeHash(`sem:${d.rel}:${p}`);
    used.add(`/pages/${h}/`);
    changes.push({ doc: d, from: p, to: `/pages/${h}/`, reason: '语义 → 哈希（未被引用）' });
  }
}

/* ==================================================================== */
/* 应用变更（--dry 只打印）                                              */
/* ==================================================================== */
console.log(`扫描 ${docs.length} 篇 md；变更 ${changes.length} 项；保留被引用语义 ${keepSemantic.length} 个\n`);

for (const c of changes) {
  const { doc } = c;
  let content = doc.content;

  if (c.from) {
    /* 替换既有 permalink 行 */
    content = content.replace(
      new RegExp(`^(permalink:[ \\t]*)${c.from.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}$`, 'm'),
      `$1${c.to}`);
  } else if (doc.hasBlock) {
    /* 已有 front matter：在开头的 --- 后插入 permalink 行 */
    content = content.replace(/^---\n/, `---\npermalink: ${c.to}\n`);
  } else {
    /* 无 front matter：整块创建 */
    const title = path.basename(doc.file, '.md').replace(/^\d+\./, '');
    content = `---\ntitle: ${title}\ndate: ${new Date().toISOString().slice(0, 10)}\npermalink: ${c.to}\n---\n\n${content}`;
  }

  console.log(`  [${c.reason}] ${doc.rel}\n      ${c.from ?? '(无)'} → ${c.to}`);
  if (!DRY) fs.writeFileSync(doc.file, content);
  doc.content = content;
  doc.permalink = c.to;
}

/* ==================================================================== */
/* 4. 生成注册表（全站最终占用名单）                                      */
/* ==================================================================== */
if (!DRY) {
  const registry = {
    generatedAt: new Date().toISOString(),
    note: '全站 permalink 占用名单。新增语义 permalink 前请先查重（避免 /pages/perf/ 式撞车）。',
    stats: { total: docs.length, hash6: docs.filter((d) => HASH_RE.test(d.permalink || '')).length,
             semanticReserved: keepSemantic.length, structural: structural.length },
    structuralRootPaths: structural,
    semanticReserved: keepSemantic.map((k) => ({ permalink: k.p, file: k.rel, refs: k.refs })),
    permalinks: Object.fromEntries(docs.map((d) => [d.permalink, d.rel])),
  };
  const regPath = path.join(__dirname, 'permalink-registry.json');
  fs.writeFileSync(regPath, JSON.stringify(registry, null, 2));
  console.log(`\n✓ 注册表已生成: ${path.relative(ROOT, regPath)}`);
}

console.log(DRY ? '\n(dry run，未写入任何文件)' : '\n✓ 规范化完成');
