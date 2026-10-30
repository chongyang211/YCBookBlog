// MediaListDelegate：自绘封面占位 + 两行文字 + 时长。
// 对应 §04 Step 4.8。
#pragma once

#include <QStyledItemDelegate>

namespace mp {

class MediaListDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit MediaListDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& opt,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& opt,
                   const QModelIndex& index) const override;
};

} // namespace mp
