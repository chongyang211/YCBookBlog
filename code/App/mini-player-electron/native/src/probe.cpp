// native/src/probe.cpp —— 文件头嗅探
//
// 对应文档 §00.README Q6："阶段②扫描按文件头探测而非仅扩展名"。
// 这是展示 C++ 典型用法（二进制 I/O + magic byte 匹配）的最小切片。

#include "mp_native.h"
#include <cstdio>
#include <cstring>
#include <cstdint>

namespace {

// 判断前缀是否匹配
bool starts_with(const uint8_t* buf, size_t n, const char* magic, size_t m) {
  if (n < m) return false;
  return std::memcmp(buf, magic, m) == 0;
}

MpFormat sniff(const uint8_t* buf, size_t n) {
  if (n < 4) return MP_FMT_UNKNOWN;

  // --- 音频 ---
  // MP3: "ID3" 或 0xFFFB/0xFFF3/0xFFF2 帧头
  if (starts_with(buf, n, "ID3", 3)) return MP_FMT_MP3;
  if (n >= 2 && buf[0] == 0xFF && (buf[1] & 0xE0) == 0xE0) return MP_FMT_MP3;

  // FLAC: "fLaC"
  if (starts_with(buf, n, "fLaC", 4)) return MP_FMT_FLAC;

  // OGG: "OggS"
  if (starts_with(buf, n, "OggS", 4)) return MP_FMT_OGG;

  // WAV: "RIFF" .. "WAVE"
  if (starts_with(buf, n, "RIFF", 4) && n >= 12
      && std::memcmp(buf + 8, "WAVE", 4) == 0) {
    return MP_FMT_WAV;
  }

  // --- 视频 / 容器 ---
  // MP4 / M4A / MOV: "....ftyp...."
  if (n >= 12 && std::memcmp(buf + 4, "ftyp", 4) == 0) {
    const char* brand = reinterpret_cast<const char*>(buf + 8);
    if (std::memcmp(brand, "M4A ", 4) == 0 ||
        std::memcmp(brand, "mp42", 4) == 0 ||
        std::memcmp(brand, "isom", 4) == 0 ||
        std::memcmp(brand, "mp41", 4) == 0 ||
        std::memcmp(brand, "dash", 4) == 0) {
      // M4A 是音频，mp4/isom/dash 一般是视频；qt 是 MOV
      if (std::memcmp(brand, "M4A ", 4) == 0) return MP_FMT_M4A;
      if (std::memcmp(brand, "qt  ", 4) == 0) return MP_FMT_MOV;
      return MP_FMT_MP4;
    }
    if (std::memcmp(brand, "qt  ", 4) == 0) return MP_FMT_MOV;
    return MP_FMT_MP4;
  }

  // MKV / WebM: EBML header 0x1A 0x45 0xDF 0xA3
  if (buf[0] == 0x1A && buf[1] == 0x45 && buf[2] == 0xDF && buf[3] == 0xA3) {
    // 简化处理：无法在只读 16 字节的情况下区分 MKV 与 WebM，统一返 MKV
    return MP_FMT_MKV;
  }

  return MP_FMT_UNKNOWN;
}

}  // namespace

extern "C" int mp_probe(const char* file_path) {
  if (!file_path) return MP_FMT_UNKNOWN;

  std::FILE* fp = std::fopen(file_path, "rb");
  if (!fp) return MP_FMT_UNKNOWN;

  uint8_t buf[16] = {0};
  const size_t n = std::fread(buf, 1, sizeof(buf), fp);
  std::fclose(fp);

  return static_cast<int>(sniff(buf, n));
}
