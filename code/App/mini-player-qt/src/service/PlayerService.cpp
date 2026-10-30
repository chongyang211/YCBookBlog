#include "PlayerService.h"
#include "data/MediaRepository.h"

#include <QUrl>
#include <QDateTime>
#include <QMediaMetaData>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QDebug>

namespace mp {

PlayerService::PlayerService(MediaRepository* repo, QObject* parent)
    : QObject(parent), m_repo(repo) {

    m_player = new QMediaPlayer(this);
    m_audio  = new QAudioOutput(this);
    m_player->setAudioOutput(m_audio);
    applyVolume();
    applyRate();

    // ---- 引擎事件翻译：Qt 6 新信号（§05 Step 5.1） ----
    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, &PlayerService::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, &PlayerService::onPlaybackStateChanged);
    connect(m_player, &QMediaPlayer::errorOccurred,
            this, &PlayerService::onErrorOccurred);
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &PlayerService::onEnginePositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged,
            this, &PlayerService::onEngineDurationChanged);

    // 200ms 节流广播（§05 Step 5.3）
    m_ticker = new QTimer(this);
    m_ticker->setInterval(200);
    connect(m_ticker, &QTimer::timeout, this, &PlayerService::onTick);

    // 恢复音量/循环模式
    if (m_repo) {
        bool ok = false;
        const float v = m_repo->getSetting(QStringLiteral("volume"), QStringLiteral("0.8")).toFloat(&ok);
        if (ok) m_volume = qBound(0.0f, v, 1.0f);
        const QString lm = m_repo->getSetting(QStringLiteral("loopMode"), QStringLiteral("order"));
        if      (lm == QStringLiteral("single")) m_loopMode = LoopMode::Single;
        else if (lm == QStringLiteral("random")) m_loopMode = LoopMode::Random;
        else                                     m_loopMode = LoopMode::Order;
        applyVolume();
    }
}

PlayerService::~PlayerService() = default;

/* ====================== 队列 / 装载 ====================== */

void PlayerService::loadQueue(const QVector<MediaItem>& items, int startIndex, bool autoplay) {
    m_queue = items;
    emit queueChanged();
    if (items.isEmpty()) return;
    startIndex = qBound(0, startIndex, items.size() - 1);
    const MediaItem& it = items[startIndex];
    m_current = it;
    emit currentChanged(m_current);
    m_positionMs = 0;
    emit positionChanged(0);
    loadIntoEngine(it, autoplay);
    persistQueueAndPosition();
}

void PlayerService::clearQueue() {
    m_player->stop();
    m_queue.clear();
    m_current = MediaItem();
    setState(PlaybackState::Idle);
    emit queueChanged();
    emit currentChanged(m_current);
}

void PlayerService::removeFromQueue(const QString& id) {
    const int idx = [&]{
        for (int i = 0; i < m_queue.size(); ++i)
            if (m_queue[i].id == id) return i;
        return -1;
    }();
    if (idx < 0) return;
    const bool isCurrent = (m_current.id == id);
    m_queue.remove(idx);
    emit queueChanged();
    if (isCurrent) {
        if (m_queue.isEmpty()) {
            clearQueue();
        } else {
            const int nextIdx = qMin(idx, m_queue.size() - 1);
            const MediaItem next = m_queue[nextIdx];
            m_current = next;
            emit currentChanged(m_current);
            loadIntoEngine(next, isPlaying());
        }
    }
}

void PlayerService::loadIntoEngine(const MediaItem& item, bool autoplay, qint64 startMs) {
    if (!QFileInfo::exists(item.filePath)) {
        m_errorText = tr("文件已不存在：%1").arg(item.filePath);
        emit errorOccurred(m_errorText);
        setState(PlaybackState::Error);
        return;
    }
    setState(PlaybackState::Loading);
    m_durationMs = item.durationMs;
    emit durationChanged(m_durationMs);

    m_player->setSource(QUrl::fromLocalFile(item.filePath));

    if (startMs > 0) {
        // seek 要等到 BufferedMedia 之后——用一次性 lambda 等待
        auto conn = std::make_shared<QMetaObject::Connection>();
        *conn = connect(m_player, &QMediaPlayer::mediaStatusChanged, this,
                        [this, conn, startMs](QMediaPlayer::MediaStatus st) {
            if (st == QMediaPlayer::LoadedMedia || st == QMediaPlayer::BufferedMedia) {
                m_player->setPosition(startMs);
                disconnect(*conn);
            }
        });
    }

    if (autoplay) m_player->play();
}

