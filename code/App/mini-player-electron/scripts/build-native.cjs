#!/usr/bin/env node
/*
 * scripts/build-native.cjs —— 跨平台 CMake 构建脚本
 *
 * 用法：
 *   node scripts/build-native.cjs            # 构建
 *   node scripts/build-native.cjs --clean    # 清理
 *   node scripts/build-native.cjs --taglib   # 启用 TagLib（需系统已装）
 *   node scripts/build-native.cjs --sysmedia # 启用系统媒体控制
 *
 * 产物位置：resources/native/{libmp_native.so | libmp_native.dylib | mp_native.dll}
 */

const { spawnSync } = require('node:child_process');
const fs = require('node:fs');
const path = require('node:path');
const os = require('node:os');

const ROOT       = path.resolve(__dirname, '..');
// 独立库工程位置：默认取同级 ../mp-native，可用环境变量 MP_NATIVE_DIR 覆盖
// （库将来迁去独立仓库时，只需设置该变量，应用侧零改动）
const NATIVE_DIR = process.env.MP_NATIVE_DIR || path.resolve(ROOT, '..', 'mp-native');
const BUILD_DIR  = path.join(NATIVE_DIR, 'out', 'build');
const OUT_DIR    = path.join(ROOT, 'resources', 'native');

if (!fs.existsSync(path.join(NATIVE_DIR, 'CMakeLists.txt'))) {
  console.error(`[build-native] 未找到库工程: ${NATIVE_DIR}`);
  console.error(`  默认约定与 Electron 工程同级（code/App/mp-native），`);
  console.error(`  或设置环境变量 MP_NATIVE_DIR 指向 mp-native 工程根。`);
  process.exit(1);
}

const argv = process.argv.slice(2);
const WANT_CLEAN    = argv.includes('--clean');
const WANT_TAGLIB   = argv.includes('--taglib');
const WANT_SYSMEDIA = argv.includes('--sysmedia');

function run(cmd, args, opts = {}) {
  console.log(`\n$ ${cmd} ${args.join(' ')}`);
  const r = spawnSync(cmd, args, { stdio: 'inherit', cwd: NATIVE_DIR, ...opts });
  if (r.status !== 0) {
    console.error(`\n✗ Command failed with exit code ${r.status}`);
    process.exit(r.status ?? 1);
  }
}

function ensureDir(p) {
  fs.mkdirSync(p, { recursive: true });
}

if (WANT_CLEAN) {
  console.log('[clean] 删除 native/build 与 resources/native 产物');
  fs.rmSync(BUILD_DIR, { recursive: true, force: true });
  if (fs.existsSync(OUT_DIR)) {
    for (const f of fs.readdirSync(OUT_DIR)) {
      if (/\.(so|dylib|dll|lib|exp|pdb)$/.test(f)) {
        fs.rmSync(path.join(OUT_DIR, f));
      }
    }
  }
  console.log('✓ cleaned');
  process.exit(0);
}

ensureDir(OUT_DIR);
ensureDir(BUILD_DIR);

const isWin = process.platform === 'win32';

// --- Configure ---
const cmakeArgs = [
  '-B', BUILD_DIR, '-S', '.',
  // 嵌入模式：产物直出本工程 resources/native（路径由库工程的 CMakeLists 接收）
  `-DMP_EMBED_OUTPUT_DIR=${OUT_DIR}`,
];

if (WANT_TAGLIB)   cmakeArgs.push('-DMP_WITH_TAGLIB=ON');
if (WANT_SYSMEDIA) cmakeArgs.push('-DMP_WITH_SYSMEDIA=ON');

if (isWin) {
  // 允许通过环境变量注入 vcpkg
  const toolchain = process.env.VCPKG_ROOT
    ? path.join(process.env.VCPKG_ROOT, 'scripts', 'buildsystems', 'vcpkg.cmake')
    : null;
  if (toolchain && fs.existsSync(toolchain)) {
    cmakeArgs.push(`-DCMAKE_TOOLCHAIN_FILE=${toolchain}`);
  }
  cmakeArgs.push('-A', 'x64');
} else {
  cmakeArgs.push('-DCMAKE_BUILD_TYPE=Release');
}

run('cmake', cmakeArgs);

// --- Build ---
const buildArgs = ['--build', BUILD_DIR, '-j', String(os.cpus().length)];
if (isWin) buildArgs.push('--config', 'Release');
run('cmake', buildArgs);

// --- Verify output ---
const expected = isWin
  ? 'mp_native.dll'
  : process.platform === 'darwin'
    ? 'libmp_native.dylib'
    : 'libmp_native.so';

const outPath = path.join(OUT_DIR, expected);
if (!fs.existsSync(outPath)) {
  console.error(`\n✗ 预期产物未生成: ${outPath}`);
  console.error('  请检查 CMake 输出；若产物在其他位置可能需要调整 CMakeLists.txt 的 LIBRARY_OUTPUT_DIRECTORY');
  process.exit(1);
}

const stat = fs.statSync(outPath);
console.log(`\n✓ Native library built successfully`);
console.log(`  Path: ${outPath}`);
console.log(`  Size: ${(stat.size / 1024).toFixed(1)} KB`);
console.log(`\n下一步：pnpm smoke   # 加载并调用 mp_version() 验证`);
