#include "MediaListDelegate.h"
#include "MediaListModel.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QPixmap>
#include <QCryptographicHash>

namespace mp {

static int hashInt(const QString& s, int shift = 0) {
    const QByteArray b = QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Md5);
    return (static_cast<unsigned char>(b[0 + shift]) * 256 +
            static_cast<unsigned char>(b[1 + shift])) % 360;
}

MediaListDelegate::MediaListDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QSize MediaListDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
    return { 0, 60 };
}

void MediaListDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt,
                              const QModelIndex& index) const {
    p->save();
    p->setRenderHint(QPainter::Antialiasing);

    const QString id       = index.data(MediaListModel::IdRole).toString();
    const QString title    = index.data(MediaListModel::TitleRole).toString();
    const QString artist   = index.data(MediaListModel::ArtistRole).toString();
    const QString album    = index.data(MediaListModel::AlbumRole).toString();
    const qint64  durMs    = index.data(MediaListModel::DurationRole).toLongLong();
    const QString cover    = index.data(MediaListModel::CoverPathRole).toString();
    const QString type     = index.data(MediaListModel::TypeRole).toString();
    const bool    playing  = index.data(MediaListModel::IsPlayingRole).toBool();

    const QRect r = opt.rect;

    // 行背景
    if (opt.state & QStyle::State_MouseOver) {
        p->fillRect(r, QColor(0, 0, 0, 10));
    }
    if (playing || (opt.state & QStyle::State_Selected)) {
        p->fillRect(r, QColor(0x8b, 0x5c, 0xf6, 36));   // 紫色 soft
    }

    constexpr int kPad = 12;
    constexpr int kCoverSize = 44;
    const QRect coverRect(r.left() + kPad, r.top() + (r.height() - kCoverSize) / 2,
                          kCoverSize, kCoverSize);

    // 封面
    QPainterPath clip;
    clip.addRoundedRect(coverRect, 8, 8);
    p->save();
    p->setClipPath(clip);
    if (!cover.isEmpty()) {
        QPixmap pix(cover);
        if (!pix.isNull()) p->drawPixmap(coverRect, pix, pix.rect());
    } else {
        // 渐变占位
        const int h1 = hashInt(id);
        const int h2 = (h1 + 40 + hashInt(id, 2)) % 360;
        QLinearGradient g(coverRect.topLeft(), coverRect.bottomRight());
        g.setColorAt(0, QColor::fromHsl(h1, 180, 150));
        g.setColorAt(1, QColor::fromHsl(h2, 180, 120));
        p->fillRect(coverRect, g);
        p->setPen(QColor(255, 255, 255, 220));
        QFont f = p->font(); f.setPixelSize(18); p->setFont(f);
        p->drawText(coverRect, Qt::AlignCenter, type == QLatin1String("video") ? QStringLiteral("▸") : QStringLiteral("♪"));
    }
    p->restore();

    // 时长字符串
    QString durStr = QStringLiteral("--:--");
    if (durMs > 0) {
        const qint64 ts = durMs / 1000;
        durStr = QStringLiteral("%1:%2").arg(ts / 60).arg(ts % 60, 2, 10, QChar('0'));
    }

    // 右侧时长
    QFont fTime = p->font(); fTime.setPixelSize(11); p->setFont(fTime);
    const QFontMetrics fmTime(fTime);
    const int durW = fmTime.horizontalAdvance(durStr) + 8;
    const QRect durRect(r.right() - kPad - durW, r.top(), durW, r.height());
    const bool isDark = opt.palette.color(QPalette::Window).lightness() < 128;
    p->setPen(isDark ? QColor(0x94, 0xa3, 0xb8) : QColor(0x64, 0x74, 0x8b));
    p->drawText(durRect, Qt::AlignRight | Qt::AlignVCenter, durStr);

    // 中间两行文字
    const int textLeft = coverRect.right() + 12;
    const int textRight = durRect.left() - 8;
    const QRect titleRect(textLeft, r.top() + 10, textRight - textLeft, 20);
    const QRect subRect  (textLeft, r.top() + 30, textRight - textLeft, 16);

    // Title
    QFont fTitle = p->font();
    fTitle.setPixelSize(14);
    fTitle.setBold(playing);
    p->setFont(fTitle);
    p->setPen(playing ? QColor(0x7c, 0x3a, 0xed) :
              (isDark ? QColor(0xf1, 0xf5, 0xf9) : QColor(0x0f, 0x17, 0x2a)));
    p->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter,
                fTitle.pointSize() > 0
                    ? QFontMetrics(fTitle).elidedText(title, Qt::ElideRight, titleRect.width())
                    : title);

    // Sub
    QFont fSub = p->font(); fSub.setPixelSize(11); fSub.setBold(false); p->setFont(fSub);
    p->setPen(isDark ? QColor(0x94, 0xa3, 0xb8) : QColor(0x64, 0x74, 0x8b));
    QString subText = artist;
    if (!album.isEmpty() && album != QLatin1String("Unknown")) {
        if (subText.isEmpty()) subText = album;
        else                   subText += QStringLiteral(" · ") + album;
    }
    p->drawText(subRect, Qt::AlignLeft | Qt::AlignVCenter,
                QFontMetrics(fSub).elidedText(subText, Qt::ElideRight, subRect.width()));

    p->restore();
}

} // namespace mp
