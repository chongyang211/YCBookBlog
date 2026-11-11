// ui/MainViewModel.java —— LiveData<PlayerState> 唯一镜像（文档 §04.3）
package com.ycbook.miniplayer.ui;

import android.app.Application;

import androidx.annotation.NonNull;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.ycbook.miniplayer.data.MediaItem;
import com.ycbook.miniplayer.data.MediaRepository;
import com.ycbook.miniplayer.service.PlayerManager;
import com.ycbook.miniplayer.service.PlayerState;

import java.util.List;

/**
 * 状态中枢：
 *  - library / scanning：媒体库（Repository 喂）
 *  - playerState：播放状态的唯一数据源（PlayerManager 事件翻译层喂）
 *
 * PlayerManager 挂在 ViewModel → Activity 旋转重建不影响播放（§02 遗留问题的正解）。
 * v0.2 迁入 PlaybackService 后此类退化为"MediaController 代理"。
 */
public class MainViewModel extends AndroidViewModel {

    private final MutableLiveData<List<MediaItem>> library = new MutableLiveData<>();
    private final MutableLiveData<Boolean> scanning = new MutableLiveData<>(false);
    private final MutableLiveData<PlayerState> playerState =
            new MutableLiveData<>(PlayerState.idle());

    private final MediaRepository repository;
    private PlayerManager playerManager;

    public MainViewModel(@NonNull Application app) {
        super(app);
        repository = new MediaRepository(app);
    }

    // ======== 暴露给 Activity 观察 ========

    public LiveData<List<MediaItem>> getLibrary()  { return library; }
    public LiveData<Boolean> getScanning()         { return scanning; }
    public LiveData<PlayerState> getPlayerState()  { return playerState; }

    // ======== 媒体库 ========

    public void refreshLibrary() {
        scanning.setValue(true);
        repository.refreshLibrary(items -> {
            // Callback 在 Repository 的后台线程 → postValue
            library.postValue(items);
            scanning.postValue(false);
        });
    }

    // ======== 播放（惰性初始化 PlayerManager）========

    public PlayerManager player() {
        if (playerManager == null) {
            playerManager = new PlayerManager(getApplication(),
                    s -> playerState.postValue(s));     // ⭐ 事件翻译层 → LiveData
            // 断点续播上报：PlayerManager 每 5 秒 → Repository 落库
            playerManager.setReporter(repository::recordPlay);
        }
        return playerManager;
    }

    public void playQueue(List<MediaItem> items, int index) {
        player().setQueue(items, index);
    }

    public void playWithResume(List<MediaItem> items, int index) {
        MediaItem target = items.get(index);
        // 先查断点，再开播（文档 §05.3 的三段接线）
        repository.getLastPosition(target.id, resumeMs -> {
            player().setQueue(items, index);
            if (resumeMs != null && resumeMs > 5000
                    && resumeMs < target.durationMs - 10000) {
                // ⭐ 有效断点才恢复（贴头/贴尾不恢复）—— 五端统一判定
                player().seekTo(resumeMs);
            }
        });
    }

    public void toggle()      { player().toggle(); }
    public void next()        { player().next(); }
    public void previous()    { player().previous(); }
    public void seekTo(long ms) { player().seekTo(ms); }
    public void cycleSpeed()  { player().cycleSpeed(); }
    public void cycleMode()   { player().cycleMode(); }

    /** Activity onStop 兜底快照（存最后一次播放位置） */
    public void snapshotPosition() {
        if (playerManager != null) playerManager.snapshot();
    }

    @Override
    protected void onCleared() {
        if (playerManager != null) playerManager.release();
        repository.shutdown();
        super.onCleared();
    }
}
