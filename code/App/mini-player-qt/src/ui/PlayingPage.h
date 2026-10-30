// PlayingPage：正在播放页。左封面大图（旋转）+ 右歌词 + 底部大进度条。
// 对应书中 §05 Step 5.8 的播放页 + §07 歌词。
#pragma once

#include <QWidget>

class QLabel;
class QPushButton;
class QStackedWidget;
class QVideoWidget;

namespace mp {

class PlayerService;
class CoverLabel;
class LyricsWidget;

class PlayingPage : public QWidget {
    Q_OBJECT
public:
    explicit PlayingPage(PlayerService* player, QWidget* parent = nullptr);

private slots:
    void onCurrentChanged();
    void onStateChanged();
    void onPositionChanged(qint64 ms);
    void onLoadLrc();
    void onToggleFullscreen();

private:
    PlayerService*  m_player;
    QStackedWidget* m_stack;      // 0 = cover+lyrics ; 1 = video
    QVideoWidget*   m_video;
    CoverLabel*     m_cover;
    LyricsWidget*   m_lyrics;
    QLabel*         m_titleLbl;
    QLabel*         m_subLbl;
    QLabel*         m_posLbl;
    QLabel*         m_durLbl;
    QPushButton*    m_loadLrcBtn;
    QPushButton*    m_fullscreenBtn;
};

} // namespace mp
