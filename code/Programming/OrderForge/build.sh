#!/usr/bin/env bash
# OrderForge 一键编译脚本（纯 JDK 17，无需 Maven）
set -e
cd "$(dirname "$0")"
rm -rf build
mkdir -p build

find src -name "*.java" > build/sources.txt

echo "==> 编译全部 Java 源码到 build/ ..."
javac -d build -encoding UTF-8 --release 17 @build/sources.txt

echo "编译完成，产物在 build/"
