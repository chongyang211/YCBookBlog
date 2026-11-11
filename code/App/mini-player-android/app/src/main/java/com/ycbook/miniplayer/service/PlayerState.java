// service/PlayerState.java —— 五端共享的状态九元组（对照架构篇 §1.4 / 文档 §04.1）
package com.ycbook.miniplayer.service;

import com.ycbook.miniplayer.data.MediaItem;

/**
 * 不可变状态对象 —— 每次变化 new 一个。
 * 不可变性保证"UI 只能读"的单向数据流（架构篇铁律二：状态只有一个数据源）。
 */
public class PlayerState {

    public enum State { IDLE, PREPARING, PLAYING, PAUSED, ERROR }
    public enum PlayMode { SEQUENCE, REPEAT_ONE, SHUFFLE }

    public final State state;
    public final MediaItem currentMedia;        // null = 队列空
    public final long positionMs;
    public final long durationMs;
    public final float speed;                   // 0.5 - 2.0
    public final float volume;                  // 0.0 - 1.0
    public final boolean muted;
    public final PlayMode playMode;
    public final long loopA;                    // A-B 循环，-1 = 未开启
    public final long loopB;

    public PlayerState(State state, MediaItem currentMedia, long positionMs,
                       long durationMs, float speed, float volume, boolean muted,
                       PlayMode playMode, long loopA, long loopB) {
        this.state = state;
        this.currentMedia = currentMedia;
        this.positionMs = positionMs;
        this.durationMs = durationMs;
        this.speed = speed;
        this.volume = volume;
        this.muted = muted;
        this.playMode = playMode;
        this.loopA = loopA;
        this.loopB = loopB;
    }

    public static PlayerState idle() {
        return new PlayerState(State.IDLE, null, 0, 0, 1f, 1f, false,
                PlayMode.SEQUENCE, -1, -1);
    }

    public boolean hasMedia() { return currentMedia != null; }
}
