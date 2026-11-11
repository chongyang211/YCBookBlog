// native/src/lrc.cpp —— LRC 歌词解析
//
// 教学目的（对应文档 §7）：演示 C ABI 返回"变长数组 + 内部变长字符串"
// 的所有权约定。整个结果使用**单块堆内存**布局：
//
//   [MpLrcLine × N][文本池：text1\0 text2\0 ...]
//    ↑指针           ↑每行 text 字段指向这里的某个偏移
//
// 释放：mp_lrc_release 只需 free 这一块即可，不需要逐行 free。
// 这避免了 FFI 边界"N 行就要 N 次跨语言 free 调用"的开销。

#include "mp_native.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// 解析 "[mm:ss.xx]" 或 "[mm:ss]"；写入 ms；返回标签文本结束位置（不含 ']'）
// 若不是时间标签返回 nullptr
const char* parse_timestamp(const char* p, const char* end, int64_t* ms) {
  // 期望形如 [12:34.56]
  if (p >= end || *p != '[') return nullptr;
  const char* q = p + 1;

  // mm
  int mm = 0;
  while (q < end && *q >= '0' && *q <= '9') { mm = mm * 10 + (*q - '0'); ++q; }
  if (q >= end || *q != ':') return nullptr;
  ++q;
  // ss
  int ss = 0;
  while (q < end && *q >= '0' && *q <= '9') { ss = ss * 10 + (*q - '0'); ++q; }
  int hundredths = 0;
  if (q < end && (*q == '.' || *q == ':')) {
    ++q;
    int digits = 0;
    while (q < end && *q >= '0' && *q <= '9' && digits < 3) {
      hundredths = hundredths * 10 + (*q - '0');
      ++q; ++digits;
    }
    // 补齐到 3 位（毫秒）
    while (digits < 3) { hundredths *= 10; ++digits; }
  }
  if (q >= end || *q != ']') return nullptr;

  *ms = static_cast<int64_t>(mm) * 60000 + static_cast<int64_t>(ss) * 1000
      + hundredths;
  return q + 1;   // 跳过 ']'
}

}  // namespace

extern "C" int mp_lrc_parse(const char* lrc_utf8, size_t len,
                            MpLrcLine** out_lines) {
  if (!lrc_utf8 || !out_lines) return 0;
  *out_lines = nullptr;

  // 临时存储：时间 + 文本
  struct Tmp { int64_t ms; std::string text; };
  std::vector<Tmp> rows;

  const char* p = lrc_utf8;
  const char* end = p + len;

  while (p < end) {
    // 跳过行首空白
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p;
    if (p >= end) break;

    // 一行可能有多个 [mm:ss.xx] 时间标签（LRC 规范允许）
    std::vector<int64_t> ts;
    while (p < end && *p == '[') {
      int64_t ms = 0;
      const char* next = parse_timestamp(p, end, &ms);
      if (next) {
        ts.push_back(ms);
        p = next;
      } else {
        // 可能是 [ar:...] 这种元信息标签，跳到行尾
        while (p < end && *p != '\n') ++p;
        break;
      }
    }

    // 读取文本直到换行
    const char* text_begin = p;
    while (p < end && *p != '\n' && *p != '\r') ++p;
    const char* text_end = p;
    // 跳过换行
    while (p < end && (*p == '\n' || *p == '\r')) ++p;

    if (ts.empty()) continue;
    std::string text(text_begin, text_end - text_begin);
    for (int64_t ms : ts) rows.push_back({ms, text});
  }

  if (rows.empty()) return 0;

  // 按时间排序
  std::sort(rows.begin(), rows.end(),
            [](const Tmp& a, const Tmp& b) { return a.ms < b.ms; });

  // 计算单块布局大小
  const size_t n = rows.size();
  const size_t header = n * sizeof(MpLrcLine);
  size_t pool = 0;
  for (auto& r : rows) pool += r.text.size() + 1;   // +1 for '\0'

  uint8_t* block = static_cast<uint8_t*>(std::malloc(header + pool));
  if (!block) return 0;

  MpLrcLine* lines = reinterpret_cast<MpLrcLine*>(block);
  char* text_ptr = reinterpret_cast<char*>(block + header);

  for (size_t i = 0; i < n; ++i) {
    lines[i].time_ms = rows[i].ms;
    lines[i].text    = text_ptr;
    std::memcpy(text_ptr, rows[i].text.data(), rows[i].text.size());
    text_ptr[rows[i].text.size()] = '\0';
    text_ptr += rows[i].text.size() + 1;
  }

  *out_lines = lines;
  return static_cast<int>(n);
}

extern "C" void mp_lrc_release(MpLrcLine* lines) {
  // 整块释放，不要逐行 free（text 指针都在同一块里）
  if (lines) std::free(lines);
}
