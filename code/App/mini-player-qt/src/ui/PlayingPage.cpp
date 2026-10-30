#include "PlayingPage.h"

#include "service/PlayerService.h"
#include "components/CoverLabel.h"
#include "LyricsWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QGuiApplication>
#include <QScreen>

namespace mp {

static QString timeStr(qint64 ms) {
    if (ms < 0) ms = 0;
    const qint64 s = ms / 1000;
    return QStringLiteral("%1:%2").arg(s / 60).arg(s % 60, 2, 10, QChar('0'));
}

PlayingPage::PlayingPage(PlayerService* player, QWidget* parent)
    : QWidget(parent), m_player(player) {

    // --- 左：封面 + 元信息 ---
    m_cover = new CoverLabel(this);
    m_cover->setFixedSize(260, 260);
    m_cover->setCircle(true);

    m_titleLbl = new QLabel(tr("未选择曲目"), this);
    QFont f1 = m_titleLbl->font(); f1.setPixelSize(22); f1.setBold(true); m_titleLbl->setFont(f1);
    m_titleLbl->setAlignment(Qt::AlignCenter);
    m_titleLbl->setWordWrap(true);

    m_subLbl = new QLabel(QString(), this);
    QFont f2 = m_subLbl->font(); f2.setPixelSize(13); m_subLbl->setFont(f2);
    m_subLbl->setAlignment(Qt::AlignCenter);
    m_subLbl->setStyleSheet(QStringLiteral("color: #64748b;"));

    m_posLbl = new QLabel(QStringLiteral("0:00"), this);
    m_durLbl = new QLabel(QStringLiteral("0:00"), this);
    for (auto* l : { m_posLbl, m_durLbl }) {
        l->setStyleSheet(QStringLiteral("color: #94a3b8; font-size: 12px;"));
    }

    auto coverBox = new QVBoxLayout();
    coverBox->setSpacing(14);
    coverBox->setAlignment(Qt::AlignCenter);
    coverBox->addStretch();
    coverBox->addWidget(m_cover, 0, Qt::AlignCenter);
    coverBox->addWidget(m_titleLbl);
    coverBox->addWidget(m_subLbl);
    auto timeBox = new QHBoxLayout();
    timeBox->setContentsMargins(0, 0, 0, 0);
    timeBox->setSpacing(6);
    timeBox->addStretch();
    timeBox->addWidget(m_posLbl);
    timeBox->addWidget(new QLabel(QStringLiteral("/"), this));
    timeBox->addWidget(m_durLbl);
    timeBox->addStretch();
    coverBox->addLayout(timeBox);

    // 全屏按钮（仅视频显示）
    m_fullscreenBtn = new QPushButton(QStringLiteral("⛶ ") + tr("视频全屏"), this);
    m_fullscreenBtn->setCursor(Qt::PointingHandCursor);
    m_fullscreenBtn->setVisible(false);
    coverBox->addWidget(m_fullscreenBtn, 0, Qt::AlignCenter);
    coverBox->addStretch();

    auto coverSide = new QWidget(this);
    coverSide->setLayout(coverBox);

    // --- 右：歌词 ---
    m_lyrics = new LyricsWidget(this);
    m_loadLrcBtn = new QPushButton(QStringLiteral("📄 ") + tr("加载 LRC"), this);
    m_loadLrcBtn->setCursor(Qt::PointingHandCursor);
    m_loadLrcBtn->setProperty("flat", true);

    auto lyricsBox = new QVBoxLayout();
    lyricsBox->setContentsMargins(0, 0, 0, 0);
    lyricsBox->setSpacing(8);
    auto toolRow = new QHBoxLayout();
    toolRow->addWidget(m_loadLrcBtn);
    toolRow->addStretch();
    lyricsBox->addLayout(toolRow);
    lyricsBox->addWidget(m_lyrics, 1);

    auto lyricsSide = new QWidget(this);
    lyricsSide->setLayout(lyricsBox);

    // --- Audio 页布局 ---
    auto audioPage = new QWidget(this);
    auto audioLay = new QHBoxLayout(audioPage);
    audioLay->setContentsMargins(30, 20, 30, 20);
    audioLay->setSpacing(30);
    audioLay->addWidget(coverSide, 1);
    audioLay->addWidget(lyricsSide, 1);

    // --- Video 页 ---
    m_video = new QVideoWidget(this);
    m_video->setStyleSheet(QStringLiteral("background: black;"));
    m_video->setMinimumSize(640, 360);
    m_player->mediaPlayer()->setVideoOutput(m_video);

    // --- Stack ---
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(audioPage);
    m_stack->addWidget(m_video);

    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(m_stack);

    // --- 信号 ---
    connect(m_loadLrcBtn, &QPushButton::clicked, this, &PlayingPage::onLoadLrc);
    connect(m_fullscreenBtn, &QPushButton::clicked, this, &PlayingPage::onToggleFullscreen);
    connect(m_lyrics, &LyricsWidget::lineClicked, player, &PlayerService::seek);

    connect(player, &PlayerService::currentChanged,   this, &PlayingPage::onCurrentChanged);
    connect(player, &PlayerService::metadataResolved, this, &PlayingPage::onCurrentChanged);
    connect(player, &PlayerService::stateChanged,     this, &PlayingPage::onStateChanged);
    connect(player, &PlayerService::positionChanged,  this, &PlayingPage::onPositionChanged);
    connect(player, &PlayerService::durationChanged,  this, [this](qint64 ms){
        m_durLbl->setText(timeStr(ms));
    });

    onCurrentChanged();
}

void PlayingPage::onCurrentChanged() {
    const MediaItem it = m_player->current();
    const bool isVideo = (it.type == MediaType::Video);
    m_stack->setCurrentIndex(isVideo ? 1 : 0);
    m_fullscreenBtn->setVisible(isVideo);

    if (it.isValid()) {
        m_titleLbl->setText(it.title);
        QString sub = it.artist;
        if (!it.album.isEmpty() && it.album != QLatin1String("Unknown")) {
            if (!sub.isEmpty()) sub += QStringLiteral(" · ");
            sub += it.album;
        }
        m_subLbl->setText(sub);
        m_cover->setIdForFallback(it.id);
        m_cover->setCoverPath(it.coverPath);
        m_durLbl->setText(timeStr(it.durationMs));
    } else {
        m_titleLbl->setText(tr("未选择曲目"));
        m_subLbl->clear();
        m_lyrics->clear();
    }
}

void PlayingPage::onStateChanged() {
    // 预留：以后可做封面旋转动画启停
}

void PlayingPage::onPositionChanged(qint64 ms) {
    m_posLbl->setText(timeStr(ms));
    m_lyrics->setPositionMs(ms);
}

void PlayingPage::onLoadLrc() {
    const QString file = QFileDialog::getOpenFileName(this, tr("选择 LRC 歌词文件"),
        QString(), QStringLiteral("LRC Files (*.lrc *.txt)"));
    if (file.isEmpty()) return;
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&f);
    m_lyrics->setLrcText(in.readAll());
}

void PlayingPage::onToggleFullscreen() {
    if (m_video->isFullScreen()) m_video->setFullScreen(false);
    else                          m_video->setFullScreen(true);
}

} // namespace mp
