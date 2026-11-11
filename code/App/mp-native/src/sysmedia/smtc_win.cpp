// native/src/sysmedia/smtc_win.cpp —— Windows SMTC 实现（占位）
//
// 构建开关：MP_WITH_SYSMEDIA=ON （且仅 _WIN32 平台编译）
//
// TODO (对应文档 §6.1 完整实现路线)：
//   1. winrt::init_apartment(multi_threaded)
//   2. 用 ISystemMediaTransportControlsInterop 从 Electron 主窗口 HWND 创建 SMTC
//   3. 挂 ButtonPressed 事件 → 调 g_cb
//   4. 用 SystemMediaTransportControlsDisplayUpdater 填充标题/封面
//
// 当前实现：空壳子，仅保证 WinRT 头文件可被解析

#include "mp_native.h"

#ifdef _WIN32
// WinRT 相关 include 保留，证明工具链配置完整
// #include <windows.media.h>
// #include <winrt/Windows.Media.h>
// #pragma comment(lib, "windowsapp")
#endif

static MpMediaCallback g_cb = nullptr;
static void* g_user = nullptr;

extern "C" int mp_sysmedia_init(MpMediaCallback cb, void* user) {
  g_cb = cb;
  g_user = user;
  // TODO: 创建 SMTC
  return 0;
}

extern "C" void mp_sysmedia_update(const MpNowPlaying* /*info*/) {
  // TODO: DisplayUpdater.Update()
}

extern "C" void mp_sysmedia_shutdown(void) {
  g_cb = nullptr;
  g_user = nullptr;
}
