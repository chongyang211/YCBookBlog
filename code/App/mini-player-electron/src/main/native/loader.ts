// src/main/native/loader.ts —— 按平台解析 mp_native 共享库路径
//
// 查找优先级（两处命中任意一个即可）：
//   1. resources/native/<platform-arch>/<lib>   ← multi/ 多平台布局
//      （darwin-arm64 / darwin-x64 / linux-x64 / linux-arm64 / win32-x64 / win32-ia32）
//      Electron 的 process.platform-arch 命名与 native/multi/ 目录一一对应，
//      因此同一个安装包可以同时携带多平台产物，运行时按需挑选。
//   2. resources/native/<lib>                   ← 嵌入源码构建布局（单平台）
//
// 开发期：从工程根 resources/native/ 加载
// 打包后：electron-builder 把 resources/native 展开到 process.resourcesPath
//
// 对应文档 §4.1 平台路径解析 + §4.11 独立库工程

import path from 'node:path';
import fs from 'node:fs';
import { app } from 'electron';

export function nativeLibDir(): string {
  if (app.isPackaged) {
    // electron-builder 把 resources/native 展开到 process.resourcesPath/native
    return path.join(process.resourcesPath, 'native');
  }
  // 开发期：out/main/index.js → 工程根 resources/native
  return path.resolve(__dirname, '../../resources/native');
}

export function nativeLibName(): string {
  switch (process.platform) {
    case 'win32':  return 'mp_native.dll';
    case 'darwin': return 'libmp_native.dylib';
    default:       return 'libmp_native.so';   // linux / freebsd 等
  }
}

/** 当前运行平台的 multi/ 目录标记，如 darwin-arm64 / win32-x64 */
export function platformTag(): string {
  return `${process.platform}-${process.arch}`;
}

/** 按优先级排列的候选路径 */
export function nativeLibCandidates(): string[] {
  const base = nativeLibDir();
  return [
    path.join(base, platformTag(), nativeLibName()),   // ① multi/ 布局
    path.join(base, nativeLibName()),                  // ② 嵌入构建布局
  ];
}

export function resolveNativeLibPath(): string {
  const candidates = nativeLibCandidates();
  for (const p of candidates) {
    if (fs.existsSync(p)) return p;
  }
  return candidates[candidates.length - 1];   // 交给 ensure 报错（信息里列出全部候选）
}

export function ensureNativeLibExists(): string {
  const candidates = nativeLibCandidates();
  for (const p of candidates) {
    if (fs.existsSync(p)) return p;
  }
  throw new Error(
    `[mp_native] Shared library not found. 查找过:\n` +
    candidates.map((c) => `  - ${c}`).join('\n') +
    `\n请先运行 'pnpm prepare:native'（优先取 native/multi/ 产物，缺失时回退源码构建）。`
  );
}
