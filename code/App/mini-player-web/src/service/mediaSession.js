// Media Session 适配：锁屏/系统媒体控件对接。
// 对应书中 §06 阶段⑤：标签页后台 + Media Session。
//
// 效果验收：Windows 按音量键弹出的媒体浮窗、macOS 控制中心、Android Chrome 的锁屏通知
// 都能看到标题/封面并可以 play/pause/next/prev/seek。
import { watch, effectScope } from 'vue';
let scope = null;
export function bindMediaSession(player) {
    if (!('mediaSession' in navigator)) {
        console.info('[mediaSession] not supported');
        return;
    }
    unbindMediaSession();
    scope = effectScope();
    scope.run(() => {
        // 当前曲目变化 → 更新 metadata
        watch(() => player.current, (cur) => {
            if (!cur) {
                navigator.mediaSession.metadata = null;
                return;
            }
            const artwork = cur.coverUrl
                ? [{ src: cur.coverUrl, sizes: '512x512', type: cur.cover?.type ?? 'image/jpeg' }]
                : [];
            navigator.mediaSession.metadata = new MediaMetadata({
                title: cur.title,
                artist: cur.artist,
                album: cur.album,
                artwork,
            });
        }, { immediate: true });
        // 播放状态
        watch(() => player.status, (s) => {
            navigator.mediaSession.playbackState =
                s === 'playing' ? 'playing' : s === 'paused' ? 'paused' : 'none';
        });
        // 位置信息（让锁屏进度自己走）
        watch(() => [player.position, player.duration, player.playbackRate], ([pos, dur, rate]) => {
            if (!navigator.mediaSession.setPositionState)
                return;
            if (!dur || !isFinite(dur) || dur <= 0)
                return;
            try {
                navigator.mediaSession.setPositionState({
                    duration: dur / 1000,
                    position: Math.min(pos, dur) / 1000,
                    playbackRate: rate,
                });
            }
            catch { /* some browsers throw if position invalid, ignore */ }
        });
    });
    // 操作回调（store 外注册，一次性）
    const ms = navigator.mediaSession;
    ms.setActionHandler('play', () => player.play());
    ms.setActionHandler('pause', () => player.pause());
    ms.setActionHandler('previoustrack', () => player.previous());
    ms.setActionHandler('nexttrack', () => player.next());
    ms.setActionHandler('seekto', (e) => {
        if (typeof e.seekTime === 'number')
            player.seek(e.seekTime * 1000);
    });
    ms.setActionHandler('seekbackward', (e) => {
        player.seek(Math.max(0, player.position - (e.seekOffset ?? 10) * 1000));
    });
    ms.setActionHandler('seekforward', (e) => {
        player.seek(player.position + (e.seekOffset ?? 10) * 1000);
    });
    ms.setActionHandler('stop', () => player.pause());
}
export function unbindMediaSession() {
    if (!('mediaSession' in navigator))
        return;
    scope?.stop();
    scope = null;
    const actions = [
        'play', 'pause', 'previoustrack', 'nexttrack',
        'seekto', 'seekbackward', 'seekforward', 'stop',
    ];
    for (const a of actions) {
        try {
            navigator.mediaSession.setActionHandler(a, null);
        }
        catch { /* noop */ }
    }
}
