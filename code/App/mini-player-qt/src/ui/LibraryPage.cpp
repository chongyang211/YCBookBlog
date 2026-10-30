#include "LibraryPage.h"

#include "service/PlayerService.h"
#include "data/MediaRepository.h"
#include "data/MediaScanner.h"
#include "components/MediaListModel.h"
#include "components/MediaListDelegate.h"
#include "components/ToastHost.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTabWidget>
#include <QListView>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>

namespace mp {

LibraryPage::LibraryPage(PlayerService* player, MediaRepository* repo, QWidget* parent)
    : QWidget(parent), m_player(player), m_repo(repo) {

    // Toolbar
    auto title = new QLabel(tr("媒体库"), this);
    QFont tFont = title->font(); tFont.setPixelSize(22); tFont.setBold(true);
    title->setFont(tFont);
    title->setObjectName(QStringLiteral("pageTitle"));

    m_search = new QLineEdit(this);
    m_search->setPlaceholderText(tr("搜索标题 / 艺术家 / 专辑..."));
    m_search->setClearButtonEnabled(true);
    m_search->setFixedHeight(32);
    m_search->setMinimumWidth(220);

    auto btnImport = new QPushButton(QStringLiteral("📁 ") + tr("导入目录"), this);
    btnImport->setProperty("primary", true);
    btnImport->setCursor(Qt::PointingHandCursor);

    auto toolbar = new QFrame(this);
    toolbar->setObjectName(QStringLiteral("libraryToolbar"));
    auto tbLay = new QHBoxLayout(toolbar);
    tbLay->setContentsMargins(16, 10, 16, 10);
    tbLay->setSpacing(10);
    tbLay->addWidget(title);
    tbLay->addStretch();
    tbLay->addWidget(m_search);
    tbLay->addWidget(btnImport);

    // Tabs
    m_tabs = new QTabWidget(this);
    m_tabs->setDocumentMode(true);
    m_tabs->addTab(new QWidget(this), tr("全部"));
    m_tabs->addTab(new QWidget(this), tr("音频"));
    m_tabs->addTab(new QWidget(this), tr("视频"));
    m_tabs->addTab(new QWidget(this), tr("我喜欢"));
    m_tabs->addTab(new QWidget(this), tr("最近播放"));

    // List
    m_model    = new MediaListModel(this);
    m_delegate = new MediaListDelegate(this);
    m_list     = new QListView(this);
    m_list->setModel(m_model);
    m_list->setItemDelegate(m_delegate);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setUniformItemSizes(true);
    m_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_list->setFrameShape(QFrame::NoFrame);

    // 状态行
    m_statusLabel = new QLabel(QString(), this);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #64748b; font-size: 12px;"));

    // 装配
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 10);
    layout->setSpacing(14);
    layout->addWidget(toolbar);
    layout->addWidget(m_tabs);
    layout->addWidget(m_list, 1);
    layout->addWidget(m_statusLabel);

    // 信号
    connect(btnImport, &QPushButton::clicked, this, &LibraryPage::onImportDirectory);
    connect(m_search, &QLineEdit::textChanged, this, &LibraryPage::onSearchChanged);
    connect(m_tabs, &QTabWidget::currentChanged, this, &LibraryPage::onTabChanged);
    connect(m_list, &QListView::activated, this, &LibraryPage::onItemActivated);
    connect(m_list, &QListView::doubleClicked, this, &LibraryPage::onItemActivated);

    // 跟随当前播放：高亮
    connect(player, &PlayerService::currentChanged, this, [this](const MediaItem& it){
        m_model->setCurrentPlayingId(it.id);
    });

    reload();
}

LibraryPage::~LibraryPage() {
    if (m_scanner) m_scanner->cancel();
    if (m_scanThread) {
        m_scanThread->quit();
        m_scanThread->wait(2000);
    }
}

