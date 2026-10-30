// SleepTimer：睡眠定时 —— N 分钟后暂停，最后 10 秒 QPropertyAnimation 淡出音量。
// 对应书中 §10 阶段⑧ Step 10.3。
#pragma once

#include <QObject>
#include <QTimer>

namespace mp {

class PlayerService;

class SleepTimer : public QObject {
    Q_OBJECT
public:
    explicit SleepTimer(QObject* parent = nullptr);

    void start(int minutes, PlayerService* player);
    void cancel();

    bool isActive() const { return m_active; }
    qint64 remainingMs() const;

signals:
    void stateChanged(bool active, qint64 remainingMs);
    void fired();

private slots:
    void onTick();

private:
    bool  m_active = false;
    qint64 m_deadlineMs = 0;
    PlayerService* m_player = nullptr;
    QTimer m_timer;
    float m_savedVolume = 0.8f;    // 为淡出用；fired 后恢复
    bool  m_fading = false;
};

} // namespace mp
