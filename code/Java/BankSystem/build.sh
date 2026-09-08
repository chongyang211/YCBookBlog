#!/bin/bash
# 编译并运行银行账户管理系统
set -e
rm -rf out && mkdir out
javac -d out -encoding UTF-8 $(find src -name "*.java") \
  && echo "✅ 编译成功" \
  && java -cp out com.bank.cli.Menu