void LibraryPage::reload() {
    const QString kw = m_search->text().trimmed();
    QVector<MediaItem> items;
    if (!kw.isEmpty()) {
        items = m_repo->search(kw);
    } else {
        switch (m_tabs->currentIndex()) {
            case TabAudio:    items = m_repo->listByType(MediaType::Audio); break;
            case TabVideo:    items = m_repo->listByType(MediaType::Video); break;
            case TabFavorite: items = m_repo->listFavorites(); break;
            case TabHistory:  items = m_repo->listHistory(); break;
            case TabAll:
            default:          items = m_repo->listAll(); break;
        }
    }
    m_model->setItems(items);
    m_model->setCurrentPlayingId(m_player->current().id);

    if (items.isEmpty()) {
        const QString hint = kw.isEmpty()
            ? (m_tabs->currentIndex() == TabFavorite ? tr("尚未收藏任何曲目")
             : m_tabs->currentIndex() == TabHistory  ? tr("暂无播放历史")
             : tr("媒体库为空 · 点击右上角「导入目录」"))
            : tr("无匹配 \"%1\" 的结果").arg(kw);
        m_statusLabel->setText(hint);
    } else {
        m_statusLabel->setText(tr("共 %1 项").arg(items.size()));
    }
}

void LibraryPage::onSearchChanged(const QString&) {
    // 防抖：220ms
    static QTimer* t = nullptr;
    if (!t) {
        t = new QTimer(this);
        t->setSingleShot(true);
        connect(t, &QTimer::timeout, this, &LibraryPage::reload);
    }
    t->start(220);
}

void LibraryPage::onTabChanged(int) { reload(); }

void LibraryPage::onItemActivated(const QModelIndex& idx) {
    if (!idx.isValid()) return;
    const MediaItem m = m_model->at(idx.row());
    if (!m.isValid()) return;
    // 把当前列表作为队列
    m_player->loadQueue(m_model->items(), idx.row(), true);
}

void LibraryPage::onImportDirectory() {
    const QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    const QString dir = QFileDialog::getExistingDirectory(this, tr("选择要扫描的媒体目录"), defaultDir);
    if (dir.isEmpty()) return;

    // 后台扫描：避免 10 万文件堵死 UI（§04 Step 4.4 的四端对照考点）
    if (m_scanThread) {
        if (m_scanner) m_scanner->cancel();
        m_scanThread->quit();
        m_scanThread->wait(1000);
        m_scanThread->deleteLater();
        m_scanThread = nullptr;
        m_scanner = nullptr;
    }

    m_scanThread = new QThread(this);
    m_scanner = new MediaScanner();   // 不设 parent，moveToThread 要求
    m_scanner->moveToThread(m_scanThread);
    connect(m_scanThread, &QThread::finished, m_scanner, &QObject::deleteLater);
    connect(m_scanner, &MediaScanner::batchFound, this, &LibraryPage::onScanBatchFound);
    connect(m_scanner, &MediaScanner::finished,   this, &LibraryPage::onScanFinished);
    connect(m_scanner, &MediaScanner::started, this, [this](int){
        m_statusLabel->setText(tr("正在扫描目录..."));
    });
    connect(m_scanner, &MediaScanner::progress, this, [this](int done, int /*total*/){
        m_statusLabel->setText(tr("已发现 %1 个媒体文件...").arg(done));
    });

    m_scanThread->start();
    QMetaObject::invokeMethod(m_scanner, "scanDirectories",
                              Qt::QueuedConnection,
                              Q_ARG(QStringList, { dir }));
    ToastHost::instance()->show(tr("开始扫描: ") + dir, ToastHost::Info);
}

void LibraryPage::onScanBatchFound(QVector<MediaItem> items) {
    // 分批入库 + 分批刷新 UI
    if (items.isEmpty()) return;
    m_repo->upsertBatch(items);
}

void LibraryPage::onScanFinished(int total) {
    ToastHost::instance()->show(tr("扫描完成，共导入/更新 %1 条").arg(total), ToastHost::Success);
    if (m_scanThread) {
        m_scanThread->quit();
        m_scanThread->wait(1500);
        m_scanThread->deleteLater();
        m_scanThread = nullptr;
        m_scanner = nullptr;
    }
    reload();
}

} // namespace mp
