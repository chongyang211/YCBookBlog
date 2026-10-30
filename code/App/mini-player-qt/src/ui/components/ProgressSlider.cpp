#include "ProgressSlider.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QToolTip>

namespace mp {

static constexpr int kRailH = 4;
static constexpr int kRailHHover = 8;
static constexpr int kTallRailH = 6;
static constexpr int kTallRailHHover = 10;
static constexpr int kThumbD = 14;

ProgressSlider::ProgressSlider(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(18);
    setAttribute(Qt::WA_Hover);
}

void ProgressSlider::setDurationMs(qint64 d) {
    if (m_durationMs == d) return;
    m_durationMs = qMax<qint64>(0, d);
    update();
}

void ProgressSlider::setPositionMs(qint64 p) {
    if (m_dragging) return;              // 拖动期间不被覆盖（四端同款）
    m_positionMs = qBound<qint64>(0, p, m_durationMs);
    update();
}

void ProgressSlider::setTall(bool t) {
    m_tall = t;
    setMinimumHeight(t ? 22 : 18);
    update();
}

qreal ProgressSlider::ratioFromX(int x) const {
    const int W = width();
    if (W <= 0) return 0.0;
    return qBound(0.0, static_cast<qreal>(x) / static_cast<qreal>(W), 1.0);
}

void ProgressSlider::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal pct = (m_durationMs > 0)
        ? static_cast<qreal>(m_dragging ? m_draftMs : m_positionMs) / static_cast<qreal>(m_durationMs)
        : 0.0;
    const int W = width();
    const int H = height();

    const int railHCur = (m_hovering || m_dragging)
        ? (m_tall ? kTallRailHHover : kRailHHover)
        : (m_tall ? kTallRailH      : kRailH);
    const int railY = (H - railHCur) / 2;

    // 底轨
    QPainterPath railPath;
    railPath.addRoundedRect(QRectF(0, railY, W, railHCur), railHCur / 2.0, railHCur / 2.0);
    p.fillPath(railPath, QColor(0, 0, 0, 24));    // dark theme 下自动够醒目

    // Fill
    const int fillW = static_cast<int>(W * pct + 0.5);
    if (fillW > 0) {
        QPainterPath fillPath;
        fillPath.addRoundedRect(QRectF(0, railY, fillW, railHCur), railHCur / 2.0, railHCur / 2.0);
        QLinearGradient grad(0, 0, W, 0);
        grad.setColorAt(0.0, QColor(0x8b, 0x5c, 0xf6));   // brand-1 紫
        grad.setColorAt(1.0, QColor(0xec, 0x48, 0x99));   // brand-2 粉
        p.fillPath(fillPath, grad);
        // 顶部 1px 高光
        p.fillRect(QRectF(0, railY, fillW, 1), QColor(255, 255, 255, 80));
    }

    // Thumb（hover / dragging / focus 可见）
    if (m_hovering || m_dragging || hasFocus()) {
        const int cx = static_cast<int>(W * pct + 0.5);
        const int cy = H / 2;
        qreal d = kThumbD;
        if (m_dragging) d *= 1.25;
        QRectF thumbR(cx - d / 2.0, cy - d / 2.0, d, d);
        if (m_dragging) {
            // 外圈光环
            p.setBrush(QColor(0x8b, 0x5c, 0xf6, 60));
            p.setPen(Qt::NoPen);
            qreal halo = d + 10;
            p.drawEllipse(QRectF(cx - halo / 2.0, cy - halo / 2.0, halo, halo));
        }
        p.setBrush(Qt::white);
        QPen pen(QColor(0xec, 0x48, 0x99), 2.5);
        p.setPen(pen);
        p.drawEllipse(thumbR);
    }

    // 悬浮时间气泡
    if (m_hovering && !m_dragging && m_durationMs > 0) {
        const qreal r = qBound(0.0, m_hoverPos.x() / W, 1.0);
        const qint64 ms = static_cast<qint64>(r * m_durationMs);
        const qint64 totalSec = ms / 1000;
        const int mm = static_cast<int>(totalSec / 60);
        const int ss = static_cast<int>(totalSec % 60);
        const QString txt = QStringLiteral("%1:%2").arg(mm, 1, 10, QChar('0')).arg(ss, 2, 10, QChar('0'));
        QFont f = p.font(); f.setPixelSize(11); p.setFont(f);
        const QFontMetrics fm(f);
        const QRectF bubble(qBound(0, int(m_hoverPos.x() - 24), W - 48),
                            qMax(0, railY - 24), 48, 20);
        QPainterPath bp;
        bp.addRoundedRect(bubble, 6, 6);
        p.fillPath(bp, QColor(255, 255, 255, 240));
        p.setPen(QColor(0, 0, 0, 180));
        p.drawText(bubble, Qt::AlignCenter, txt);
    }
}

void ProgressSlider::mousePressEvent(QMouseEvent* e) {
    if (m_durationMs <= 0) return;
    m_dragging = true;
    m_draftMs  = static_cast<qint64>(ratioFromX(e->position().x()) * m_durationMs);
    emit seekingChanged(true);
    update();
}

void ProgressSlider::mouseMoveEvent(QMouseEvent* e) {
    m_hoverPos = e->position();
    if (m_dragging) {
        m_draftMs = static_cast<qint64>(ratioFromX(e->position().x()) * m_durationMs);
    }
    update();
}

void ProgressSlider::mouseReleaseEvent(QMouseEvent* e) {
    if (!m_dragging) return;
    const qint64 ms = static_cast<qint64>(ratioFromX(e->position().x()) * m_durationMs);
    m_dragging = false;
    m_positionMs = ms;
    emit seek(ms);
    emit seekingChanged(false);
    update();
}

void ProgressSlider::enterEvent(QEnterEvent* e) {
    m_hovering = true;
    m_hoverPos = e->position();
    update();
}

void ProgressSlider::leaveEvent(QEvent*) {
    m_hovering = false;
    update();
}

void ProgressSlider::keyPressEvent(QKeyEvent* e) {
    if (m_durationMs <= 0) { QWidget::keyPressEvent(e); return; }
    const qint64 step = qMax<qint64>(1000, m_durationMs / 20);
    switch (e->key()) {
        case Qt::Key_Left:  emit seek(qMax<qint64>(0, m_positionMs - step)); return;
        case Qt::Key_Right: emit seek(qMin<qint64>(m_durationMs, m_positionMs + step)); return;
        case Qt::Key_Home:  emit seek(0); return;
        case Qt::Key_End:   emit seek(m_durationMs); return;
        default: break;
    }
    QWidget::keyPressEvent(e);
}

void ProgressSlider::updateHover(QPoint pos) {
    m_hoverPos = pos;
    update();
}

} // namespace mp
