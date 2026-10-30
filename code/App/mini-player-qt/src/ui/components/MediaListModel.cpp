#include "MediaListModel.h"

namespace mp {

MediaListModel::MediaListModel(QObject* parent) : QAbstractListModel(parent) {}

void MediaListModel::setItems(const QVector<MediaItem>& items) {
    beginResetModel();
    m_items = items;
    endResetModel();
}

MediaItem MediaListModel::at(int row) const {
    if (row < 0 || row >= m_items.size()) return {};
    return m_items[row];
}

void MediaListModel::setCurrentPlayingId(const QString& id) {
    if (m_playingId == id) return;
    const QString oldId = m_playingId;
    m_playingId = id;
    // 两个受影响行 emit dataChanged
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == oldId || m_items[i].id == id) {
            const auto idx = index(i);
            emit dataChanged(idx, idx, { IsPlayingRole });
        }
    }
}

int MediaListModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : m_items.size();
}

QVariant MediaListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return {};
    const int row = index.row();
    if (row < 0 || row >= m_items.size()) return {};
    const MediaItem& m = m_items[row];
    switch (role) {
        case Qt::DisplayRole:
        case TitleRole:    return m.title;
        case IdRole:       return m.id;
        case TypeRole:     return QString::fromLatin1(toString(m.type));
        case ArtistRole:   return m.artist;
        case AlbumRole:    return m.album;
        case DurationRole: return m.durationMs;
        case FilePathRole: return m.filePath;
        case CoverPathRole:return m.coverPath;
        case IsPlayingRole:return m.id == m_playingId;
    }
    return {};
}

QHash<int, QByteArray> MediaListModel::roleNames() const {
    return {
        { IdRole,         "id" },
        { TypeRole,       "type" },
        { TitleRole,      "title" },
        { ArtistRole,     "artist" },
        { AlbumRole,      "album" },
        { DurationRole,   "duration" },
        { FilePathRole,   "filePath" },
        { CoverPathRole,  "coverPath" },
        { IsPlayingRole,  "isPlaying" },
    };
}

} // namespace mp
