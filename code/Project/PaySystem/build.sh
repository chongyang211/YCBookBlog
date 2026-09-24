#!/usr/bin/env bash
# PayFlow 一键编译脚本（无需 Maven，纯 JDK）
set -e

cd "$(dirname "$0")"
rm -rf build
mkdir -p build

# 使用 find 收集全部 .java 源码
find src -name "*.java" > build/sources.txt

echo "==> 编译全部 Java 源码到 build/ ..."
javac -d build -encoding UTF-8 --release 17 @build/sources.txt

# 生成一份便于运行的类路径
echo "build" > build/classpath.txt

echo "编译完成，产物在 build/"
