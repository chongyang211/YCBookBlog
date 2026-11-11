// native/src/sysmedia/stub.cpp —— 系统媒体控制 Stub 实现
//
// 当 MP_WITH_SYSMEDIA=OFF 时编译本文件，提供空实现以保证符号表完整。
// 真正的 MPRIS / MPNowPlaying / SMTC 实现分别在同目录的三个平台文件里，
// 由 CMake 的 if (MP_WITH_SYSMEDIA) 分支择一编译。
//
// 这个空实现本身也是一个有价值的产品形态：
//   不想做系统媒体控制、只想做个"能跑"的音乐播放器时，这就是最终版本。

#include "mp_native.h"

extern "C" int mp_sysmedia_init(MpMediaCallback /*cb*/, void* /*user*/) {
  return 0;   // 什么也不做，但返回成功，让上层逻辑继续
}

extern "C" void mp_sysmedia_update(const MpNowPlaying* /*info*/) {
  // no-op
}

extern "C" void mp_sysmedia_shutdown(void) {
  // no-op
}
