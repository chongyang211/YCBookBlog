#include "LyricsWidget.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QMouseEvent>

namespace mp {

LyricsWidget::LyricsWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(240, 240);
}

void LyricsWidget::setLrcText(const QString& text) {
    m_doc = LyricsParser::parse(text);
    m_activeIdx = -1;
    m_scrollY = 0;
    update();
}

void LyricsWidget::clear() {
    m_doc = {};
    m_activeIdx = -1;
    m_scrollY = 0;
    update();
}

void LyricsWidget::setPositionMs(qint64 ms) {
    if (m_doc.lines.isEmpty()) return;
    const int newIdx = LyricsParser::findLineIndex(m_doc.lines, ms - m_doc.offsetMs);
    if (newIdx == m_activeIdx) return;
    m_activeIdx = newIdx;

    // 滚动动画
    const qreal targetY = m_activeIdx * kRowH;
    auto* ani = new QPropertyAnimation(this, "scrollY");
    ani->setDuration(420);
    ani->setStartValue(m_scrollY);
    ani->setEndValue(targetY);
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->start(QAbstractAnimation::DeleteWhenStopped);
}

void LyricsWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (m_doc.lines.isEmpty()) {
        p.setPen(QColor(0x94, 0xa3, 0xb8));
        QFont f = p.font(); f.setPixelSize(14); p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, tr("暂无歌词 · 加载 .lrc 文件以同步显示"));
        return;
    }

    const int H = height();
    const qreal centerY = H / 2.0;

    // 从中心向两侧绘制
    for (int i = 0; i < m_doc.lines.size(); ++i) {
        const int y = static_cast<int>(centerY + i * kRowH - m_scrollY);
        if (y < -kRowH || y > H + kRowH) continue;

        const bool isActive = (i == m_activeIdx);
        const bool isPast   = (i < m_activeIdx);

        QColor color = isActive ? QColor(0x8b, 0x5c, 0xf6)
                                : (isPast ? QColor(0x94, 0xa3, 0xb8, 160)
                                          : QColor(0x94, 0xa3, 0xb8, 220));
        QFont f = p.font();
        f.setPixelSize(isActive ? 20 : 14);
        f.setBold(isActive);
        p.setFont(f);
        p.setPen(color);
        p.drawText(QRect(0, y - kRowH / 2, width(), kRowH),
                   Qt::AlignCenter, m_doc.lines[i].text.isEmpty()
                       ? QStringLiteral("♪") : m_doc.lines[i].text);
    }
}

void LyricsWidget::mousePressEvent(QMouseEvent* e) {
    if (m_doc.lines.isEmpty()) return;
    const qreal centerY = height() / 2.0;
    const qreal relY = e->position().y() - centerY + m_scrollY;
    const int idx = qRound(relY / kRowH);
    if (idx >= 0 && idx < m_doc.lines.size()) {
        emit lineClicked(m_doc.lines[idx].timeMs);
    }
}

} // namespace mp
