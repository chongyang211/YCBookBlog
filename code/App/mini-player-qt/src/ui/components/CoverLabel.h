// CoverLabel：无封面时用 id hash → 渐变色占位；有封面时显示图片。
// 对应书中 §8.5.5（Web 端）的三端对照实现。
#pragma once

#include <QWidget>
#include <QPixmap>
#include <QString>

namespace mp {

class CoverLabel : public QWidget {
    Q_OBJECT
public:
    explicit CoverLabel(QWidget* parent = nullptr);

    void setIdForFallback(const QString& id);
    void setCoverPath(const QString& path);
    /** 让封面变圆（PlayingPage 大图用）*/
    void setCircle(bool c);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;

private:
    QString  m_id;
    QPixmap  m_pixmap;
    bool     m_circle = false;
};

} // namespace mp
