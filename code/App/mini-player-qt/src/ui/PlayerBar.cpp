#include "PlayerBar.h"
#include "service/PlayerService.h"
#include "components/CoverLabel.h"
#include "components/ProgressSlider.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QMouseEvent>

namespace mp {

static QString timeStr(qint64 ms) {
    if (ms < 0) ms = 0;
    const qint64 s = ms / 1000;
    return QStringLiteral("%1:%2").arg(s / 60).arg(s % 60, 2, 10, QChar('0'));
}

PlayerBar::PlayerBar(PlayerService* player, QWidget* parent)
    : QFrame(parent), m_player(player) {
    setObjectName(QStringLiteral("playerBar"));
    setFixedHeight(94);
    setFrameShape(QFrame::NoFrame);

    // --- 左：封面 + 标题/艺术家 ---
    m_cover = new CoverLabel(this);
    m_cover->setFixedSize(54, 54);
    m_cover->setCursor(Qt::PointingHandCursor);

    m_title  = new QLabel(tr("未选择曲目"), this);
    m_title->setObjectName(QStringLiteral("titleLabel"));
    m_artist = new QLabel(tr("从媒体库选一首歌开始播放"), this);
    m_artist->setObjectName(QStringLiteral("artistLabel"));

    auto metaBox = new QVBoxLayout();
    metaBox->setContentsMargins(0, 0, 0, 0);
    metaBox->setSpacing(2);
    metaBox->addStretch();
    metaBox->addWidget(m_title);
    metaBox->addWidget(m_artist);
    metaBox->addStretch();

    auto leftBox = new QHBoxLayout();
    leftBox->setContentsMargins(0, 0, 0, 0);
    leftBox->setSpacing(12);
    leftBox->addWidget(m_cover);
    leftBox->addLayout(metaBox);
    leftBox->addStretch();

    auto leftW = new QWidget(this);
    leftW->setLayout(leftBox);
    leftW->setFixedWidth(300);
    leftW->setCursor(Qt::PointingHandCursor);

    // --- 中：按钮 + 进度条 ---
    m_loopBtn = new QPushButton(QStringLiteral("🔁"), this);
    m_prevBtn = new QPushButton(QStringLiteral("⏮"), this);
    m_playBtn = new QPushButton(QStringLiteral("▶"), this);
    m_nextBtn = new QPushButton(QStringLiteral("⏭"), this);
    for (auto* b : { m_loopBtn, m_prevBtn, m_nextBtn }) {
        b->setProperty("iconBtn", true);
        b->setCursor(Qt::PointingHandCursor);
    }
    m_playBtn->setProperty("playMain", true);
    m_playBtn->setCursor(Qt::PointingHandCursor);

    auto btnBox = new QHBoxLayout();
    btnBox->setContentsMargins(0, 0, 0, 0);
    btnBox->setSpacing(10);
    btnBox->addStretch();
    btnBox->addWidget(m_loopBtn);
    btnBox->addWidget(m_prevBtn);
    btnBox->addWidget(m_playBtn);
    btnBox->addWidget(m_nextBtn);
    btnBox->addStretch();

    m_posLabel = new QLabel(QStringLiteral("0:00"), this);
    m_posLabel->setObjectName(QStringLiteral("timeLabel"));
    m_posLabel->setFixedWidth(44);
    m_posLabel->setAlignment(Qt::AlignCenter);

    m_durLabel = new QLabel(QStringLiteral("0:00"), this);
    m_durLabel->setObjectName(QStringLiteral("timeLabel"));
    m_durLabel->setFixedWidth(44);
    m_durLabel->setAlignment(Qt::AlignCenter);

    m_progress = new ProgressSlider(this);

    auto progBox = new QHBoxLayout();
    progBox->setContentsMargins(0, 0, 0, 0);
    progBox->setSpacing(8);
    progBox->addWidget(m_posLabel);
    progBox->addWidget(m_progress, 1);
    progBox->addWidget(m_durLabel);

    auto midBox = new QVBoxLayout();
    midBox->setContentsMargins(0, 0, 0, 0);
    midBox->setSpacing(2);
    midBox->addLayout(btnBox);
    midBox->addLayout(progBox);
    auto midW = new QWidget(this);
    midW->setLayout(midBox);

    // --- 右：音量 ---
    m_muteBtn = new QPushButton(QStringLiteral("🔊"), this);
    m_muteBtn->setProperty("iconBtn", true);
    m_muteBtn->setCursor(Qt::PointingHandCursor);
    m_volSlider = new QSlider(Qt::Horizontal, this);
    m_volSlider->setRange(0, 100);
    m_volSlider->setValue(static_cast<int>(player->volume() * 100));
    m_volSlider->setFixedWidth(110);

    auto rightBox = new QHBoxLayout();
    rightBox->setContentsMargins(0, 0, 0, 0);
    rightBox->setSpacing(8);
    rightBox->addStretch();
    rightBox->addWidget(m_muteBtn);
    rightBox->addWidget(m_volSlider);
    auto rightW = new QWidget(this);
    rightW->setLayout(rightBox);
    rightW->setFixedWidth(220);

    // --- 装配 ---
    auto outer = new QHBoxLayout(this);
    outer->setContentsMargins(20, 10, 20, 10);
    outer->setSpacing(24);
    outer->addWidget(leftW);
    outer->addWidget(midW, 1);
    outer->addWidget(rightW);

    // 顶部 1px 分隔线靠 QSS 的 border-top
    setStyleSheet(QString()); // 让 QSS 从 app 全局继承

    // --- 信号槽 ---
    connect(m_playBtn, &QPushButton::clicked, player, &PlayerService::toggle);
    connect(m_prevBtn, &QPushButton::clicked, player, &PlayerService::previous);
    connect(m_nextBtn, &QPushButton::clicked, player, [p = player]{ p->next(false); });
    connect(m_loopBtn, &QPushButton::clicked, this, [this, player]{
        const LoopMode cur = player->loopMode();
        LoopMode nxt = LoopMode::Order;
        if      (cur == LoopMode::Order)  nxt = LoopMode::Single;
        else if (cur == LoopMode::Single) nxt = LoopMode::Random;
        else                              nxt = LoopMode::Order;
        player->setLoopMode(nxt);
    });
    connect(m_muteBtn, &QPushButton::clicked, player, &PlayerService::toggleMute);
    connect(m_volSlider, &QSlider::valueChanged, player, [p = player](int v){
        p->setVolume(static_cast<float>(v) / 100.0f);
    });

    // 进度条
    connect(m_progress, &ProgressSlider::seek, player, &PlayerService::seek);
    connect(m_progress, &ProgressSlider::seekingChanged, player, &PlayerService::setSeeking);

    // 封面点击进入 Playing 页
    connect(m_cover, &CoverLabel::clicked, this, &PlayerBar::gotoPlayingPage);

    // 服务 → UI
    connect(player, &PlayerService::stateChanged,     this, &PlayerBar::onStateChanged);
    connect(player, &PlayerService::positionChanged,  this, &PlayerBar::onPositionChanged);
    connect(player, &PlayerService::durationChanged,  this, &PlayerBar::onDurationChanged);
    connect(player, &PlayerService::currentChanged,   this, &PlayerBar::onCurrentChanged);
    connect(player, &PlayerService::metadataResolved, this, &PlayerBar::onCurrentChanged);
    connect(player, &PlayerService::volumeChanged,    this, &PlayerBar::onVolumeChanged);
    connect(player, &PlayerService::mutedChanged,     this, &PlayerBar::onMutedChanged);
    connect(player, &PlayerService::loopModeChanged,  this, &PlayerBar::onLoopModeChanged);

    refreshLoopIcon();
}

void PlayerBar::onCurrentChanged() {
    const MediaItem it = m_player->current();
    if (it.isValid()) {
        m_title->setText(it.title.isEmpty() ? tr("未命名") : it.title);
        m_artist->setText(it.artist.isEmpty() ? tr("—") : it.artist);
        m_cover->setIdForFallback(it.id);
        m_cover->setCoverPath(it.coverPath);
        m_durLabel->setText(timeStr(it.durationMs));
        m_progress->setDurationMs(it.durationMs);
    } else {
        m_title->setText(tr("未选择曲目"));
        m_artist->setText(tr("从媒体库选一首歌开始播放"));
        m_cover->setIdForFallback(QString());
        m_cover->setCoverPath(QString());
    }
}

void PlayerBar::onStateChanged() {
    const bool playing = m_player->isPlaying();
    m_playBtn->setText(playing ? QStringLiteral("⏸") : QStringLiteral("▶"));
}

void PlayerBar::onPositionChanged(qint64 ms) {
    m_progress->setPositionMs(ms);
    m_posLabel->setText(timeStr(ms));
}

void PlayerBar::onDurationChanged(qint64 ms) {
    m_progress->setDurationMs(ms);
    m_durLabel->setText(timeStr(ms));
}

void PlayerBar::onVolumeChanged() {
    if (!m_volSlider->isSliderDown())
        m_volSlider->setValue(static_cast<int>(m_player->volume() * 100));
}

void PlayerBar::onMutedChanged() {
    const bool m = m_player->isMuted();
    m_muteBtn->setText(m ? QStringLiteral("🔇") : QStringLiteral("🔊"));
}

void PlayerBar::onLoopModeChanged() {
    refreshLoopIcon();
}

void PlayerBar::refreshLoopIcon() {
    switch (m_player->loopMode()) {
        case LoopMode::Single: m_loopBtn->setText(QStringLiteral("🔂")); m_loopBtn->setToolTip(tr("单曲循环")); break;
        case LoopMode::Random: m_loopBtn->setText(QStringLiteral("🔀")); m_loopBtn->setToolTip(tr("随机播放")); break;
        default:               m_loopBtn->setText(QStringLiteral("🔁")); m_loopBtn->setToolTip(tr("顺序循环")); break;
    }
}

} // namespace mp
