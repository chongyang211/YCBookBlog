// MainActivity.java —— 列表 + PlayerBar + 权限流程 + SeekBar 双向绑定
// 对应文档 §02 / §03.3 / §04.4 / §05.3
package com.ycbook.miniplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.ViewModelProvider;
import androidx.recyclerview.widget.LinearLayoutManager;

import com.ycbook.miniplayer.data.MediaItem;
import com.ycbook.miniplayer.databinding.ActivityMainBinding;
import com.ycbook.miniplayer.service.PlayerState;
import com.ycbook.miniplayer.ui.MediaListAdapter;
import com.ycbook.miniplayer.ui.MainViewModel;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private static final int REQ_MEDIA = 1001;

    private ActivityMainBinding binding;
    private MainViewModel viewModel;
    private MediaListAdapter adapter;

    /** SeekBar 拖动标志位 —— 与 PlayerManager.userSeeking 双保险（文档 §04.4） */
    private boolean uiSeeking = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        viewModel = new ViewModelProvider(this).get(MainViewModel.class);

        setupList();
        setupPlayerBar();
        observeState();

        ensurePermissionThenScan();
    }

    // ====================================================================
    // 权限：三代分叉（文档 §03.3）
    // ====================================================================

    private String[] mediaReadPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {   // API 33+
            return new String[]{
                    Manifest.permission.READ_MEDIA_AUDIO,
                    Manifest.permission.READ_MEDIA_VIDEO,
            };
        }
        return new String[]{Manifest.permission.READ_EXTERNAL_STORAGE};
    }

    private boolean hasAllPermissions() {
        for (String p : mediaReadPermissions()) {
            if (ContextCompat.checkSelfPermission(this, p)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    private void ensurePermissionThenScan() {
        if (hasAllPermissions()) {
            viewModel.refreshLibrary();
        } else {
            requestPermissions(mediaReadPermissions(), REQ_MEDIA);
        }
    }

    @Override
    public void onRequestPermissionsResult(int code, @NonNull String[] perms,
                                           @NonNull int[] results) {
        super.onRequestPermissionsResult(code, perms, results);
        if (code == REQ_MEDIA) {
            if (hasAllPermissions()) {
                viewModel.refreshLibrary();
            } else {
                binding.tvEmpty.setVisibility(View.VISIBLE);
                binding.tvEmpty.setText(R.string.empty_hint);
            }
        }
    }

    // ====================================================================
    // 列表
    // ====================================================================

    private void setupList() {
        adapter = new MediaListAdapter((item, position) -> {
            // 当前列表快照作为播放队列（v0.1 简化；正式版应传稳定引用）
            List<MediaItem> snapshot =
                    new ArrayList<>(adapter.getCurrentList());
            viewModel.playWithResume(snapshot, position);   // ⭐ 断点续播接线
        });
        binding.rvMedia.setLayoutManager(new LinearLayoutManager(this));
        binding.rvMedia.setAdapter(adapter);

        viewModel.getLibrary().observe(this, items -> {
            adapter.submitList(items);
            boolean empty = items == null || items.isEmpty();
            binding.tvEmpty.setVisibility(empty ? View.VISIBLE : View.GONE);
            binding.rvMedia.setVisibility(empty ? View.GONE : View.VISIBLE);
        });

        // 冷启动 DB 缓存优先（秒开）—— LiveData 直接从 Room observe 即可，
        // v0.1 由 refreshLibrary 的入库触发首次投递
    }

    // ====================================================================
    // PlayerBar：SeekBar 双向绑定（五端同款坑 · 文档 §04.4）
    // ====================================================================

    private void setupPlayerBar() {
        binding.btnToggle.setOnClickListener(v -> viewModel.toggle());
        binding.btnNext.setOnClickListener(v -> viewModel.next());
        binding.btnPrev.setOnClickListener(v -> viewModel.previous());

        binding.seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar sb, int progress, boolean fromUser) {
                if (fromUser) {
                    binding.tvPosition.setText(
                            MediaListAdapter.formatMs(progress * 1000L));
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar sb) {
                uiSeeking = true;                        // ⭐ 冻结外部回写
                viewModel.player().setUserSeeking(true);
            }

            @Override
            public void onStopTrackingTouch(SeekBar sb) {
                viewModel.seekTo(sb.getProgress() * 1000L);
                uiSeeking = false;                       // ⭐ 释放，恢复自动前进
                viewModel.player().setUserSeeking(false);
            }
        });
    }

    // ====================================================================
    // 状态观察：UI 是 PlayerState 的镜像（铁律二）
    // ====================================================================

    private void observeState() {
        viewModel.getPlayerState().observe(this, s -> {
            // —— PlayerBar 可见性 ——
            binding.playerBar.setVisibility(s.hasMedia() ? View.VISIBLE : View.GONE);
            if (!s.hasMedia()) return;

            // —— 元信息 ——
            binding.tvNowTitle.setText(s.currentMedia.title);
            binding.tvNowArtist.setText(s.currentMedia.artist != null
                    ? s.currentMedia.artist : "未知艺术家");

            // —— 播放/暂停按钮 ——
            binding.btnToggle.setImageResource(
                    s.state == PlayerState.State.PLAYING
                            ? android.R.drawable.ic_media_pause
                            : android.R.drawable.ic_media_play);

            // —— 进度（只在非拖动时回写 —— 文档 §04.4 的修复版）——
            if (!uiSeeking) {
                binding.tvDuration.setText(MediaListAdapter.formatMs(s.durationMs));
                binding.tvPosition.setText(MediaListAdapter.formatMs(s.positionMs));
                binding.seekBar.setMax((int) Math.max(1, s.durationMs / 1000));
                binding.seekBar.setProgress((int) (s.positionMs / 1000));
            }
        });

        viewModel.getScanning().observe(this, scanning ->
                binding.tvHeader.setText(scanning
                        ? getString(R.string.app_name) + " · 扫描中…"
                        : getString(R.string.app_name)));
    }

    // ====================================================================
    // 生命周期
    // ====================================================================

    @Override
    protected void onStop() {
        super.onStop();
        // 兜底快照：哪怕被系统直接回收，断点也已落库（文档 §05.3 第三段）
        viewModel.snapshotPosition();
    }

    // 注：PlayerManager 在 ViewModel —— 旋转屏幕播放不中断（§04 的核心收益）
    // v0.1 限制：划掉进程播放即停。v0.2 迁入 PlaybackService 后解决（文档 §06）
}
