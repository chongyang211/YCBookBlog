// PlayerService：封装 QMediaPlayer + QAudioOutput，面向 UI 的"心脏"。
// 对应书中 §05 阶段③。
//
// 四端对照：
//   Android: PlayerService + ExoPlayer.Listener
//   iOS:     PlayerController + AVPlayer + KVO/periodicTimeObserver
//   Web:     Pinia store 持 <audio> + 事件翻译
//   Qt:      QObject + QMediaPlayer 信号槽（Qt 对象树自动管内存，不漏）
//
// 关键设计：
//   1. 单一 QMediaPlayer（音视频通吃），输出用 QAudioOutput + 可选 QVideoSink
//   2. 位置节流：QMediaPlayer 内置 1Hz 的 positionChanged，加 QTimer 200ms 平滑插值
//   3. seeking 标志位：进度条拖动期间不被引擎覆盖（四端同款坑）
//   4. 元数据 lazy：mediaStatusChanged == LoadedMedia 时 upsertMetadata 回库
#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVector>
#include <QTimer>

#include "types/MediaItem.h"

namespace mp {

class MediaRepository;

class PlayerService : public QObject {
    Q_OBJECT
    // Q_PROPERTY：UI 可直接订阅变化（本工程主要用信号槽；Q_PROPERTY 为将来 QML 预留）
    Q_PROPERTY(mp::PlaybackState state READ state NOTIFY stateChanged)
    Q_PROPERTY(qint64 positionMs READ positionMs NOTIFY positionChanged)
    Q_PROPERTY(qint64 durationMs READ durationMs NOTIFY durationChanged)
    Q_PROPERTY(bool muted READ isMuted NOTIFY mutedChanged)
    Q_PROPERTY(mp::LoopMode loopMode READ loopMode NOTIFY loopModeChanged)
public:
    explicit PlayerService(MediaRepository* repo, QObject* parent = nullptr);
    ~PlayerService() override;

    QMediaPlayer* mediaPlayer() const { return m_player; }   // 给 QVideoWidget 用

    // ---- getters ----
    PlaybackState state() const { return m_state; }
    bool isPlaying() const { return m_state == PlaybackState::Playing; }
    qint64 positionMs() const { return m_positionMs; }
    qint64 durationMs() const { return m_durationMs; }
    float  volume()   const { return m_volume; }
    bool   isMuted()  const { return m_muted; }
    LoopMode loopMode() const { return m_loopMode; }
    qreal  playbackRate() const { return m_rate; }
    MediaItem current() const { return m_current; }
    const QVector<MediaItem>& queue() const { return m_queue; }
    bool seeking() const { return m_seeking; }
    QString errorText() const { return m_errorText; }

public slots:
    // ---- 队列 ----
    void loadQueue(const QVector<mp::MediaItem>& items, int startIndex = 0, bool autoplay = true);
    void clearQueue();
    void removeFromQueue(const QString& id);

    // ---- 基础播放 ----
    void play();
    void pause();
    void toggle();
    void seek(qint64 ms);
    void next(bool auto_ = false);
    void previous();

    // ---- 控制项 ----
    void setVolume(float v);     // 0..1
    void setMuted(bool m);
    void toggleMute();
    void setLoopMode(LoopMode mode);
    void setPlaybackRate(qreal rate);

    // ---- 进度拖动标志位（§05 Step 5.4：四端同款坑） ----
    void setSeeking(bool v) { m_seeking = v; }

signals:
    void stateChanged(mp::PlaybackState s);
    void positionChanged(qint64 ms);
    void durationChanged(qint64 ms);
    void mutedChanged(bool m);
    void volumeChanged(float v);
    void loopModeChanged(mp::LoopMode m);
    void playbackRateChanged(qreal r);
    void currentChanged(const mp::MediaItem& it);
    void queueChanged();
    void errorOccurred(const QString& text);
    /** 元数据到位（LoadedMedia 时触发），UI 可据此刷新标题/时长 */
    void metadataResolved(const mp::MediaItem& it);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus st);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState st);
    void onErrorOccurred(QMediaPlayer::Error err, const QString& errorString);
    void onEnginePositionChanged(qint64 posMs);
    void onEngineDurationChanged(qint64 durMs);
    void onTick();   // 200ms 节流广播

private:
    void setState(PlaybackState s);
    void loadIntoEngine(const MediaItem& item, bool autoplay, qint64 startMs = 0);
    void applyVolume();
    void applyRate();
    void persistQueueAndPosition();

    MediaRepository* m_repo = nullptr;
    QMediaPlayer*    m_player = nullptr;
    QAudioOutput*    m_audio  = nullptr;
    QTimer*          m_ticker = nullptr;

    PlaybackState m_state = PlaybackState::Idle;
    qint64  m_positionMs = 0;
    qint64  m_durationMs = 0;
    float   m_volume = 0.8f;
    bool    m_muted = false;
    LoopMode m_loopMode = LoopMode::Order;
    qreal   m_rate = 1.0;
    bool    m_seeking = false;
    QString m_errorText;

    MediaItem m_current;
    QVector<MediaItem> m_queue;
    qint64 m_lastRecordedAtMs = 0;
};

} // namespace mp
