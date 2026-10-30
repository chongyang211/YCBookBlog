// LyricsWidget：QWidget::paintEvent 自绘歌词 + QPropertyAnimation 平滑滚动。
// 对应书中 §07 Step 7.2，与 Android onDraw / Web CSS 滚动同构。
#pragma once

#include <QWidget>
#include "service/LyricsParser.h"

namespace mp {

class LyricsWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal scrollY READ scrollY WRITE setScrollY)
public:
    explicit LyricsWidget(QWidget* parent = nullptr);

    void setLrcText(const QString& text);
    void setPositionMs(qint64 ms);
    void clear();

    qreal scrollY() const { return m_scrollY; }
    void setScrollY(qreal y) { m_scrollY = y; update(); }

signals:
    void lineClicked(qint64 timeMs);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;

private:
    LyricsDocument m_doc;
    int    m_activeIdx = -1;
    qreal  m_scrollY = 0;
    static constexpr int kRowH = 44;
};

} // namespace mp
