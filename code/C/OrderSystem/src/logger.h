/* 线程安全日志（教程阶段⑥）：锁外格式化 + 锁内一次输出 = 行原子 */
#ifndef LOGGER_H
#define LOGGER_H

void logger_info(const char *fmt, ...)
        __attribute__((format(printf, 1, 2)));

#endif
