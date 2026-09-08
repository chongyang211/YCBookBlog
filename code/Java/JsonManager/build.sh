#!/bin/bash
# 编译并运行 Mini JSON DB
set -e
rm -rf out && mkdir out
javac -d out -encoding UTF-8 --release 17 $(find src -name "*.java") \
  && echo "✅ 编译成功" \
  && java -cp out com.minijson.cli.Main
