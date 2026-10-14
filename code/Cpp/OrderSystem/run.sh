#!/usr/bin/env bash
#
# ============================================================================
# 【文件定位】run.sh —— OrderSystem 一键构建 + 运行脚本（C++17）
# ----------------------------------------------------------------------------
# 【设计思路】把"清理→配置→编译→运行"固化为一条命令，并用参数开关
#   切换目标（CLI/压测/Debug）—— 团队协作的"零记忆成本入口"。
# 【bash 技巧清单】set -euo pipefail（防错_accumulate）、heredoc 帮助文本、
#   $# 参数循环 case 分发、nproc/sysctl 跨平台核数探测、exec 替换进程。
# ============================================================================

# 用法:
#   ./run.sh              # 配置 + 编译，然后运行交互式 CLI（先打印线程库 demo）
#   ./run.sh --bench      # 同上，但运行万级并发压测程序
#   ./run.sh --clean      # 删除 build 目录后重新构建并运行
#   ./run.sh --build-only # 只编译，不运行
#   ./run.sh --help       # 显示帮助
#
set -euo pipefail
# 【三连保险】
#   -e：任何命令失败立刻退出（不再执行后续步骤——编译失败就不会运行）；
#   -u：引用未定义变量报错退出（防拼写错误的 $BUID_DIR 之类的坑）；
#   -o pipefail：管道中任何一环失败则整个管道失败（默认只看最后一个命令）

# 切换到脚本所在目录，保证在任何路径下执行都能工作
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# BASH_SOURCE[0]：脚本自身路径（比 $0 可靠 —— 被 source 时 $0 指向调用者）
# cd+pwd 组合把"相对路径"规范化为"绝对路径"（符号链接也解析掉）
cd "$SCRIPT_DIR"

BUILD_DIR="build"
BUILD_TYPE="Release"
TARGET="OrderSystem"      # 默认目标：交互式 CLI（--bench 会切换）
RUN_AFTER_BUILD=1
DO_CLEAN=0

usage() {
  cat <<'EOF'
OrderSystem 一键构建 + 运行脚本（C++17）

用法:
  ./run.sh              # 配置 + 编译，然后运行交互式 CLI（先打印线程库 demo）
  ./run.sh --bench      # 同上，但运行万级并发压测程序
  ./run.sh --clean      # 删除 build 目录后重新构建并运行
  ./run.sh --build-only # 只编译，不运行
  ./run.sh --debug      # 以 Debug 模式构建
  ./run.sh --help       # 显示本帮助
EOF
  exit 0
}
# heredoc 用 <<'EOF'（带引号）：内容不做变量展开/命令替换 —— 帮助文本原样输出

while [[ $# -gt 0 ]]; do
  # $# 是剩余参数个数：逐个处理直到清空（shift 消费）
  case "$1" in
    --bench)      TARGET="bench" ;;          # 切换压测目标
    --clean)      DO_CLEAN=1 ;;              # 标记需要清理
    --build-only) RUN_AFTER_BUILD=0 ;;       # 只编译不运行
    --debug)      BUILD_TYPE="Debug" ;;      # Debug 模式（-O0 -g：调试器友好）
    -h|--help)    usage ;;                   # 显示帮助后退出
    *)            echo "未知参数: $1（用 --help 查看用法）" >&2; exit 1 ;;
    # 未知参数报错到 stderr 并以失败码退出（fail-fast，不静默忽略）
  esac
  shift    # 消费当前参数，指针后移
done

# 选择并行编译的 job 数（跨平台核数探测）
if command -v nproc >/dev/null 2>&1; then
  JOBS="$(nproc)"            # Linux：nproc 报告可用核数
elif command -v sysctl >/dev/null 2>&1; then
  JOBS="$(sysctl -n hw.ncpu)"    # macOS：sysctl 查 hw.ncpu
else
  JOBS="4"                   # 都没有就保守 4 并发
fi
# command -v X >/dev/null 2>&1：探测"命令是否存在"的标准写法
# （输出丢弃、只看退出码 —— which 的可移植替代）

echo "==> 环境检查"
cmake --version >/dev/null || { echo "未找到 cmake，请先安装" >&2; exit 1; }
c++ --version >/dev/null 2>&1 || { echo "未找到 C++ 编译器" >&2; exit 1; }
# 前置依赖显式校验：缺工具立刻报人话，而不是让 cmake 报一屏天书

if [[ "$DO_CLEAN" -eq 1 ]]; then
  echo "==> 清理 $BUILD_DIR 目录"
  rm -rf "$BUILD_DIR"        # 全量重建（怀疑缓存脏了/切换编译器后的救急开关）
fi

echo "==> 配置 CMake (Build type: $BUILD_TYPE)"
# -DCMAKE_EXPORT_COMPILE_COMMANDS=ON：生成 compile_commands.json ——
# clangd/LSP 的"跳转/补全"靠它索引本工程（编辑器体验的关键配置）
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "==> 编译 (-j$JOBS)"
cmake --build "$BUILD_DIR" -j"$JOBS"      # -jN 并行编译（N = 核数）

BIN="$BUILD_DIR/$TARGET"
if [[ ! -x "$BIN" ]]; then
  # -x：文件存在且可执行 —— 编译产物校验（链接失败/名字错误早发现）
  echo "编译产物不存在: $BIN" >&2
  exit 1
fi

if [[ "$RUN_AFTER_BUILD" -eq 1 ]]; then
  echo "==> 运行 $BIN"
  echo
  exec "$BIN"
  # exec 用新进程"替换"当前 shell 进程：省一层进程开销，
  # 且 Ctrl+C 的信号直达目标程序（不再经过包装 shell）
else
  echo "==> 构建完成: $BIN"
fi
