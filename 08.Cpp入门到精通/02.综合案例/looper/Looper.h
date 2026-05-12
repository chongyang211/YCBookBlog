//
// Created by 杨充 on 2025/9/2.
//

#ifndef LOOPER_H
#define LOOPER_H

#include <random>
#include <sstream>

#include "IPollingStrategy.h"
#include "Poller.h"
#include "TaskStatistics.h"
#include "TimeConverter.h"
#include "TimeUnit.h"


/**
 * @brief 固定间隔轮询策略
 */
class FixedIntervalStrategy : public IPollingStrategy {
public:
    FixedIntervalStrategy(long interval, TimeUnit unit)
        : interval_ns(TimeConverter::toNanoseconds(interval, unit)) {}

    virtual long getNextInterval() override { return interval_ns; }
    virtual void reset() override {}
    virtual void recordSuccess() override {}
    virtual void recordFailure() override {}

    virtual std::string getConfigInfo() const override {
        std::ostringstream oss;
        oss << "FixedInterval: " << interval_ns << " ns";
        return oss.str();
    }

private:
    const long interval_ns;
};

/**
 * @brief 指数退避策略
 */
class ExponentialBackoffStrategy : public IPollingStrategy {
public:
    ExponentialBackoffStrategy(long initial, long max, TimeUnit unit)
        : initial_ns(TimeConverter::toNanoseconds(initial, unit)),
          max_ns(TimeConverter::toNanoseconds(max, unit)),
          current_ns(initial_ns) {}

    virtual long getNextInterval() override {
        long result = current_ns;
        current_ns = std::min(current_ns * 2, max_ns);
        return result;
    }

    virtual void reset() override { current_ns = initial_ns; }

    virtual void recordSuccess() override { reset(); }

    virtual void recordFailure() override {
        // 失败时自动增加间隔 (在getNextInterval中实现)
    }

    virtual std::string getConfigInfo() const override {
        std::ostringstream oss;
        oss << "ExponentialBackoff: Initial=" << initial_ns << "ns, Max=" << max_ns << "ns";
        return oss.str();
    }

private:
    const long initial_ns;
    const long max_ns;
    long current_ns;
};

/**
 * @brief 随机间隔策略
 */
class RandomIntervalStrategy : public IPollingStrategy {
public:
    RandomIntervalStrategy(long min, long max, TimeUnit unit)
        : min_ns(TimeConverter::toNanoseconds(min, unit)),
          max_ns(TimeConverter::toNanoseconds(max, unit)),
          engine(std::random_device()()) {}

    virtual long getNextInterval() override {
        std::uniform_int_distribution<long> dist(min_ns, max_ns);
        return dist(engine);
    }

    virtual void reset() override {}
    virtual void recordSuccess() override {}
    virtual void recordFailure() override {}

    virtual std::string getConfigInfo() const override {
        std::ostringstream oss;
        oss << "RandomInterval: Min=" << min_ns << "ns, Max=" << max_ns << "ns";
        return oss.str();
    }

private:
    const long min_ns;
    const long max_ns;
    std::default_random_engine engine;
};




#endif //LOOPER_H
