#!/usr/bin/env bash
# ======================================================================
# native/scripts/container-build.sh —— 在 Linux 容器内构建 linux/windows 产物
# 由 build.cjs 的 multi 命令通过 `docker exec` 调用（docker cp 注入源码，
# 不依赖 bind mount，本地/远程 Docker 引擎通用）。
#
# 容器内依赖（build.cjs multi 已安装）：
#   g++ make cmake file g++-mingw-w64-x86-64 g++-mingw-w64-i686
#   [可选] g++-x86-64-linux-gnu  ← ARM 容器交叉出 x64 Linux 库
#
# 产物写入 /work/out/multi/：
#   linux-<本机架构>/            ← 容器原生架构（x64 或 arm64）
#   linux-x64/                   ← 若交叉编译器可用
#   win32-x64/  win32-ia32/      ← MinGW 交叉
# ======================================================================
set -eu
cd /work

echo "[container] toolchain: $(g++ --version | head -1)"
HOST_ARCH=$(uname -m)
case "$HOST_ARCH" in
  x86_64)  HOST_TAG="x64" ;;
  aarch64) HOST_TAG="arm64" ;;
  *)       HOST_TAG="$HOST_ARCH" ;;
esac
echo "[container] host arch: $HOST_ARCH ($HOST_TAG)"

# ---- build_linux <目录tag> <toolchain文件|空> ----
build_linux() {
  local tag="$1" tc="$2"
  echo "[container] building linux-$tag ..."
  if [ -n "$tc" ]; then
    cmake -S . -B "out/build-multi/linux-$tag" \
      -DCMAKE_TOOLCHAIN_FILE="cmake/$tc" \
      -DCMAKE_BUILD_TYPE=Release -DMP_EMBED_OUTPUT=OFF > /dev/null
  else
    cmake -S . -B "out/build-multi/linux-$tag" \
      -DCMAKE_BUILD_TYPE=Release -DMP_EMBED_OUTPUT=OFF > /dev/null
  fi
  cmake --build "out/build-multi/linux-$tag" -j > /dev/null
  mkdir -p "out/multi/linux-$tag"
  cp "out/build-multi/linux-$tag/libmp_native.so" "out/multi/linux-$tag/"
  file "out/multi/linux-$tag/libmp_native.so"
}

# ---- 1. 容器本机架构的 Linux 库 ----
build_linux "$HOST_TAG" ""

# ---- 2. 交叉 x86_64（ARM 容器且装了交叉编译器时）----
if [ "$HOST_TAG" != "x64" ] && [ -x /usr/bin/x86_64-linux-gnu-g++ ]; then
  build_linux x64 linux-x64-gnu.toolchain.cmake
fi

# ---- 3. win32-x64（MinGW-w64 交叉）----
echo "[container] building win32-x64 ..."
cmake -S . -B out/build-multi/win32-x64 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/windows-x64-mingw.toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DMP_EMBED_OUTPUT=OFF > /dev/null
cmake --build out/build-multi/win32-x64 -j > /dev/null
mkdir -p out/multi/win32-x64
cp out/build-multi/win32-x64/mp_native.dll out/multi/win32-x64/
file out/multi/win32-x64/mp_native.dll

# ---- 4. win32-ia32（MinGW-w64 i686 交叉）----
echo "[container] building win32-ia32 ..."
cmake -S . -B out/build-multi/win32-ia32 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/windows-ia32-mingw.toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DMP_EMBED_OUTPUT=OFF > /dev/null
cmake --build out/build-multi/win32-ia32 -j > /dev/null
mkdir -p out/multi/win32-ia32
cp out/build-multi/win32-ia32/mp_native.dll out/multi/win32-ia32/
file out/multi/win32-ia32/mp_native.dll

echo "[container] all done"
