#!/usr/bin/env bash
#
# Demo 一键构建 + 运行脚本（C++17）
#
# 用法:
#   ./run.sh              # 配置 + 编译，然后运行线程库用法示例
#   ./run.sh --clean      # 删除 build 目录后重新构建并运行
#   ./run.sh --build-only # 只编译，不运行
#   ./run.sh --debug      # 以 Debug 模式构建
#   ./run.sh --help       # 显示帮助
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DIR="build"
BUILD_TYPE="Release"
TARGET="thread_demo"
RUN_AFTER_BUILD=1
DO_CLEAN=0

usage() {
  cat <<'EOF'
Demo 一键构建 + 运行脚本（C++17）

用法:
  ./run.sh              # 配置 + 编译，然后运行线程库用法示例
  ./run.sh --clean      # 删除 build 目录后重新构建并运行
  ./run.sh --build-only # 只编译，不运行
  ./run.sh --debug      # 以 Debug 模式构建
  ./run.sh --help       # 显示本帮助
EOF
  exit 0
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --clean)      DO_CLEAN=1 ;;
    --build-only) RUN_AFTER_BUILD=0 ;;
    --debug)      BUILD_TYPE="Debug" ;;
    -h|--help)    usage ;;
    *)            echo "未知参数: $1（用 --help 查看用法）" >&2; exit 1 ;;
  esac
  shift
done

if command -v nproc >/dev/null 2>&1; then
  JOBS="$(nproc)"
elif command -v sysctl >/dev/null 2>&1; then
  JOBS="$(sysctl -n hw.ncpu)"
else
  JOBS="4"
fi

echo "==> 环境检查"
cmake --version >/dev/null || { echo "未找到 cmake，请先安装" >&2; exit 1; }
c++ --version >/dev/null 2>&1 || { echo "未找到 C++ 编译器" >&2; exit 1; }

if [[ "$DO_CLEAN" -eq 1 ]]; then
  echo "==> 清理 $BUILD_DIR 目录"
  rm -rf "$BUILD_DIR"
fi

echo "==> 配置 CMake (Build type: $BUILD_TYPE)"
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "==> 编译 (-j$JOBS)"
cmake --build "$BUILD_DIR" -j"$JOBS"

BIN="$BUILD_DIR/$TARGET"
if [[ ! -x "$BIN" ]]; then
  echo "编译产物不存在: $BIN" >&2
  exit 1
fi

if [[ "$RUN_AFTER_BUILD" -eq 1 ]]; then
  echo "==> 运行 $BIN"
  echo
  exec "$BIN"
else
  echo "==> 构建完成: $BIN"
fi
