#include "ToastHost.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QEvent>

namespace mp {

ToastHost* ToastHost::instance() {
    static ToastHost* inst = new ToastHost();
    return inst;
}

ToastHost::ToastHost(QWidget* parent) : QWidget(parent) {
    // 顶层无边框
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    hide();
}

void ToastHost::show(const QString& message, Kind kind, int ms) {
    m_message = message;
    m_kind = kind;
    // 根据字体宽度决定 size
    QFont f = font(); f.setPixelSize(13); setFont(f);
    const QFontMetrics fm(f);
    const int w = qMin(fm.horizontalAdvance(message) + 48, 520);
    const int h = 38;
    resize(w, h);
    reposition();
    QWidget::show();
    update();
    QTimer::singleShot(ms, this, [this]{ this->hide(); });
}

void ToastHost::reposition() {
    QWidget* top = QApplication::activeWindow();
    QRect ref = (top && top->isVisible()) ? top->frameGeometry()
                                          : QApplication::primaryScreen()->availableGeometry();
    const int x = ref.center().x() - width() / 2;
    const int y = ref.top() + 24;
    move(x, y);
}

bool ToastHost::eventFilter(QObject*, QEvent*) {
    return false;
}

void ToastHost::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF r = rect().adjusted(0, 0, 0, 0);
    QPainterPath path;
    path.addRoundedRect(r, 18, 18);

    // 背景（浅色主玻璃效果近似）
    p.fillPath(path, QColor(255, 255, 255, 240));
    p.setPen(QColor(0, 0, 0, 32));
    p.drawPath(path);

    // kind 圆点
    QColor dot;
    switch (m_kind) {
        case Success: dot = QColor(0x22, 0xc5, 0x5e); break;
        case Warn:    dot = QColor(0xf5, 0x9e, 0x0b); break;
        case Error:   dot = QColor(0xef, 0x44, 0x44); break;
        case Info:
        default:      dot = QColor(0x3b, 0x82, 0xf6); break;
    }
    p.setBrush(dot);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QRectF(14, (height() - 10) / 2.0, 10, 10));

    // 文本
    p.setPen(QColor(0x0f, 0x17, 0x2a));
    QFont f = font(); f.setPixelSize(13); p.setFont(f);
    const QRect textRect(32, 0, width() - 46, height());
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_message);
}

} // namespace mp
