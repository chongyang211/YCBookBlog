#!/usr/bin/env bash
# Mini-KV 一键编译（纯 javac，无网络依赖）
set -e
cd "$(dirname "$0")"

rm -rf out
mkdir -p out
javac -d out -encoding UTF-8 --release 17 $(find src -name "*.java")
echo "编译完成 → out/"
