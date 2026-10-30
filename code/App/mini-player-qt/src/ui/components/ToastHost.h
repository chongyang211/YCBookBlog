// ToastHost：全局顶部中央 Toast 宿主（对标 Web 端 ToastHost.vue）。
// 用法：ToastHost::instance()->show("已导入 20 条", ToastHost::Success);
#pragma once

#include <QObject>
#include <QWidget>

namespace mp {

class ToastHost : public QWidget {
    Q_OBJECT
public:
    enum Kind { Info, Success, Warn, Error };

    static ToastHost* instance();
    void show(const QString& message, Kind kind = Info, int ms = 2600);

protected:
    void paintEvent(QPaintEvent*) override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    explicit ToastHost(QWidget* parent = nullptr);
    void reposition();

    QString m_message;
    Kind    m_kind = Info;
};

} // namespace mp
