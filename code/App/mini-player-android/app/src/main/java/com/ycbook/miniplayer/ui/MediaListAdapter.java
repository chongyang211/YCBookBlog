// ui/MediaListAdapter.java —— ListAdapter + DiffUtil（文档 §03.5）
package com.ycbook.miniplayer.ui;

import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import com.ycbook.miniplayer.data.MediaItem;
import com.ycbook.miniplayer.databinding.ItemMediaBinding;

import java.util.Locale;

/**
 * ListAdapter 内置后台 Diff —— submitList() 自动增量刷新，
 * 扫描完成提交 2000 条也不闪烁（对照文档 §03.5）。
 */
public class MediaListAdapter extends ListAdapter<MediaItem, MediaListAdapter.VH> {

    public interface OnItemClick { void onClick(MediaItem item, int position); }

    private final OnItemClick listener;

    public MediaListAdapter(OnItemClick listener) {
        super(new DiffUtil.ItemCallback<MediaItem>() {
            @Override
            public boolean areItemsTheSame(@NonNull MediaItem a, @NonNull MediaItem b) {
                return a.id.equals(b.id);              // ⭐ 稳定 ID 的价值
            }

            @Override
            public boolean areContentsTheSame(@NonNull MediaItem a, @NonNull MediaItem b) {
                return a.durationMs == b.durationMs
                        && eq(a.title, b.title)
                        && eq(a.artist, b.artist)
                        && eq(a.coverPath, b.coverPath);
            }

            private boolean eq(String x, String y) {
                return x == null ? y == null : x.equals(y);
            }
        });
        this.listener = listener;
    }

    @NonNull
    @Override
    public VH onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new VH(ItemMediaBinding.inflate(
                LayoutInflater.from(parent.getContext()), parent, false));
    }

    @Override
    public void onBindViewHolder(@NonNull VH h, int position) {
        MediaItem m = getItem(position);
        h.binding.tvTitle.setText(m.title);
        h.binding.tvArtist.setText(m.artist != null ? m.artist : "未知艺术家");
        h.binding.tvDuration.setText(formatMs(m.durationMs));

        // 封面：有缓存用缓存（降采样），无缓存用系统占位图标
        if (m.coverPath != null) {
            h.binding.ivCover.setImageBitmap(decodeSampled(m.coverPath, 96, 96));
        } else {
            h.binding.ivCover.setImageResource(android.R.drawable.ic_media_play);
        }

        h.itemView.setOnClickListener(v -> {
            if (listener != null) listener.onClick(m, h.getBindingAdapterPosition());
        });
    }

    /** 降采样解码：48dp 的格子不需要解码 2000x2000 的原图（性能防线） */
    private static android.graphics.Bitmap decodeSampled(String path, int w, int h) {
        final android.graphics.BitmapFactory.Options opts = new android.graphics.BitmapFactory.Options();
        opts.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(path, opts);
        opts.inSampleSize = Math.max(1, Math.min(opts.outWidth / w, opts.outHeight / h));
        opts.inJustDecodeBounds = false;
        return BitmapFactory.decodeFile(path, opts);
    }

    public static String formatMs(long ms) {
        if (ms <= 0) return "0:00";
        long s = ms / 1000;
        return String.format(Locale.US, "%d:%02d", s / 60, s % 60);
    }

    static class VH extends RecyclerView.ViewHolder {
        final ItemMediaBinding binding;
        VH(ItemMediaBinding b) { super(b.getRoot()); binding = b; }
    }
}
