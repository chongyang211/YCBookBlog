#!/usr/bin/env node
/**
 * gen-permalink.js — 生成 / 校验正文文章的 6 位 hex permalink
 *
 * 用法：
 *   node scripts/gen-permalink.js                # 生成一个未被占用的 hex
 *   node scripts/gen-permalink.js 84c3ee         # 检查 84c3ee 是否已被占用
 *   node scripts/gen-permalink.js -n 5           # 一次生成 5 个不重复的 hex
 */

const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

// 文档根目录（脚本位于 packages/website/scripts/）
const ROOT = path.resolve(__dirname, '..');

/** 收集全站已占用的 permalink */
function collectPermalinks() {
  const links = new Set();
  function walk(dir) {
    let entries;
    try {
      entries = fs.readdirSync(dir, { withFileTypes: true });
    } catch (e) {
      return;
    }
    for (const e of entries) {
      const p = path.join(dir, e.name);
      if (e.isDirectory()) {
        if (e.name.startsWith('.') || e.name === '@pages' || e.name === 'node_modules') continue;
        walk(p);
      } else if (e.name.endsWith('.md')) {
        const c = fs.readFileSync(p, 'utf8');
        const m = c.match(/^---\s*\n([\s\S]*?)\n---/);
        if (!m) continue;
        const pm = m[1].match(/^permalink:\s*(\S+)\s*$/m);
        if (pm) links.add(pm[1]);
      }
    }
  }
  walk(ROOT);
  return links;
}

const used = collectPermalinks();

function isUsed(hex) {
  return used.has(`/pages/${hex}/`);
}

/** 生成一个未被占用的 hex */
function genOne() {
  for (let i = 0; i < 1000; i++) {
    const hex = crypto.randomBytes(3).toString('hex');
    if (!isUsed(hex)) return hex;
  }
  return null;
}

const args = process.argv.slice(2);

// 批量生成模式：-n <count>
if (args[0] === '-n') {
  const count = Math.max(1, parseInt(args[1], 10) || 1);
  const results = [];
  for (let i = 0; i < count; i++) {
    const hex = genOne();
    if (!hex) {
      console.error('❌ 生成失败（尝试 1000 次都撞车），请重试');
      process.exit(1);
    }
    results.push(hex);
    used.add(`/pages/${hex}/`); // 批量时避免互相重复
  }
  console.log('✅ 生成的 permalink：');
  results.forEach(h => console.log(`permalink: /pages/${h}/`));
  process.exit(0);
}

// 校验模式：<hex>
if (args[0]) {
  const hex = args[0].toLowerCase();
  if (!/^[0-9a-f]{6}$/.test(hex)) {
    console.error('❌ 格式错误：请输入 6 位十六进制（0-9 a-f），如 84c3ee');
    console.error('   用法：node scripts/gen-permalink.js 84c3ee');
    process.exit(1);
  }
  if (isUsed(hex)) {
    console.log(`❌ ${hex} 已被占用：/pages/${hex}/`);
    process.exit(1);
  }
  console.log(`✅ ${hex} 可用：`);
  console.log(`permalink: /pages/${hex}/`);
  process.exit(0);
}

// 生成模式（无参数）：生成 1 个
const hex = genOne();
if (!hex) {
  console.error('❌ 生成失败，请重试');
  process.exit(1);
}
console.log('✅ 生成的 permalink：');
console.log(`permalink: /pages/${hex}/`);
