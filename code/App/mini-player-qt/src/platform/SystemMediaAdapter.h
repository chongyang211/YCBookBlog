// SystemMediaAdapter：三端系统媒体控制的统一抽象。
// 对应书中 §09 阶段⑤⭐⭐ Step 9.1。
//
// Linux  → MprisAdapter (D-Bus MPRIS)       [待实现]
// macOS  → MPNowPlayingAdapter (Obj-C++)    [待实现]
// Windows→ SmtcAdapter (C++/WinRT)          [待实现]
// 当前全部走 StubAdapter（空实现），上层代码无需感知切换。
#pragma once

#include <QObject>
#include "types/MediaItem.h"

namespace mp {

class SystemMediaAdapter : public QObject {
    Q_OBJECT
public:
    static SystemMediaAdapter* create(QObject* parent = nullptr);

    virtual ~SystemMediaAdapter() = default;
    virtual void setMetadata(const MediaItem& item) = 0;
    virtual void setPlaybackState(PlaybackState s) = 0;
    virtual void setPosition(qint64 positionMs, qint64 durationMs) = 0;

signals:
    void playRequested();
    void pauseRequested();
    void nextRequested();
    void previousRequested();
    void seekRequested(qint64 positionMs);

protected:
    explicit SystemMediaAdapter(QObject* parent = nullptr) : QObject(parent) {}
};

} // namespace mp