/* ====================== 基础控制 ====================== */

void PlayerService::play() {
    if (!m_current.isValid()) return;
    m_player->play();
}

void PlayerService::pause() { m_player->pause(); }

void PlayerService::toggle() { isPlaying() ? pause() : play(); }

void PlayerService::seek(qint64 ms) {
    if (m_durationMs <= 0) return;
    ms = qBound<qint64>(0, ms, m_durationMs);
    m_player->setPosition(ms);
    m_positionMs = ms;
    emit positionChanged(ms);
}

void PlayerService::next(bool auto_) {
    if (m_queue.isEmpty() || !m_current.isValid()) return;
    int idx = -1;
    for (int i = 0; i < m_queue.size(); ++i) if (m_queue[i].id == m_current.id) { idx = i; break; }
    if (idx < 0) return;

    int nextIdx = 0;
    if (auto_ && m_loopMode == LoopMode::Single) {
        nextIdx = idx;
    } else if (m_loopMode == LoopMode::Random) {
        if (m_queue.size() == 1) { nextIdx = 0; }
        else {
            do { nextIdx = QRandomGenerator::global()->bounded(m_queue.size()); }
            while (nextIdx == idx);
        }
    } else {
        nextIdx = (idx + 1) % m_queue.size();
    }
    const MediaItem tgt = m_queue[nextIdx];
    m_current = tgt;
    emit currentChanged(m_current);
    m_positionMs = 0;
    emit positionChanged(0);
    loadIntoEngine(tgt, true);
    persistQueueAndPosition();
}

void PlayerService::previous() {
    if (m_queue.isEmpty() || !m_current.isValid()) return;
    // > 3s 回到开头（音乐软件惯例，§05 Step 5.5）
    if (m_positionMs > 3000) { seek(0); return; }
    int idx = -1;
    for (int i = 0; i < m_queue.size(); ++i) if (m_queue[i].id == m_current.id) { idx = i; break; }
    if (idx < 0) return;
    const int prev = (idx - 1 + m_queue.size()) % m_queue.size();
    const MediaItem tgt = m_queue[prev];
    m_current = tgt;
    emit currentChanged(m_current);
    m_positionMs = 0;
    emit positionChanged(0);
    loadIntoEngine(tgt, true);
    persistQueueAndPosition();
}

/* ====================== 控制项 ====================== */

void PlayerService::setVolume(float v) {
    m_volume = qBound(0.0f, v, 1.0f);
    if (m_muted && v > 0.0f) {   // 拖动音量自动解除静音
        m_muted = false;
        emit mutedChanged(false);
    }
    applyVolume();
    if (m_repo) m_repo->setSetting(QStringLiteral("volume"), QString::number(m_volume));
    emit volumeChanged(m_volume);
}

void PlayerService::setMuted(bool m) {
    if (m_muted == m) return;
    m_muted = m;
    applyVolume();
    emit mutedChanged(m);
}

void PlayerService::toggleMute() { setMuted(!m_muted); }

void PlayerService::setLoopMode(LoopMode mode) {
    if (m_loopMode == mode) return;
    m_loopMode = mode;
    emit loopModeChanged(mode);
    if (m_repo) {
        const QString s = (mode == LoopMode::Single) ? QStringLiteral("single") :
                          (mode == LoopMode::Random) ? QStringLiteral("random") :
                                                       QStringLiteral("order");
        m_repo->setSetting(QStringLiteral("loopMode"), s);
    }
}

void PlayerService::setPlaybackRate(qreal rate) {
    m_rate = qBound(0.25, rate, 4.0);
    applyRate();
    emit playbackRateChanged(m_rate);
}

