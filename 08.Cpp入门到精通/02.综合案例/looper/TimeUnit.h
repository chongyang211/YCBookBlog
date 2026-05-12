//
// Created by 杨充 on 2025/9/5.
//

#ifndef TIMEUNIT_H
#define TIMEUNIT_H

// ===========================================
// 时间单位定义和转换工具
// ===========================================
// 枚举值的作用域限定在枚举类型内部。
enum class TimeUnit {
    NANOSECONDS,
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    MINUTES
};

// 非限定作用域枚举。枚举值的作用域是全局的，可能会与其他枚举类型或全局变量冲突。
enum TimeUnitOnly {
    NANOSECONDS,
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    MINUTES
};


#endif //TIMEUNIT_H
