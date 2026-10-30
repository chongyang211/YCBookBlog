#include "MainWindow.h"

#include "LibraryPage.h"
#include "PlayingPage.h"
#include "SettingsPage.h"
#include "PlayerBar.h"
#include "service/PlayerService.h"
#include "service/ThemeManager.h"
#include "data/MediaRepository.h"
#include "platform/SystemMediaAdapter.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QKeySequence>
#include <QStyle>

namespace mp {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("MiniPlayer"));

    // ---- 核心服务（Qt 对象树自动管内存）----
    m_repo    = new MediaRepository(this);
    m_player  = new PlayerService(m_repo, this);
    m_adapter = SystemMediaAdapter::create(this);

    // 适配器双向桥接：§09 真实实现后，锁屏/托盘控件就能联动
    connect(m_player, &PlayerService::currentChanged, m_adapter,
            [this](const MediaItem& it){ m_adapter->setMetadata(it); });
    connect(m_player, &PlayerService::stateChanged,
            m_adapter, &SystemMediaAdapter::setPlaybackState);
    connect(m_player, &PlayerService::positionChanged, m_adapter,
            [this](qint64 pos){ m_adapter->setPosition(pos, m_player->durationMs()); });
    connect(m_adapter, &SystemMediaAdapter::playRequested,     m_player, &PlayerService::play);
    connect(m_adapter, &SystemMediaAdapter::pauseRequested,    m_player, &PlayerService::pause);
    connect(m_adapter, &SystemMediaAdapter::nextRequested,     this, [this]{ m_player->next(false); });
    connect(m_adapter, &SystemMediaAdapter::previousRequested, m_player, &PlayerService::previous);
    connect(m_adapter, &SystemMediaAdapter::seekRequested,     m_player, &PlayerService::seek);

    // ---- 侧栏 ----
    auto sidebar = new QFrame(this);
    sidebar->setObjectName(QStringLiteral("sidebar"));
    sidebar->setFixedWidth(220);
    auto sideLay = new QVBoxLayout(sidebar);
    sideLay->setContentsMargins(14, 20, 14, 20);
    sideLay->setSpacing(4);

    auto logo = new QLabel(QStringLiteral("● MiniPlayer"), sidebar);
    logo->setObjectName(QStringLiteral("logo"));
    sideLay->addWidget(logo);

    struct NavItem { QString icon; QString label; int page; };
    const QList<NavItem> navs = {
        { QStringLiteral("♪"), tr("媒体库"), 0 },
        { QStringLiteral("▶"), tr("正在播放"), 1 },
        { QStringLiteral("⚙"), tr("设置"), 2 },
    };
    QList<QPushButton*> navBtns;
    for (const auto& n : navs) {
        auto b = new QPushButton(QStringLiteral("  %1    %2").arg(n.icon, n.label), sidebar);
        b->setProperty("navItem", true);
        b->setCursor(Qt::PointingHandCursor);
        sideLay->addWidget(b);
        navBtns.append(b);
    }
    sideLay->addStretch();

    // ---- 主区 ----
    m_libraryPage  = new LibraryPage(m_player, m_repo, this);
    m_playingPage  = new PlayingPage(m_player, this);
    m_settingsPage = new SettingsPage(m_player, m_repo, this);

    m_mainStack = new QStackedWidget(this);
    m_mainStack->addWidget(m_libraryPage);
    m_mainStack->addWidget(m_playingPage);
    m_mainStack->addWidget(m_settingsPage);

    auto activatePage = [this, navBtns](int page){
        m_mainStack->setCurrentIndex(page);
        for (int i = 0; i < navBtns.size(); ++i) {
            navBtns[i]->setProperty("selected", i == page);
            navBtns[i]->style()->unpolish(navBtns[i]);
            navBtns[i]->style()->polish(navBtns[i]);
        }
    };
    for (int i = 0; i < navBtns.size(); ++i) {
        connect(navBtns[i], &QPushButton::clicked, this, [=]{ activatePage(i); });
    }
    activatePage(0);

    // ---- 顶部横向：sidebar + mainStack ----
    auto topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(0);
    topRow->addWidget(sidebar);
    topRow->addWidget(m_mainStack, 1);

    // ---- 底部 PlayerBar ----
    m_playerBar = new PlayerBar(m_player, this);
    connect(m_playerBar, &PlayerBar::gotoPlayingPage, this, [=]{ activatePage(1); });

    // ---- 装配 ----
    auto central = new QWidget(this);
    auto rootLay = new QVBoxLayout(central);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->setSpacing(0);
    rootLay->addLayout(topRow, 1);
    rootLay->addWidget(m_playerBar);
    setCentralWidget(central);

    setupShortcuts();

    // 库清空时刷新 Library 列表
    connect(m_settingsPage, &SettingsPage::libraryCleared, m_libraryPage, &LibraryPage::reload);

    // ---- 主题跟随（ThemeManager 单例构造时 topLevelWidgets 为空，这里自己同步一次） ----
    auto applyTheme = [this]{
        const bool dark = ThemeManager::instance().isDarkActive();
        setProperty("theme", dark ? "dark" : "light");
        style()->unpolish(this);
        style()->polish(this);
        update();
    };
    applyTheme();
    connect(&ThemeManager::instance(), &ThemeManager::darkActiveChanged, this, [applyTheme](bool){
        applyTheme();
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::setupShortcuts() {
    // 对照架构篇 §1.7 的四端通用键位表
    new QShortcut(QKeySequence(Qt::Key_Space), this, [this]{ m_player->toggle(); });
    new QShortcut(QKeySequence(Qt::Key_Left),  this, [this]{
        m_player->seek(qMax<qint64>(0, m_player->positionMs() - 5000));
    });
    new QShortcut(QKeySequence(Qt::Key_Right), this, [this]{
        m_player->seek(m_player->positionMs() + 5000);
    });
    new QShortcut(QKeySequence(Qt::Key_Up), this, [this]{
        m_player->setVolume(qMin(1.0f, m_player->volume() + 0.05f));
    });
    new QShortcut(QKeySequence(Qt::Key_Down), this, [this]{
        m_player->setVolume(qMax(0.0f, m_player->volume() - 0.05f));
    });
    new QShortcut(QKeySequence(Qt::Key_M), this, [this]{ m_player->toggleMute(); });
    new QShortcut(QKeySequence(Qt::Key_N), this, [this]{ m_player->next(false); });
    new QShortcut(QKeySequence(Qt::Key_P), this, [this]{ m_player->previous(); });
}

} // namespace mp
