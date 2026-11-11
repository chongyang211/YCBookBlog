#!/usr/bin/env bash
# ======================================================================
# native/scripts/multi-platform.sh —— 多平台产物一键构建
#
# 产出（Electron 生态惯例的 platform-arch 目录，每目录一个库文件）：
#   out/multi/darwin-arm64/libmp_native.dylib   ← 本机原生
#   out/multi/darwin-x64/libmp_native.dylib     ← AppleClang 交叉
#   out/multi/linux-x64/libmp_native.so         ← Docker ubuntu:24.04
#   out/multi/win32-x64/mp_native.dll           ← Docker + MinGW-w64
#   out/multi/win32-ia32/mp_native.dll          ← Docker + MinGW-w64 (i686)
#   out/multi/mp-native-<版本>-<平台>.zip        ← 每平台一个 zip
#
# 全部输出统一收敛在 out/ 下（构建中间产物 out/build-multi、最终产物 out/multi）
#
# 用法：
#   bash scripts/multi-platform.sh             # 全平台（需要 Docker）
#   bash scripts/multi-platform.sh --no-docker # 只构建 darwin 两平台
#   bash scripts/multi-platform.sh --no-win    # 跳过 Windows 两平台
# ======================================================================
set -eu

cd "$(dirname "$0")/.."

WITH_DOCKER=1
WITH_WIN=1
for arg in "$@"; do
  case "$arg" in
    --no-docker) WITH_DOCKER=0 ;;
    --no-win)    WITH_WIN=0 ;;
    *) echo "unknown flag: $arg"; exit 1 ;;
  esac
done

VERSION=$(grep -m1 'project(mp_native VERSION' CMakeLists.txt \
  | sed -E 's/.*VERSION ([0-9.]+).*/\1/')
OUT=out/multi
BUILD_ROOT=out/build-multi
DOCKER_IMAGE=ubuntu:24.04

# ---- 清理 ----
rm -rf "$OUT" "$BUILD_ROOT"

# ======================================================================
# ① darwin-arm64（本机原生）
# ======================================================================
echo "──────────────────────────────────────────────"
echo "▶ [1/5] darwin-arm64（本机原生）"
cmake -S . -B "$BUILD_ROOT/darwin-arm64" \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_BUILD_TYPE=Release \
  -DMP_EMBED_OUTPUT=OFF > /dev/null
cmake --build "$BUILD_ROOT/darwin-arm64" -j > /dev/null
mkdir -p "$OUT/darwin-arm64"
cp build-multi/darwin-arm64/libmp_native.dylib "$OUT/darwin-arm64/"
lipo -info "$OUT/darwin-arm64/libmp_native.dylib"

# ======================================================================
# ② darwin-x64（AppleClang 交叉编译）
# ======================================================================
echo "▶ [2/5] darwin-x64（交叉编译）"
cmake -S . -B "$BUILD_ROOT/darwin-x64" \
  -DCMAKE_OSX_ARCHITECTURES=x86_64 \
  -DCMAKE_BUILD_TYPE=Release \
  -DMP_EMBED_OUTPUT=OFF > /dev/null
cmake --build "$BUILD_ROOT/darwin-x64" -j > /dev/null
mkdir -p "$OUT/darwin-x64"
cp build-multi/darwin-x64/libmp_native.dylib "$OUT/darwin-x64/"
lipo -info "$OUT/darwin-x64/libmp_native.dylib"

# ======================================================================
# ③④⑤ linux-x64 + win32-x64 + win32-ia32（Docker 容器内一次装齐工具链）
# ======================================================================
if [ "$WITH_DOCKER" = "1" ]; then
  if ! docker info > /dev/null 2>&1; then
    echo "⚠ Docker daemon 未运行，跳过 linux/windows 平台（CI 上可补齐）"
  else
    echo "▶ [3-5/5] linux-x64 + win32-x64 + win32-ia32（Docker：${DOCKER_IMAGE}）"
    echo "  首次运行会拉取镜像并在容器内安装 g++/cmake/mingw-w64（约 1-2 分钟）"
    echo "  注：使用 docker cp 注入源码（不依赖 bind mount，远程引擎同样可用）"

    CID="mp-native-build-$$"
    docker rm -f "$CID" > /dev/null 2>&1 || true
    docker run -d --name "$CID" "$DOCKER_IMAGE" sleep infinity > /dev/null
    trap 'docker rm -f "$CID" > /dev/null 2>&1 || true' EXIT

    echo "  [docker] 安装工具链（g++ / cmake / mingw-w64）..."
    docker exec "$CID" bash -c '
      set -eu
      export DEBIAN_FRONTEND=noninteractive
      apt-get update -qq
      apt-get install -y -qq --no-install-recommends \
          g++ make cmake file \
          g++-mingw-w64-x86-64 g++-mingw-w64-i686 \
          g++-x86-64-linux-gnu > /dev/null'

    docker exec "$CID" mkdir -p /work/out/multi/linux-x64 /work/out/multi/win32-x64 /work/out/multi/win32-ia32
    for d in include src cmake examples scripts; do
      docker cp "./$d" "$CID:/work/" > /dev/null
    done
    docker cp CMakeLists.txt "$CID:/work/" > /dev/null

    echo "  [docker] 构建 linux-x64 / win32-x64 / win32-ia32 ..."
    docker exec "$CID" bash /work/scripts/container-build.sh

    docker cp "$CID:/work/out/multi/." "$OUT/" > /dev/null
    docker rm -f "$CID" > /dev/null
    trap - EXIT
  fi
else
  echo "▷ 跳过 linux/windows（--no-docker）"
fi

# ======================================================================
# 每平台一个 zip（动态发现 multi/ 下所有非空平台目录）
# ======================================================================
echo "──────────────────────────────────────────────"
for dir in "$OUT"/*/; do
  [ -d "$dir" ] || continue
  [ -n "$(ls -A "$dir" 2>/dev/null)" ] || continue
  p=$(basename "$dir")
  (cd "$dir" && zip -qrg "../mp-native-${VERSION}-${p}.zip" .)
  echo "  ✓ $p → mp-native-${VERSION}-${p}.zip"
done

# ======================================================================
# 最终清点
# ======================================================================
echo
echo "✓ 多平台产物（out/multi/）："
find "$OUT" -type f \( -name "*.dylib" -o -name "*.so" -o -name "*.dll" \) -exec sh -c \
  'printf "  %-42s %s\n" "${0#./}" "$(file -b "$0" | cut -c1-60)"' {} \;
echo
echo "提示：构建的是 Stub 模式（无 TagLib/SysMedia）——全功能版把对应开关"
echo "      加进本脚本的 cmake 参数即可（MP_WITH_TAGLIB / MP_WITH_SYSMEDIA）。"
