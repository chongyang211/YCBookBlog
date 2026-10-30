// LibraryPage：媒体库页 —— 三 Tab 列表 + 导入目录 + 搜索。
// 对应书中 §04 阶段② + §08 阶段⑦。
#pragma once

#include <QWidget>

class QLineEdit;
class QTabWidget;
class QListView;
class QLabel;
class QThread;

namespace mp {

class PlayerService;
class MediaRepository;
class MediaScanner;
class MediaListModel;
class MediaListDelegate;

class LibraryPage : public QWidget {
    Q_OBJECT
public:
    LibraryPage(PlayerService* player, MediaRepository* repo, QWidget* parent = nullptr);
    ~LibraryPage() override;

public slots:
    void reload();   // 当前 Tab + 搜索关键字下重新拉数据

private slots:
    void onImportDirectory();
    void onScanBatchFound(QVector<mp::MediaItem> items);
    void onScanFinished(int total);
    void onItemActivated(const QModelIndex& idx);
    void onSearchChanged(const QString& kw);
    void onTabChanged(int idx);

private:
    enum TabIndex { TabAll = 0, TabAudio, TabVideo, TabFavorite, TabHistory };

    PlayerService*  m_player;
    MediaRepository* m_repo;

    QTabWidget*     m_tabs;
    QLineEdit*      m_search;
    QListView*      m_list;
    MediaListModel* m_model;
    MediaListDelegate* m_delegate;
    QLabel*         m_statusLabel;

    QThread*        m_scanThread = nullptr;
    MediaScanner*   m_scanner = nullptr;
};

} // namespace mp
