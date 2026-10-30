// StubAdapter：§09 平台特化未接入前的占位实现。
// 不影响核心功能；上线真实实现时只改 SystemMediaAdapter::create() 工厂。
#pragma once

#include "platform/SystemMediaAdapter.h"

namespace mp {

class StubAdapter : public SystemMediaAdapter {
    Q_OBJECT
public:
    explicit StubAdapter(QObject* parent = nullptr) : SystemMediaAdapter(parent) {}
    void setMetadata(const MediaItem&) override {}
    void setPlaybackState(PlaybackState) override {}
    void setPosition(qint64, qint64) override {}
};

} // namespace mp
