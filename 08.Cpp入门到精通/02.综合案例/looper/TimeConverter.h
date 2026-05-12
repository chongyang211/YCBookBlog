//
// Created by 杨充 on 2025/9/5.
//

#ifndef TIMECONVERTER_H
#define TIMECONVERTER_H
#include <string>
#include "TimeUnit.h"

class TimeConverter {
public:
    static long toNanoseconds(long duration, TimeUnit unit);
    static std::string unitToString(TimeUnit unit);
};

#endif //TIMECONVERTER_H
