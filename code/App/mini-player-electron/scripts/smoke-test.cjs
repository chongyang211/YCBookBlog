#!/usr/bin/env node
/*
 * scripts/smoke-test.cjs —— 不启动 Electron，直接 node 加载 mp_native，验证 FFI 通路
 *
 * 运行：node scripts/smoke-test.cjs [可选: 要探测的文件路径]
 */

const koffi = require('koffi');
const path = require('node:path');
const fs = require('node:fs');

const platform = process.platform;
const libName = platform === 'win32'  ? 'mp_native.dll'
              : platform === 'darwin' ? 'libmp_native.dylib'
                                      : 'libmp_native.so';
const libPath = path.resolve(__dirname, '..', 'resources', 'native', libName);

if (!fs.existsSync(libPath)) {
  console.error(`✗ Shared library not found: ${libPath}`);
  console.error(`  请先运行: pnpm native:build`);
  process.exit(1);
}

console.log(`→ loading: ${libPath}`);
const lib = koffi.load(libPath);

const mp_version = lib.func('mp_version', 'string', []);
const mp_probe   = lib.func('mp_probe',   'int',    ['string']);

const FMT_NAMES = ['unknown','mp3','flac','m4a','wav','ogg','mp4','mkv','mov','webm'];

console.log(`✓ loaded`);
console.log(`  version: ${mp_version()}`);

const target = process.argv[2];
if (target && fs.existsSync(target)) {
  const code = mp_probe(target);
  console.log(`  probe('${target}') = ${code} (${FMT_NAMES[code] ?? '?'})`);
} else {
  console.log(`\n提示：传入文件路径可测试 mp_probe，例如：`);
  console.log(`  node scripts/smoke-test.cjs ~/Music/test.mp3`);
}
