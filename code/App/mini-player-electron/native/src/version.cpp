// native/src/version.cpp —— 版本号 + mp_free
//
// 为什么把 mp_free 放这里？因为它是"全库共用"的 malloc 包装层——
// 本库所有返回堆指针的函数（如 LRC 行数组、封面 PNG 字节）都期望
// JS 侧调用 mp_free 释放。这避免了跨 CRT 堆的灾难（Win 下 MSVC
// 的 CRT 和 Node 的 CRT 可能是不同实例，混用 free() 会直接崩）。

#include "mp_native.h"
#include <cstdlib>

extern "C" const char* mp_version(void) {
  // 静态字符串，生命周期永久 —— JS 侧 koffi 收到后拷贝一份就安全
  return "mp_native 0.1.0";
}

extern "C" void mp_free(void* ptr) {
  if (ptr) std::free(ptr);
}
