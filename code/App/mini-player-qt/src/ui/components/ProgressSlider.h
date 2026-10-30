// ProgressSlider：自绘进度条（紫粉渐变 fill + hover 长高 + 拖动光环 + 时间气泡）。
// 对应书中 §05 Step 5.4（四端同款坑：seeking 标志位）
//              + Web §8.5.8 ProgressBar v2 的桌面端对照实现。
//
// 为什么不用 QSlider？
//   原生 QSlider 的 subcontrol 外观能靠 QSS 美化，但：
//     - 渐变 + 顶部高光 + 悬浮气泡 + hover 变粗 全自绘更稳
//     - 拖动标志位（sliderPressed/Released + 自管 draft）心智更清晰
//   本类继承 QWidget，接口与四端对齐：positionMs/durationMs + seek 信号。
#pragma once

#include <QWidget>

namespace mp {

class ProgressSlider : public QWidget {
    Q_OBJECT
public:
    explicit ProgressSlider(QWidget* parent = nullptr);

    void setDurationMs(qint64 d);
    void setPositionMs(qint64 p);
    void setTall(bool t);
    qint64 positionMs() const { return m_dragging ? m_draftMs : m_positionMs; }

signals:
    void seek(qint64 ms);
    void seekingChanged(bool v);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;

private:
    qreal ratioFromX(int x) const;
    void updateHover(QPoint pos);

    qint64 m_positionMs = 0;
    qint64 m_durationMs = 0;
    qint64 m_draftMs    = 0;
    bool   m_dragging   = false;
    bool   m_hovering   = false;
    QPointF m_hoverPos  { 0, 0 };
    bool   m_tall       = false;
};

} // namespace mp
