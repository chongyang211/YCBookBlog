#include "ThemeManager.h"

#include <QApplication>
#include <QGuiApplication>
#include <QStyleHints>
#include <QSettings>
#include <QWidget>
#include <QStyle>

namespace mp {

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager() = default;

void ThemeManager::applyInitial() {
    // 从 QSettings 读上次模式（和 Dexie 分开，首屏不异步）
    QSettings s;
    const QString v = s.value(QStringLiteral("theme"), QStringLiteral("auto")).toString();
    if      (v == QStringLiteral("light")) m_mode = ThemeMode::Light;
    else if (v == QStringLiteral("dark"))  m_mode = ThemeMode::Dark;
    else                                   m_mode = ThemeMode::Auto;

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Qt 6.5+ 原生跨平台监听
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, &ThemeManager::onSystemColorSchemeChanged);
#endif

    m_darkActive = (m_mode == ThemeMode::Dark) ||
                   (m_mode == ThemeMode::Auto && computeDarkFromSystem());
    applyToApp(m_darkActive);
}

void ThemeManager::setMode(ThemeMode m) {
    m_mode = m;
    QSettings s;
    const QString v = (m == ThemeMode::Light) ? QStringLiteral("light") :
                      (m == ThemeMode::Dark)  ? QStringLiteral("dark")  : QStringLiteral("auto");
    s.setValue(QStringLiteral("theme"), v);
    emit modeChanged(m);

    const bool dark = (m == ThemeMode::Dark) ||
                      (m == ThemeMode::Auto && computeDarkFromSystem());
    if (dark != m_darkActive) {
        m_darkActive = dark;
        applyToApp(dark);
        emit darkActiveChanged(dark);
    }
}

void ThemeManager::onSystemColorSchemeChanged() {
    if (m_mode != ThemeMode::Auto) return;
    const bool dark = computeDarkFromSystem();
    if (dark == m_darkActive) return;
    m_darkActive = dark;
    applyToApp(dark);
    emit darkActiveChanged(dark);
}

bool ThemeManager::computeDarkFromSystem() const {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    // 兜底：看 window text 和 base 的亮度（粗糙但够用）
    const QColor win  = QGuiApplication::palette().color(QPalette::Window);
    return win.lightness() < 128;
#endif
}

void ThemeManager::applyToApp(bool dark) {
    // QSS 选择器通过根 widget 的 "theme" 自定义属性匹配
    // 这里给 qApp 自身也带上一份，让 QStyleSheet 的 cascade 正确
    for (QWidget* w : qApp->topLevelWidgets()) {
        w->setProperty("theme", dark ? "dark" : "light");
        // 让 style 重新计算
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    }
}

} // namespace mp
