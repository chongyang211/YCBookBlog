// Entry 结构体、isExpired()、EntryPtr/IndexMap 别名全部是 header-only，
// 因此本文件没有需要单独编译的实现。保留它只是为了与工程目录结构对齐。
// 【工程视角】"空 .cpp + 纯头文件"的组合是刻意设计：
//   ① 目录结构对称（每模块 include/src 成对出现），新人一眼看懂布局；
//   ② 将来 Entry 若加逻辑（如 LRU 淘汰的 touch()），实现位置已预留，
//     不需要动 CMakeLists（编译清单不变）；
//   ③ 代价几乎为零：空翻译单元编译极快，链接器无感。
#include "mkv/entry.hpp"
