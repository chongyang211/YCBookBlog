// src/main/native/loader.ts —— 按平台解析 mp_native 共享库路径
//
// 开发期：从工程根 resources/native/ 加载
// 打包后：Electron 把 extraResources 展开到 process.resourcesPath
//
// 对应文档 §4.1 平台路径解析

import path from 'node:path';
import fs from 'node:fs';
import { app } from 'electron';

export function nativeLibDir(): string {
  if (app.isPackaged) {
    // electron-builder 把 resources/native 展开到 process.resourcesPath/native
    return path.join(process.resourcesPath, 'native');
  }
  // 开发期：out/main/index.js → 工程根 resources/native
  // __dirname 大致是 .../out/main/
  return path.resolve(__dirname, '../../resources/native');
}

export function nativeLibName(): string {
  switch (process.platform) {
    case 'win32':  return 'mp_native.dll';
    case 'darwin': return 'libmp_native.dylib';
    default:       return 'libmp_native.so';   // linux / freebsd 等
  }
}

export function nativeLibPath(): string {
  return path.join(nativeLibDir(), nativeLibName());
}

export function ensureNativeLibExists(): string {
  const p = nativeLibPath();
  if (!fs.existsSync(p)) {
    throw new Error(
      `[mp_native] Shared library not found: ${p}\n` +
      `请先运行 'pnpm native:build' 构建 C++ 共享库。\n` +
      `详见 README.md §快速开始。`
    );
  }
  return p;
}
