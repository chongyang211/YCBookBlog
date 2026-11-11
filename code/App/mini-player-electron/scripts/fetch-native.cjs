#!/usr/bin/env node
/*
 * scripts/fetch-native.cjs —— 为应用准备当前平台的 mp_native 产物
 *
 * 解析优先级：
 *   1. native/multi/<platform-arch>/<lib>   ← 多平台预编译产物（package 脚本产出）
 *   2. 回退：源码构建（scripts/build-native.cjs，产物直出 resources/native/）
 *
 * 用法：
 *   node scripts/fetch-native.cjs                  # 当前平台 → resources/native/
 *   node scripts/fetch-native.cjs --universal      # darwin: lipo 合并 arm64+x64 为 fat dylib
 *   node scripts/fetch-native.cjs --all            # 所有平台带入 resources/native/<tag>/
 *
 * 设计对应文档 §4.11：应用通过「标准产物接口」消费独立库工程，
 * 目录命名 native/multi/<platform-arch>/ 与 Electron 的
 * ${process.platform}-${process.arch} 一一对应。
 */
const { execFileSync } = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');

const ROOT = path.resolve(__dirname, '..');
// 独立库工程的 multi/ 产物目录：默认取同级 ../mp-native/multi，
// 可用环境变量 MP_NATIVE_MULTI_DIR 覆盖（库迁独立仓库时零改动）
const MULTI = process.env.MP_NATIVE_MULTI_DIR
  || path.resolve(ROOT, '..', 'mp-native', 'out', 'multi');
const DEST = path.join(ROOT, 'resources', 'native');

const argv = process.argv.slice(2);
const WANT_ALL = argv.includes('--all');
const WANT_UNIVERSAL = argv.includes('--universal');

// ---- 平台标识（与 native/multi/ 目录名一一对应）----
const TAG = `${process.platform}-${process.arch}`;
const LIBNAME =
  process.platform === 'win32' ? 'mp_native.dll'
  : process.platform === 'darwin' ? 'libmp_native.dylib'
  : 'libmp_native.so';

const isPlatformDir = (d) =>
  /^(darwin|linux|win32)-(arm64|x64|ia32)$/.test(d);

function multiDirs() {
  if (!fs.existsSync(MULTI)) return [];
  return fs.readdirSync(MULTI).filter(
    (d) => isPlatformDir(d) && fs.statSync(path.join(MULTI, d)).isDirectory()
  );
}

function copyLib(srcDir, destDir) {
  const src = path.join(srcDir, LIBNAME);
  if (!fs.existsSync(src)) return false;
  fs.mkdirSync(destDir, { recursive: true });
  fs.copyFileSync(src, path.join(destDir, LIBNAME));
  return true;
}

function buildFromSource() {
  console.log(`[fetch-native] multi/ 无 ${TAG} 产物 → 回退源码构建 ...`);
  const r = spawnSync(process.execPath, [path.join(__dirname, 'build-native.cjs')], {
    stdio: 'inherit',
  });
  if (r.status !== 0) {
    console.error(`[fetch-native] 源码构建失败（exit ${r.status}）`);
    process.exit(r.status ?? 1);
  }
}

fs.mkdirSync(DEST, { recursive: true });
const dirs = multiDirs();

// ============ --all：携带全部平台 ============
if (WANT_ALL) {
  if (dirs.length === 0) {
    console.error('[fetch-native] --all 需要 native 产物（先跑: node ../mp-native/scripts/build.cjs multi）');
    process.exit(1);
  }
  // 整目录照搬（含所有 OS 的库）：stub 库每个 ~50KB，全带上体积可忽略；
  // loader 运行时按 <platform-arch> 自选，electron-builder extraResources 会
  // 把它们全部带入安装包。
  for (const d of dirs) {
    fs.cpSync(path.join(MULTI, d), path.join(DEST, d), { recursive: true });
  }
  // 根目录同时放当前平台的，兼容 smoke-test 等直接加载路径
  copyLib(path.join(MULTI, TAG), DEST);
  console.log(`[fetch-native] --all done: ${dirs.join(', ')} → resources/native/<tag>/`);
  console.log(`[fetch-native] 运行时由 loader 按 ${TAG} 自动挑选`);
  process.exit(0);
}

// ============ --universal：darwin fat binary ============
if (WANT_UNIVERSAL) {
  if (process.platform !== 'darwin') {
    console.log('[fetch-native] --universal 仅 darwin 有意义，忽略');
  } else {
    const arm64 = path.join(MULTI, 'darwin-arm64', LIBNAME);
    const x64 = path.join(MULTI, 'darwin-x64', LIBNAME);
    const out = path.join(DEST, LIBNAME);
    if (fs.existsSync(arm64) && fs.existsSync(x64)) {
      fs.mkdirSync(DEST, { recursive: true });
      execFileSync('lipo', ['-create', arm64, x64, '-output', out]);
      console.log(`[fetch-native] universal dylib → ${out}`);
      console.log('  (arm64 + x86_64 合并，可配合 electron-builder mac universal target)');
      process.exit(0);
    }
    console.log('[fetch-native] multi/ 缺少 darwin 双架构产物，回退当前平台/源码构建');
  }
}

// ============ 默认：当前平台 ============
const multiSrc = path.join(MULTI, TAG, LIBNAME);
if (fs.existsSync(multiSrc)) {
  fs.mkdirSync(DEST, { recursive: true });
  fs.copyFileSync(multiSrc, path.join(DEST, LIBNAME));
  console.log(`[fetch-native] ✓ ${TAG} 产物（来自 native/multi/）→ resources/native/`);
} else {
  buildFromSource();
}
console.log(`[fetch-native] 就绪: ${path.join(DEST, LIBNAME)}`);
