#include "SettingsPage.h"

#include "service/PlayerService.h"
#include "service/SleepTimer.h"
#include "service/ThemeManager.h"
#include "data/MediaRepository.h"
#include "components/ToastHost.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QMessageBox>
#include <QButtonGroup>
#include <QTimer>
#include <QFrame>

namespace mp {

static QFrame* makeCard(QWidget* parent, const QString& title, const QString& desc) {
    auto card = new QFrame(parent);
    card->setObjectName(QStringLiteral("card"));
    auto lay = new QVBoxLayout(card);
    lay->setContentsMargins(20, 18, 20, 18);
    lay->setSpacing(10);
    auto t = new QLabel(title, card);
    QFont tf = t->font(); tf.setPixelSize(16); tf.setBold(true); t->setFont(tf);
    lay->addWidget(t);
    if (!desc.isEmpty()) {
        auto d = new QLabel(desc, card);
        d->setStyleSheet(QStringLiteral("color:#64748b; font-size:12px;"));
        lay->addWidget(d);
    }
    return card;
}

SettingsPage::SettingsPage(PlayerService* player, MediaRepository* repo, QWidget* parent)
    : QWidget(parent), m_player(player), m_repo(repo), m_timer(new SleepTimer(this)) {

    // ---- 外观 ----
    auto appearance = makeCard(this, tr("外观"), tr("选择颜色主题，auto 会跟随操作系统"));
    auto apRow = new QHBoxLayout();
    apRow->setSpacing(8);

    auto group = new QButtonGroup(this);
    group->setExclusive(true);
    for (auto pair : { std::make_pair(QString(QStringLiteral("🖥 跟随系统")), ThemeMode::Auto),
                       std::make_pair(QString(QStringLiteral("☀ 浅色")),       ThemeMode::Light),
                       std::make_pair(QString(QStringLiteral("🌙 深色")),      ThemeMode::Dark) }) {
        auto b = new QPushButton(pair.first, this);
        b->setCheckable(true);
        b->setCursor(Qt::PointingHandCursor);
        if (ThemeManager::instance().mode() == pair.second) b->setChecked(true);
        if (pair.second == ThemeMode::Auto)  b->setChecked(ThemeManager::instance().mode() == ThemeMode::Auto);
        apRow->addWidget(b);
        group->addButton(b);
        connect(b, &QPushButton::clicked, this, [this, m = pair.second]{
            ThemeManager::instance().setMode(m);
            ToastHost::instance()->show(tr("主题已切换"), ToastHost::Success, 1200);
        });
    }
    apRow->addStretch();
    qobject_cast<QVBoxLayout*>(appearance->layout())->addLayout(apRow);

    // ---- 睡眠定时 ----
    auto sleepCard = makeCard(this, tr("睡眠定时"),
                              tr("N 分钟后自动暂停，最后 10 秒做音量淡出"));
    auto sleepRow = new QHBoxLayout();
    sleepRow->setSpacing(10);
    m_minutes = new QSpinBox(this);
    m_minutes->setRange(1, 180);
    m_minutes->setValue(30);
    m_minutes->setSuffix(tr(" 分钟"));
    m_minutes->setFixedWidth(140);

    m_startBtn  = new QPushButton(tr("启动"), this);
    m_startBtn->setProperty("primary", true);
    m_cancelBtn = new QPushButton(tr("取消"), this);
    m_cancelBtn->setEnabled(false);

    m_remainLabel = new QLabel(tr("剩余: —"), this);
    m_remainLabel->setStyleSheet(QStringLiteral("color:#64748b;"));

    sleepRow->addWidget(m_minutes);
    sleepRow->addWidget(m_startBtn);
    sleepRow->addWidget(m_cancelBtn);
    sleepRow->addStretch();
    sleepRow->addWidget(m_remainLabel);
    qobject_cast<QVBoxLayout*>(sleepCard->layout())->addLayout(sleepRow);

    connect(m_startBtn, &QPushButton::clicked, this, &SettingsPage::onStartTimer);
    connect(m_cancelBtn, &QPushButton::clicked, this, &SettingsPage::onCancelTimer);
    connect(m_timer, &SleepTimer::stateChanged, this, [this](bool active, qint64 remain){
        m_startBtn->setEnabled(!active);
        m_cancelBtn->setEnabled(active);
        if (!active) { m_remainLabel->setText(tr("剩余: —")); return; }
        const int m = static_cast<int>(remain / 60000);
        const int s = static_cast<int>((remain % 60000) / 1000);
        m_remainLabel->setText(tr("剩余: %1:%2").arg(m).arg(s, 2, 10, QChar('0')));
    });

    m_uiTick = new QTimer(this);
    m_uiTick->setInterval(500);
    m_uiTick->start();

    // ---- 媒体库 ----
    auto libCard = makeCard(this, tr("媒体库"), tr("本地 SQLite 存储（WAL 模式）"));
    m_statLabel = new QLabel(this);
    m_statLabel->setStyleSheet(QStringLiteral("font-size:13px; color:#475569;"));
    auto clearBtn = new QPushButton(tr("清空媒体库"), this);
    clearBtn->setProperty("danger", true);
    auto libRow = new QHBoxLayout();
    libRow->addWidget(m_statLabel);
    libRow->addStretch();
    libRow->addWidget(clearBtn);
    qobject_cast<QVBoxLayout*>(libCard->layout())->addLayout(libRow);
    connect(clearBtn, &QPushButton::clicked, this, &SettingsPage::onClearLibrary);

    // ---- 关于 ----
    auto about = makeCard(this, tr("关于"), QString());
    auto aboutRow = qobject_cast<QVBoxLayout*>(about->layout());
    for (const QString& line : {
        QStringLiteral("MiniPlayer · Qt 桌面端  v0.1.0"),
        QStringLiteral("C++17 · Qt 6 · QMediaPlayer · QSqlDatabase"),
        QStringLiteral("配套：书中 10.Apps/07.综合音视频案例/05.Qt桌面端实现(C++三端共用).md")
    }) {
        auto l = new QLabel(line, this);
        l->setStyleSheet(QStringLiteral("color:#64748b; font-size:12px;"));
        aboutRow->addWidget(l);
    }

    // ---- 装配 ----
    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(30, 20, 30, 20);
    outer->setSpacing(14);
    auto title = new QLabel(tr("设置"), this);
    QFont tFont = title->font(); tFont.setPixelSize(22); tFont.setBold(true); title->setFont(tFont);
    outer->addWidget(title);
    outer->addWidget(appearance);
    outer->addWidget(sleepCard);
    outer->addWidget(libCard);
    outer->addWidget(about);
    outer->addStretch();

    refreshStats();
}

SettingsPage::~SettingsPage() = default;

void SettingsPage::onStartTimer() {
    if (!m_player->current().isValid()) {
        ToastHost::instance()->show(tr("请先选择一首歌再启动定时"), ToastHost::Warn);
        return;
    }
    m_timer->start(m_minutes->value(), m_player);
    ToastHost::instance()->show(tr("%1 分钟后自动暂停").arg(m_minutes->value()), ToastHost::Success);
}

void SettingsPage::onCancelTimer() {
    m_timer->cancel();
    ToastHost::instance()->show(tr("已取消睡眠定时"), ToastHost::Info);
}

void SettingsPage::onClearLibrary() {
    const auto reply = QMessageBox::question(this, tr("清空媒体库"),
        tr("确定要清空整个媒体库吗？\n（歌单/历史/收藏/设置都将被删除）"));
    if (reply != QMessageBox::Yes) return;
    m_repo->clearAll();
    ToastHost::instance()->show(tr("媒体库已清空"), ToastHost::Success);
    emit libraryCleared();
    refreshStats();
}

void SettingsPage::refreshStats() {
    const int total = m_repo->listAll().size();
    const int fav   = m_repo->listFavorites().size();
    const int hist  = m_repo->listHistory(500).size();
    m_statLabel->setText(tr("曲目 %1 · 收藏 %2 · 历史 %3").arg(total).arg(fav).arg(hist));
}

} // namespace mp
