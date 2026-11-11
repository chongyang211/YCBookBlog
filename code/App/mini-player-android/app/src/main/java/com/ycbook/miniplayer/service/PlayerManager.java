// service/PlayerManager.java —— 本端心脏：ExoPlayer + 事件翻译 + 位置节流
// 对应文档 §04.2（事件向上翻译铁律三 / 200ms 节流 / 播放模式决策树）
package com.ycbook.miniplayer.service;

import android.content.Context;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;

import androidx.annotation.Nullable;
import androidx.media3.common.PlaybackException;
import androidx.media3.common.PlaybackParameters;
import androidx.media3.common.Player;
import androidx.media3.exoplayer.ExoPlayer;

import com.ycbook.miniplayer.data.MediaItem;

import java.util.ArrayList;
import java.util.List;

/**
 * 播放心脏类。v0.1 挂在 ViewModel（Activity 旋转不死）；
 * v0.2 将整体迁入 PlaybackService（文档 §06）—— 类代码零改动，只换宿主。
 */
public class PlayerManager implements Player.Listener {

    public interface StateChanged { void onChange(PlayerState s); }

    private final ExoPlayer player;
    private final StateChanged notifier;
    private final Handler handler = new Handler(Looper.getMainLooper());

    // —— 队列（架构篇 §1.5 setQueue 契约）——
    private final List<MediaItem> queue = new ArrayList<>();
    private int queueIndex = -1;
    private PlayerState.PlayMode mode = PlayerState.PlayMode.SEQUENCE;

    // —— seek 拖动标志位（五端同款坑 · 文档 §04.4）——
    private volatile boolean userSeeking = false;

    // —— 位置节流 + 播放上报（文档 §05.3：每 5 秒存断点）——
    private static final long TICK_MS = 200;
    private static final long REPORT_MS = 5000;
    private long lastReportWallMs = 0;
    private float lastVolume = 1f;

    public PlayerManager(Context ctx, StateChanged notifier) {
        this.notifier = notifier;
        player = new ExoPlayer.Builder(ctx.getApplicationContext()).build();
        player.addListener(this);
        startTick();
    }

    // ====================================================================
    // 队列契约
    // ====================================================================

    public void setQueue(List<MediaItem> items, int startIndex) {
        queue.clear();
        queue.addAll(items);
        playAt(startIndex);
    }

    public void playAt(int index) {
        if (index < 0 || index >= queue.size()) return;
        queueIndex = index;
        MediaItem m = queue.get(index);

        // ⭐ MediaStore content URI 直接喂 ExoPlayer（无需 File 路径）
        player.setMediaItem(androidx.media3.common.MediaItem.fromUri(Uri.parse(m.uri)));
        player.prepare();
        player.play();

        pushState();
    }

    // ====================================================================
    // 控制契约（架构篇 §1.5）
    // ====================================================================

    public void toggle() {
        if (queueIndex < 0) return;
        if (player.isPlaying()) player.pause();
        else player.play();
    }

    public void resume()  { if (queueIndex >= 0) player.play(); }
    public void pause()   { player.pause(); }

    public void seekTo(long ms) {
        player.seekTo(ms);
        pushState();          // 立即反馈，不等下一 tick
    }

    public void setSpeed(float rate) {
        player.setPlaybackParameters(new PlaybackParameters(rate));
        pushState();
    }

