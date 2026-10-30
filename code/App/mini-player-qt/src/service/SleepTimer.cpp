#include "SleepTimer.h"
#include "PlayerService.h"

#include <QDateTime>

namespace mp {

SleepTimer::SleepTimer(QObject* parent) : QObject(parent), m_timer(this) {
    m_timer.setInterval(500);
    connect(&m_timer, &QTimer::timeout, this, &SleepTimer::onTick);
}

void SleepTimer::start(int minutes, PlayerService* player) {
    if (minutes <= 0 || !player) return;
    cancel();
    m_player        = player;
    m_savedVolume   = player->volume();
    m_deadlineMs    = QDateTime::currentMSecsSinceEpoch() + static_cast<qint64>(minutes) * 60'000;
    m_active        = true;
    m_fading        = false;
    m_timer.start();
    emit stateChanged(true, remainingMs());
}

void SleepTimer::cancel() {
    if (!m_active) return;
    m_timer.stop();
    if (m_player && m_fading) m_player->setVolume(m_savedVolume);
    m_active = false;
    m_fading = false;
    m_player = nullptr;
    emit stateChanged(false, 0);
}

qint64 SleepTimer::remainingMs() const {
    if (!m_active) return 0;
    return qMax<qint64>(0, m_deadlineMs - QDateTime::currentMSecsSinceEpoch());
}

void SleepTimer::onTick() {
    if (!m_active || !m_player) return;
    const qint64 remain = remainingMs();
    const qint64 kFadeMs = 10'000;

    if (remain <= 0) {
        m_player->pause();
        m_player->setVolume(m_savedVolume);     // 还原
        m_active = false;
        m_fading = false;
        m_timer.stop();
        emit stateChanged(false, 0);
        emit fired();
        return;
    }

    // 最后 10 秒线性淡出
    if (remain <= kFadeMs) {
        m_fading = true;
        const float ratio = static_cast<float>(remain) / static_cast<float>(kFadeMs);
        m_player->setVolume(m_savedVolume * ratio);
    }
    emit stateChanged(true, remain);
}

} // namespace mp
