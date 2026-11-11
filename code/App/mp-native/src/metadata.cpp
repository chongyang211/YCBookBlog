// native/src/metadata.cpp —— 元数据提取
//
// 两种构建模式：
// 1. MP_WITH_TAGLIB=OFF (默认)：返回 -3，用户在 JS 侧用文件名作为 fallback
// 2. MP_WITH_TAGLIB=ON：真实调用 TagLib 读 ID3/FLAC/Vorbis Comment
//
// 对应文档 §3.4：演示 FFI 边界"结构体内含 UTF-8 字符串字段 + 对应
// release 函数"的所有权约定。

#include "mp_native.h"
#include <cstdlib>
#include <cstring>

#ifdef MP_WITH_TAGLIB
  #include <taglib/fileref.h>
  #include <taglib/tag.h>
#endif

namespace {

// 把 C++ 字符串拷贝到 malloc 堆（之后由 mp_metadata_release 释放）
char* dup_utf8(const char* src, size_t len) {
  if (!src || len == 0) return nullptr;
  char* buf = static_cast<char*>(std::malloc(len + 1));
  if (!buf) return nullptr;
  std::memcpy(buf, src, len);
  buf[len] = '\0';
  return buf;
}

#ifdef MP_WITH_TAGLIB
char* dup_utf8(const TagLib::String& s) {
  if (s.isEmpty()) return nullptr;
  const auto u8 = s.to8Bit(/*unicode=*/true);
  return dup_utf8(u8.data(), u8.size());
}
#endif

}  // namespace

extern "C" int mp_metadata_read(const char* file_path, MpMetadata* out) {
  if (!file_path || !out) return -1;

  // 先清零，保证 release 时不会 free 到野指针
  std::memset(out, 0, sizeof(*out));

#ifdef MP_WITH_TAGLIB
  TagLib::FileRef f(file_path);
  if (f.isNull() || !f.tag()) return -2;

  auto* tag = f.tag();
  auto* ap  = f.audioProperties();

  out->title        = dup_utf8(tag->title());
  out->artist       = dup_utf8(tag->artist());
  out->album        = dup_utf8(tag->album());
  out->duration_ms  = ap ? ap->lengthInMilliseconds() : 0;
  out->bitrate_kbps = ap ? ap->bitrate() : 0;
  out->sample_rate  = ap ? ap->sampleRate() : 0;
  out->channels     = ap ? ap->channels() : 0;
  return 0;
#else
  // 未启用 TagLib —— 文档 §2.2 已提醒用户按平台装 taglib 后用
  //    cmake -B build -DMP_WITH_TAGLIB=ON
  // 重新构建。此处返回 -3 让 JS 侧走 fallback（取文件名作为标题）。
  (void)file_path;
  return -3;
#endif
}

extern "C" void mp_metadata_release(MpMetadata* m) {
  if (!m) return;
  // 字段是 const char*，但本库分配时是 malloc，释放时 const_cast 掉
  std::free(const_cast<char*>(m->title));
  std::free(const_cast<char*>(m->artist));
  std::free(const_cast<char*>(m->album));
  std::memset(m, 0, sizeof(*m));
}
