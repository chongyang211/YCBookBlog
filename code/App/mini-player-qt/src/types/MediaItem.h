// 数据模型 · MediaItem：音频/视频统一建模。
// 对应架构篇 §1.3 的 MediaItem，Qt 端用 struct + Q_DECLARE_METATYPE 跨信号槽传递。
//
// 四端对照：
//   Android: class MediaItem + Room @Entity
//   iOS:     MediaItem : NSObject
//   Web:     interface MediaItem
//   Qt:      struct MediaItem（值语义，std::vector 友好，SQL 建表字段一一对应）
#pragma once

#include <QString>
#include <QMetaType>
#include <QDateTime>

namespace mp {

enum class MediaType {
    Audio,
    Video,
};

inline const char* toString(MediaType t) {
    return t == MediaType::Video ? "video" : "audio";
}
inline MediaType mediaTypeFromString(const QString& s) {
    return s == QStringLiteral("video") ? MediaType::Video : MediaType::Audio;
}

struct MediaItem {
    /** 稳定 ID：path hash（path+size+mtime 组合），保证同一文件重复导入 ID 一致 */
    QString id;
    MediaType type = MediaType::Audio;
    QString filePath;              // 绝对路径
    QString title;                 // 元数据标题，缺省用文件名
    QString artist;
    QString album;
    qint64  durationMs = 0;        // 时长，毫秒
    qint64  sizeBytes  = 0;        // 文件大小
    qint64  addedAt    = 0;        // 入库时间（Unix 秒）
    QString coverPath;             // 封面缓存路径，可空
    bool    exists     = true;     // 运行时字段：文件是否仍存在（不入库）

    bool isValid() const { return !id.isEmpty() && !filePath.isEmpty(); }
};

/** 播放状态机：对照架构篇 §1.4 */
enum class PlaybackState {
    Idle,
    Loading,
    Playing,
    Paused,
    Ended,
    Error,
};

/** 循环模式 */
enum class LoopMode {
    Order,    // 顺序
    Single,   // 单曲循环
    Random,   // 随机
};

} // namespace mp

// 让 MediaItem 可跨线程/信号槽/QVariant 传递
Q_DECLARE_METATYPE(mp::MediaItem)
Q_DECLARE_METATYPE(mp::PlaybackState)
Q_DECLARE_METATYPE(mp::LoopMode)
