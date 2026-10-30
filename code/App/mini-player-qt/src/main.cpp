// 应用入口。
// 对应书中 §03 阶段① MVP 骨架。
//
// Qt 的"心跳" = QApplication::exec() 的事件循环：
//   - 一切 Widget 事件、信号槽排队、QTimer、QMediaPlayer 回调都走这个 loop
//   - 任何同步耗时操作（如主线程遍历 10 万文件）都会堵死 UI → §04 要移到 QThread

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

#include "data/DbManager.h"
#include "service/ThemeManager.h"
#include "ui/MainWindow.h"
#include "types/MediaItem.h"

int main(int argc, char* argv[]) {
    // 注册自定义类型（Q_DECLARE_METATYPE 不够，还要 qRegisterMetaType 才能跨线程传）
    qRegisterMetaType<mp::MediaItem>("mp::MediaItem");
    qRegisterMetaType<mp::PlaybackState>("mp::PlaybackState");
    qRegisterMetaType<mp::LoopMode>("mp::LoopMode");

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("YCCoding");
    QCoreApplication::setOrganizationDomain("yccoding.com");
    QCoreApplication::setApplicationName("MiniPlayer");
    QCoreApplication::setApplicationVersion("0.1.0");

    // ---------------- 数据库初始化 ----------------
    // Linux  : ~/.local/share/MiniPlayer/library.db
    // macOS  : ~/Library/Application Support/MiniPlayer/library.db
    // Windows: %APPDATA%\MiniPlayer\library.db
    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    const QString dbPath = dataDir + QStringLiteral("/library.db");
    if (!mp::DbManager::instance().init(dbPath)) {
        qWarning("[main] 数据库初始化失败: %s", qUtf8Printable(dbPath));
        // 不致命——内存模式仍可运行
    }

    // ---------------- 主题（深浅色自动跟随系统） ----------------
    mp::ThemeManager::instance().applyInitial();

    // 加载全局 QSS（打包进资源文件）
    QFile qssFile(QStringLiteral(":/resources/style.qss"));
    if (qssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(qssFile.readAll()));
    }

    // ---------------- 主窗口 ----------------
    mp::MainWindow w;
    w.resize(1200, 760);
    w.show();

    return app.exec();
}
