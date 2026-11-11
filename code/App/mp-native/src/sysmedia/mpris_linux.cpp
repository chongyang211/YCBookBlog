// native/src/sysmedia/mpris_linux.cpp —— Linux MPRIS D-Bus 实现（占位）
//
// 构建开关：MP_WITH_SYSMEDIA=ON （且本文件仅在 UNIX 非 APPLE 平台编译）
//
// TODO (对应文档 §6.1 完整实现路线)：
//   1. dbus_bus_get(DBUS_BUS_SESSION, ...)
//   2. 注册 org.mpris.MediaPlayer2.MiniPlayer bus name
//   3. 实现 org.mpris.MediaPlayer2 和 .Player 两个 interface
//   4. 在后台线程 while(dbus_connection_read_write_dispatch(bus, -1))
//   5. 收到 Play/Pause/Next/Previous 方法调用时，调 g_cb 回调
//
// 当前实现：空壳子，让 CMake 开 MP_WITH_SYSMEDIA=ON 时也能通过构建

#include "mp_native.h"
#include <dbus/dbus.h>   // 仅验证 dbus 头文件可被找到

static MpMediaCallback g_cb = nullptr;
static void* g_user = nullptr;

extern "C" int mp_sysmedia_init(MpMediaCallback cb, void* user) {
  g_cb = cb;
  g_user = user;
  // TODO: 创建 DBusConnection + 注册 MPRIS 对象路径
  return 0;
}

extern "C" void mp_sysmedia_update(const MpNowPlaying* /*info*/) {
  // TODO: emit PropertiesChanged signal
}

extern "C" void mp_sysmedia_shutdown(void) {
  g_cb = nullptr;
  g_user = nullptr;
  // TODO: dbus_connection_unref
}
