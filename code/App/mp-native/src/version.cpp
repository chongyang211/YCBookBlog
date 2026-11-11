// native/src/version.cpp —— 版本号 + mp_free
//
// 为什么把 mp_free 放这里？因为它是"全库共用"的 malloc 包装层——
// 本库所有返回堆指针的函数（如 LRC 行数组、封面 PNG 字节）都期望
// JS 侧调用 mp_free 释放。这避免了跨 CRT 堆的灾难（Win 下 MSVC
// 的 CRT 和 Node 的 CRT 可能是不同实例，混用 free() 会直接崩）。
//
// 版本号单一来源：CMakeLists 的 project(VERSION) 编译期注入
// MP_NATIVE_VERSION 字符串字面量，避免头文件/CMake 两处维护漂移。

#include "mp_native.h"
#include <cstdlib>

#ifndef MP_NATIVE_VERSION
#define MP_NATIVE_VERSION "0.0.0-dev"
#endif

extern "C" const char* mp_version(void) {
  // 静态字符串，生命周期永久 —— 调用方（JS/C/任何 FFI）不要 free
  // 编译期字符串拼接："mp_native " + "1.0.0"
  return "mp_native " MP_NATIVE_VERSION;
}

extern "C" void mp_free(void* ptr) {
  if (ptr) std::free(ptr);
}

extern "C" const char* mp_error_string(int32_t code) {
  switch (code) {
    case MP_OK:            return "ok";
    case MP_ERR_ARGS:      return "invalid argument";
    case MP_ERR_IO:        return "io error";
    case MP_ERR_NOT_FOUND: return "not found";
    case MP_ERR_STATE:     return "invalid state";
    case MP_ERR_OOM:       return "out of memory";
    default:               return "unknown error";
  }
}