    /** 倍速五档循环：0.5 / 0.75 / 1.0 / 1.5 / 2.0（文档 §04.6） */
    public void cycleSpeed() {
        float[] speeds = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f};
        float cur = player.getPlaybackParameters().speed;
        for (int i = 0; i < speeds.length; i++) {
            if (speeds[i] == cur) { setSpeed(speeds[(i + 1) % speeds.length]); return; }
        }
        setSpeed(1.0f);
    }

    public void setVolume(float v) {
        lastVolume = Math.max(0f, Math.min(1f, v));
        player.setVolume(lastVolume);
        pushState();
    }

    public float getVolume() { return lastVolume; }

    public void setMuted(boolean m) {
        player.setVolume(m ? 0f : lastVolume);
        pushState();
    }

    public void setMode(PlayerState.PlayMode m) { mode = m; pushState(); }
    public void cycleMode() {
        mode = (mode == PlayerState.PlayMode.SEQUENCE) ? PlayerState.PlayMode.REPEAT_ONE
             : (mode == PlayerState.PlayMode.REPEAT_ONE) ? PlayerState.PlayMode.SHUFFLE
             : PlayerState.PlayMode.SEQUENCE;
        pushState();
    }

    public void next()     { playAt(nextIndex(+1)); }
    public void previous() { playAt(nextIndex(-1)); }

    /** ⭐ 播放模式决策树（与 Qt / Web / Electron 端逐字一致 · 文档 §04.5） */
    private int nextIndex(int dir) {
        if (queue.isEmpty()) return -1;
        if (mode == PlayerState.PlayMode.REPEAT_ONE) return queueIndex;
        if (mode == PlayerState.PlayMode.SHUFFLE) {
            if (queue.size() <= 1) return queueIndex;
            int r;
            do { r = (int) (Math.random() * queue.size()); } while (r == queueIndex);
            return r;
        }
        int n = queueIndex + dir;
        if (n >= queue.size()) n = 0;            // 顺序循环
        if (n < 0) n = queue.size() - 1;
        return n;
    }

    // ====================================================================
    // Player.Listener：引擎事件统一向上翻译（铁律三）
    // ====================================================================

    @Override
    public void onIsPlayingChanged(boolean isPlaying) {
        pushState();
    }

    @Override
    public void onPlaybackStateChanged(int playbackState) {
        if (playbackState == Player.STATE_ENDED) {
            next();                               // 播完自动下一首（受模式决策树控制）
        } else {
            pushState();
        }
    }

    @Override
    public void onPlayerError(PlaybackException error) {
        pushState();                              // 状态映射为 ERROR，UI 统一提示
    }

    // ====================================================================
    // 位置节流：Handler 200ms（对照架构篇 §1.4 表 · Qt 端 QTimer 同款）
    // ====================================================================

    private final Runnable tick = new Runnable() {
        @Override public void run() {
            if (!userSeeking) {
                pushState();

                // 每 5 秒上报断点（repository 由宿主注入，见 setReporter）
                long wall = SystemClock.elapsedRealtime();
                if (reporter != null && player.isPlaying()
                        && wall - lastReportWallMs >= REPORT_MS) {
                    reporter.accept(current().id, player.getCurrentPosition());
                    lastReportWallMs = wall;
                }
            }
            handler.postDelayed(this, TICK_MS);
        }
    };

    private void startTick() { handler.post(tick); }

    /** 断点上报注入（v0.1 由 ViewModel 连到 Repository） */
    public interface PositionReporter { void accept(String mediaId, long positionMs); }
    @Nullable private PositionReporter reporter;
    public void setReporter(PositionReporter r) { this.reporter = r; }

    // ====================================================================
    // 状态聚合与广播
    // ====================================================================

    @Nullable
    public MediaItem current() {
        return queueIndex >= 0 && queueIndex < queue.size() ? queue.get(queueIndex) : null;
    }

    public ExoPlayer exoPlayer() { return player; }   // §04.7 PlayerView 挂渲染用

    private void pushState() {
        MediaItem cur = current();
        notifier.onChange(new PlayerState(
                mapState(),
                cur,
                player.getCurrentPosition(),
                player.getDuration(),
                player.getPlaybackParameters().speed,
                player.getVolume(),
                player.getVolume() == 0f,
                mode,
                -1, -1));
    }

    private PlayerState.State mapState() {
        switch (player.getPlaybackState()) {
            case Player.STATE_BUFFERING: return PlayerState.State.PREPARING;
            case Player.STATE_READY:
                return player.isPlaying()
                        ? PlayerState.State.PLAYING : PlayerState.State.PAUSED;
            case Player.STATE_ENDED:     return PlayerState.State.PAUSED;
            case Player.STATE_IDLE:
            default:
                return player.getPlayerError() != null
                        ? PlayerState.State.ERROR : PlayerState.State.IDLE;
        }
    }

    // ====================================================================
    // 生命周期
    // ====================================================================

    public void setUserSeeking(boolean v) { userSeeking = v; }

    /** 兜底快照（宿主 onStop 时调用，存最后一次位置） */
    public void snapshot() {
        if (reporter != null && current() != null) {
            reporter.accept(current().id, player.getCurrentPosition());
        }
    }

    public void release() {
        handler.removeCallbacksAndMessages(null);
        player.release();
    }
}
