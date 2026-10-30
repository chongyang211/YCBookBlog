// MainWindow：三栏 shell：左侧栏（Library/Playing/Settings）+ 主区 StackedWidget + 底部 PlayerBar。
// 对应书中 §03/§05 的主窗口组装。
#pragma once

#include <QMainWindow>

class QStackedWidget;

namespace mp {

class PlayerService;
class MediaRepository;
class LibraryPage;
class PlayingPage;
class SettingsPage;
class PlayerBar;
class SystemMediaAdapter;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupShortcuts();

    MediaRepository*     m_repo;
    PlayerService*       m_player;
    SystemMediaAdapter*  m_adapter;

    QStackedWidget*  m_mainStack;
    LibraryPage*     m_libraryPage;
    PlayingPage*     m_playingPage;
    SettingsPage*    m_settingsPage;
    PlayerBar*       m_playerBar;
};

} // namespace mp
