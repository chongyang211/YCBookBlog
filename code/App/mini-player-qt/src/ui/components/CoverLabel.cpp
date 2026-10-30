#include "CoverLabel.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QCryptographicHash>
#include <QMouseEvent>

namespace mp {

static int hashInt(const QString& s) {
    const QByteArray b = QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Md5);
    return (static_cast<unsigned char>(b[0]) * 256 + static_cast<unsigned char>(b[1])) % 360;
}

CoverLabel::CoverLabel(QWidget* parent) : QWidget(parent) {
    setMinimumSize(32, 32);
}

void CoverLabel::setIdForFallback(const QString& id) {
    if (m_id == id) return;
    m_id = id;
    update();
}

void CoverLabel::setCoverPath(const QString& path) {
    if (path.isEmpty()) m_pixmap = QPixmap();
    else                m_pixmap.load(path);
    update();
}

void CoverLabel::setCircle(bool c) {
    if (m_circle == c) return;
    m_circle = c;
    update();
}

void CoverLabel::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) emit clicked();
    QWidget::mousePressEvent(e);
}

void CoverLabel::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF rect = this->rect();
    QPainterPath clip;
    const qreal r = m_circle ? rect.width() / 2.0 : 10.0;
    clip.addRoundedRect(rect, r, r);
    p.setClipPath(clip);

    if (!m_pixmap.isNull()) {
        p.drawPixmap(rect, m_pixmap, m_pixmap.rect());
        return;
    }

    // 渐变占位：id hash 稳定取两个色相
    const int h1 = hashInt(m_id.isEmpty() ? QStringLiteral("default") : m_id);
    const int h2 = (h1 + 40 + hashInt(m_id + QLatin1Char('!'))) % 360;
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0, QColor::fromHsl(h1, 180, 150));
    grad.setColorAt(1, QColor::fromHsl(h2, 180, 120));
    p.fillRect(rect, grad);

    // 中心 "♪"
    p.setPen(QColor(255, 255, 255, 220));
    QFont f = p.font();
    f.setPointSizeF(qMax(10.0, rect.width() * 0.32));
    f.setBold(false);
    p.setFont(f);
    p.drawText(rect, Qt::AlignCenter, QStringLiteral("♪"));
}

} // namespace mp
