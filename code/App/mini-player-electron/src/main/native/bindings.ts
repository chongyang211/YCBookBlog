// src/main/native/bindings.ts —— koffi FFI 绑定层
//
// 对应文档 §4.2 FFI 边界设计。此文件是整个工程**最重要的 TS 文件**：
// 它把 mp_native.h 的 C ABI 翻译成 TypeScript 可调的函数。

import koffi from 'koffi';
import { ensureNativeLibExists } from './loader';

// 懒加载：仅在首次调用 getNative() 时真正 load dylib，避免模块导入期崩溃
let cached: NativeApi | null = null;

export interface NativeMetadata {
  title: string | null;
  artist: string | null;
  album: string | null;
  duration_ms: number;
  bitrate_kbps: number;
  sample_rate: number;
  channels: number;
}

export interface NativeApi {
  version(): string;
  probe(path: string): number;
  readMetadata(path: string): NativeMetadata | null;    // null 表示失败
  parseLrc(text: string): Array<{ timeMs: number; text: string }>;
}

export function getNative(): NativeApi {
  if (cached) return cached;

  const libPath = ensureNativeLibExists();
  const lib = koffi.load(libPath);

  // —— 结构体：MpMetadata（对应 mp_native.h）——
  // koffi 要求字段顺序与 C struct 完全一致
  const MpMetadata = koffi.struct('MpMetadata', {
    title:        'string',
    artist:       'string',
    album:        'string',
    duration_ms:  'int32',
    bitrate_kbps: 'int32',
    sample_rate:  'int32',
    channels:     'int32',
  });

  // —— 结构体：MpLrcLine ——
  const MpLrcLine = koffi.struct('MpLrcLine', {
    time_ms: 'int64',
    text:    'string',
  });

  // —— 函数绑定 ——
  const fn = {
    mp_version:         lib.func('mp_version', 'string', []),
    mp_probe:           lib.func('mp_probe',   'int',    ['string']),

    mp_metadata_read:    lib.func('mp_metadata_read',
                                   'int', ['string', koffi.out(koffi.pointer(MpMetadata))]),
    mp_metadata_release: lib.func('mp_metadata_release',
                                   'void', [koffi.pointer(MpMetadata)]),

    mp_lrc_parse:   lib.func('mp_lrc_parse',
                              'int', ['string', 'size_t', koffi.out(koffi.pointer(koffi.pointer(MpLrcLine)))]),
    mp_lrc_release: lib.func('mp_lrc_release', 'void', [koffi.pointer(MpLrcLine)]),
  };

  cached = {
    version: () => fn.mp_version(),

    probe: (path: string) => fn.mp_probe(path),

    readMetadata(path: string) {
      const out: any = {};
      const rc = fn.mp_metadata_read(path, out);
      if (rc !== 0) return null;
      // ⭐ 先浅拷贝一份出来，再释放原生字段
      const copy: NativeMetadata = {
        title:        out.title ?? null,
        artist:       out.artist ?? null,
        album:        out.album ?? null,
        duration_ms:  out.duration_ms,
        bitrate_kbps: out.bitrate_kbps,
        sample_rate:  out.sample_rate,
        channels:     out.channels,
      };
      fn.mp_metadata_release(out);
      return copy;
    },

    parseLrc(text: string) {
      const outPtr: any = {};
      const n = fn.mp_lrc_parse(text, Buffer.byteLength(text, 'utf8'), outPtr);
      if (n <= 0) return [];
      // outPtr 是 MpLrcLine** —— decode 为 n 个 MpLrcLine
      const linesPtr = outPtr.deref ? outPtr.deref() : outPtr;   // koffi 版本差异兼容
      const arr = koffi.decode(linesPtr, MpLrcLine, n) as Array<{ time_ms: bigint | number; text: string }>;
      const result = arr.map((row) => ({
        timeMs: typeof row.time_ms === 'bigint' ? Number(row.time_ms) : row.time_ms,
        text:   row.text ?? '',
      }));
      fn.mp_lrc_release(linesPtr);
      return result;
    },
  };

  return cached;
}
