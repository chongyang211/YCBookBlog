//
// Created by 杨充 on 2025/9/5.
//

#ifndef IPOLLINGSTRATEGY_H
#define IPOLLINGSTRATEGY_H

#include <string>

// ===========================================
// 轮询策略接口和实现
// ===========================================
class IPollingStrategy {
public:
    // 析构函数，有一个具体实现。表示该类可以被示例化。
    virtual ~IPollingStrategy(){}
    //获取下一次轮训的时间间隔。子类必须实现
    virtual long getNextInterval() = 0;
    //重置策略状态
    virtual void reset() = 0;
    //记录任务成功
    virtual void recordSuccess() = 0;
    //记录任务失败
    virtual void recordFailure() = 0;
    //获取策略配置信息
    virtual std::string getConfigInfo() const = 0;
};

#endif //IPOLLINGSTRATEGY_H
