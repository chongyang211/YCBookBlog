#!/usr/bin/env bash
# ======================================================================
# native/scripts/package-native.sh —— 独立分发打包（Linux / macOS）
#
# 用法：
#   bash scripts/package-native.sh                 # Release 纯净版
#   bash scripts/package-native.sh --taglib        # 链 TagLib
#   bash scripts/package-native.sh --sysmedia      # 开系统媒体控制
#   bash scripts/package-native.sh --universal     # macOS universal
#
# 产物：
#   native/dist/include/mp_native.h
#   native/dist/lib/libmp_native.{so|dylib}
#   native/dist/lib/cmake/mp-native/*              # find_package 支持
#   native/dist/mp-native-<版本>-<平台>-<架构>.zip
#
# 兼容性：macOS 自带 bash 3.2（无关联数组/空数组展开陷阱），全部用基础语法。
# ======================================================================
set -eu

cd "$(dirname "$0")/.."     # → native/

# ---- 解析参数 ----
WITH_TAGLIB=0
WITH_SYSMEDIA=0
UNIVERSAL=0
for arg in "$@"; do
  case "$arg" in
    --taglib)   WITH_TAGLIB=1 ;;
    --sysmedia) WITH_SYSMEDIA=1 ;;
    --universal) UNIVERSAL=1 ;;
    *) echo "unknown flag: $arg"; exit 1 ;;
  esac
done

# ---- 版本号：从 CMakeLists 提取 ----
VERSION=$(grep -m1 'project(mp_native VERSION' CMakeLists.txt \
  | sed -E 's/.*VERSION ([0-9.]+).*/\1/')

# ---- 平台与架构 ----
OS=$(uname -s)
ARCH=$(uname -m)
case "$OS" in
  Linux*) PLATFORM="linux" ;;
  Darwin) PLATFORM="macos" ;;
  *) echo "unsupported OS: $OS"; exit 1 ;;
esac
if [ "$UNIVERSAL" = "1" ] && [ "$PLATFORM" = "macos" ]; then
  ARCH="universal"
fi

# ---- 清理 ----
rm -rf build-dist dist

# ---- 配置 ----
cmake -S . -B build-dist \
  -DCMAKE_BUILD_TYPE=Release \
  -DMP_EMBED_OUTPUT=OFF \
  -DMP_BUILD_EXAMPLES=ON \
  -DMP_WITH_TAGLIB=$([ "$WITH_TAGLIB" = "1" ] && echo ON || echo OFF) \
  -DMP_WITH_SYSMEDIA=$([ "$WITH_SYSMEDIA" = "1" ] && echo ON || echo OFF) \
  -DMP_UNIVERSAL_MACOS=$([ "$UNIVERSAL" = "1" ] && echo ON || echo OFF) \
  -DCMAKE_INSTALL_PREFIX=dist

# ---- 构建 + 安装 ----
cmake --build build-dist -j
cmake --install build-dist

# ---- 冒烟 1：纯 C 示例跑通（证明 C ABI 独立于 Node）----
./build-dist/mp_smoke

# ---- 冒烟 2：符号已导出 ----
case "$PLATFORM" in
  linux)
    LIB="dist/lib/libmp_native.so"
    nm -gD "$LIB" | grep -q " T mp_version" || { echo "✗ mp_version not exported"; exit 1; }
    ;;
  macos)
    LIB="dist/lib/libmp_native.dylib"
    nm -gU "$LIB" | grep -q "_mp_version" || { echo "✗ mp_version not exported"; exit 1; }
    ;;
esac

# ---- zip 打包 ----
ZIP_NAME="mp-native-${VERSION}-${PLATFORM}-${ARCH}.zip"
cd dist && zip -rq "../${ZIP_NAME}" . && cd ..

echo
echo "✓ packaged: native/${ZIP_NAME}"
echo "  contents:"
unzip -l "${ZIP_NAME}"
