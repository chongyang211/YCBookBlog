// ThemeManager：主题三态（跟随系统 / 浅色 / 深色）。
// 对应书中 §08 阶段⑦ Step 8.2-8.3。
//
// Qt 6.5+ 新增 QStyleHints::colorScheme() 一行跨平台取系统主题，低版本只能走 QPalette 猜。
// 实际 CSS 切换：给 QApplication/根 QWidget 设置 property("theme", "dark"|"light")。
#pragma once

#include <QObject>

namespace mp {

enum class ThemeMode {
    Auto,     // 跟随系统
    Light,
    Dark,
};

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();

    void applyInitial();
    void setMode(ThemeMode m);
    ThemeMode mode() const { return m_mode; }
    bool isDarkActive() const { return m_darkActive; }

signals:
    void modeChanged(ThemeMode m);
    void darkActiveChanged(bool dark);

private slots:
    void onSystemColorSchemeChanged();

private:
    ThemeManager();
    void applyToApp(bool dark);
    bool computeDarkFromSystem() const;

    ThemeMode m_mode = ThemeMode::Auto;
    bool m_darkActive = false;
};

} // namespace mp
