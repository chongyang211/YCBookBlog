// native/src/sysmedia/nowplaying_mac.mm —— macOS MPNowPlaying 实现（占位）
//
// 构建开关：MP_WITH_SYSMEDIA=ON （且仅 APPLE 平台编译；文件扩展名 .mm → Obj-C++ 混编）
//
// TODO (对应文档 §6.1 完整实现路线)：
//   1. MPRemoteCommandCenter 注册 play/pause/nextTrack/previousTrack/changePlaybackPosition
//   2. MPNowPlayingInfoCenter.defaultCenter.nowPlayingInfo 实时填充
//   3. 处理 ControlCenter 的 remote command handler
//
// 当前实现：空壳子

#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#include "mp_native.h"

static MpMediaCallback g_cb = nullptr;
static void* g_user = nullptr;

extern "C" int mp_sysmedia_init(MpMediaCallback cb, void* user) {
  g_cb = cb;
  g_user = user;

  // TODO: 下方注释代码是真实实现，v0.1 刻意不启用
  // MPRemoteCommandCenter *cc = [MPRemoteCommandCenter sharedCommandCenter];
  // [cc.playCommand addTargetWithHandler:^(MPRemoteCommandEvent *e) {
  //   if (g_cb) g_cb(MP_SYSMEDIA_PLAY, g_user);
  //   return MPRemoteCommandHandlerStatusSuccess;
  // }];
  // [cc.pauseCommand addTargetWithHandler:^(MPRemoteCommandEvent *e) {
  //   if (g_cb) g_cb(MP_SYSMEDIA_PAUSE, g_user);
  //   return MPRemoteCommandHandlerStatusSuccess;
  // }];
  // // ... nextTrack / previousTrack / changePlaybackPosition
  return 0;
}

extern "C" void mp_sysmedia_update(const MpNowPlaying* /*info*/) {
  // TODO: MPNowPlayingInfoCenter 填充字典
}

extern "C" void mp_sysmedia_shutdown(void) {
  g_cb = nullptr;
  g_user = nullptr;
  // TODO: removeTarget
}
