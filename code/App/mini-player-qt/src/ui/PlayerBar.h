// PlayerBar：底部常驻控制条。
// 对应书中 §05 Step 5.8 的 ControlBar + Web §8.5 PlayerBar 的桌面对照实现。
#pragma once

#include <QFrame>

class QLabel;
class QPushButton;
class QSlider;
class QWidget;

namespace mp {

class PlayerService;
class CoverLabel;
class ProgressSlider;

class PlayerBar : public QFrame {
    Q_OBJECT
public:
    explicit PlayerBar(PlayerService* player, QWidget* parent = nullptr);

signals:
    void gotoPlayingPage();

private slots:
    void onCurrentChanged();
    void onStateChanged();
    void onPositionChanged(qint64 ms);
    void onDurationChanged(qint64 ms);
    void onVolumeChanged();
    void onMutedChanged();
    void onLoopModeChanged();

private:
    void refreshLoopIcon();

    PlayerService* m_player;

    CoverLabel*   m_cover;
    QLabel*       m_title;
    QLabel*       m_artist;
    QPushButton*  m_loopBtn;
    QPushButton*  m_prevBtn;
    QPushButton*  m_playBtn;
    QPushButton*  m_nextBtn;
    QPushButton*  m_muteBtn;
    QSlider*      m_volSlider;
    QLabel*       m_posLabel;
    QLabel*       m_durLabel;
    ProgressSlider* m_progress;
};

} // namespace mp