void PlayerService::applyVolume() {
    // QAudioOutput::setVolume 期望线性 0..1；人耳对线性非线性，这里保持简单
    m_audio->setVolume(m_muted ? 0.0f : m_volume);
    m_audio->setMuted(m_muted);
}

void PlayerService::applyRate() { m_player->setPlaybackRate(m_rate); }

/* ====================== 引擎事件翻译 ====================== */

void PlayerService::setState(PlaybackState s) {
    if (m_state == s) return;
    m_state = s;
    emit stateChanged(s);
    if (s == PlaybackState::Playing) {
        if (!m_ticker->isActive()) m_ticker->start();
    } else {
        m_ticker->stop();
    }
}

void PlayerService::onMediaStatusChanged(QMediaPlayer::MediaStatus st) {
    switch (st) {
        case QMediaPlayer::LoadingMedia:
            setState(PlaybackState::Loading);
            break;
        case QMediaPlayer::LoadedMedia:
        case QMediaPlayer::BufferedMedia: {
            // ---- 元数据到位：回填 Title/Artist/Album/Duration（§05 Step 5.1）----
            if (m_repo && m_current.isValid()) {
                const QMediaMetaData md = m_player->metaData();
                const QString title  = md.stringValue(QMediaMetaData::Title);
                const QString artist = md.stringValue(QMediaMetaData::ContributingArtist).isEmpty()
                                       ? md.stringValue(QMediaMetaData::AlbumArtist)
                                       : md.stringValue(QMediaMetaData::ContributingArtist);
                const QString album  = md.stringValue(QMediaMetaData::AlbumTitle);
                const qint64  dur    = m_player->duration();
                m_repo->upsertMetadata(m_current.id, title, artist, album, dur);
                // 更新内存快照（UI 可据此刷新）
                if (!title.isEmpty())  m_current.title  = title;
                if (!artist.isEmpty()) m_current.artist = artist;
                if (!album.isEmpty())  m_current.album  = album;
                m_current.durationMs = dur;
                emit metadataResolved(m_current);
            }
            break;
        }
        case QMediaPlayer::EndOfMedia:
            setState(PlaybackState::Ended);
            next(true);
            break;
        case QMediaPlayer::InvalidMedia:
            m_errorText = tr("无效的媒体（格式不支持或文件损坏）");
            emit errorOccurred(m_errorText);
            setState(PlaybackState::Error);
            break;
        default: break;
    }
}

void PlayerService::onPlaybackStateChanged(QMediaPlayer::PlaybackState st) {
    switch (st) {
        case QMediaPlayer::PlayingState: setState(PlaybackState::Playing); break;
        case QMediaPlayer::PausedState:  setState(PlaybackState::Paused);  break;
        case QMediaPlayer::StoppedState:
            if (m_state != PlaybackState::Error && m_state != PlaybackState::Ended)
                setState(PlaybackState::Paused);
            break;
    }
}

void PlayerService::onErrorOccurred(QMediaPlayer::Error, const QString& text) {
    m_errorText = text;
    emit errorOccurred(text);
    setState(PlaybackState::Error);
}

void PlayerService::onEnginePositionChanged(qint64 posMs) {
    if (m_seeking) return;          // 拖动时不被覆盖（§05 Step 5.4）
    m_positionMs = posMs;
    // 不在这里 emit —— 由 200ms 节流 tick 发射，防 UI 洪水
}

void PlayerService::onEngineDurationChanged(qint64 durMs) {
    if (durMs <= 0) return;
    m_durationMs = durMs;
    emit durationChanged(durMs);
}

void PlayerService::onTick() {
    if (!m_seeking) emit positionChanged(m_positionMs);
    // 每 5 秒记录一次断点
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (now - m_lastRecordedAtMs >= 5000 && m_current.isValid() && m_repo) {
        m_repo->recordPlay(m_current.id, m_positionMs);
        m_lastRecordedAtMs = now;
    }
}

void PlayerService::persistQueueAndPosition() {
    if (!m_repo) return;
    // 简化：仅记录 lastMediaId，队列恢复放挑战题
    m_repo->setSetting(QStringLiteral("lastMediaId"), m_current.id);
}

} // namespace mp
