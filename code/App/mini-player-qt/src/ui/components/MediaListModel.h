// MediaListModel：QAbstractListModel 承载 MediaItem 列表。
// 对应书中 §04 Step 4.7 + 架构篇 §1.6 对照（iOS DataSource / Web <MediaList>）
#pragma once

#include <QAbstractListModel>
#include <QVector>
#include "types/MediaItem.h"

namespace mp {

class MediaListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        ArtistRole,
        AlbumRole,
        DurationRole,
        FilePathRole,
        CoverPathRole,
        IsPlayingRole,
    };

    explicit MediaListModel(QObject* parent = nullptr);

    void setItems(const QVector<MediaItem>& items);
    const QVector<MediaItem>& items() const { return m_items; }
    MediaItem at(int row) const;

    /** 高亮当前播放：触发 dataChanged(IsPlayingRole) */
    void setCurrentPlayingId(const QString& id);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<MediaItem> m_items;
    QString m_playingId;
};

} // namespace mp
