// SettingsPage：主题 + 睡眠定时 + 媒体库维护。
// 对应书中 §08 阶段⑦ + §10 Step 10.3。
#pragma once

#include <QWidget>

class QSpinBox;
class QLabel;
class QPushButton;
class QTimer;

namespace mp {

class PlayerService;
class MediaRepository;
class SleepTimer;

class SettingsPage : public QWidget {
    Q_OBJECT
public:
    SettingsPage(PlayerService* player, MediaRepository* repo, QWidget* parent = nullptr);
    ~SettingsPage() override;

signals:
    void libraryCleared();

private slots:
    void onStartTimer();
    void onCancelTimer();
    void onClearLibrary();
    void refreshStats();

private:
    PlayerService*   m_player;
    MediaRepository* m_repo;
    SleepTimer*      m_timer;

    QLabel*     m_statLabel;
    QSpinBox*   m_minutes;
    QLabel*     m_remainLabel;
    QPushButton* m_startBtn;
    QPushButton* m_cancelBtn;
    QTimer*     m_uiTick;
};

} // namespace mp
