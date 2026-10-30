#include "SystemMediaAdapter.h"
#include "stub/StubAdapter.h"

namespace mp {

SystemMediaAdapter* SystemMediaAdapter::create(QObject* parent) {
    // TODO §09：Linux/macOS/Windows 分别返回真实实现
    //
    // #if defined(Q_OS_LINUX)
    //     return new MprisAdapter(parent);
    // #elif defined(Q_OS_MACOS)
    //     return new MPNowPlayingAdapter(parent);
    // #elif defined(Q_OS_WIN)
    //     return new SmtcAdapter(parent);
    // #endif
    return new StubAdapter(parent);
}

} // namespace mp
