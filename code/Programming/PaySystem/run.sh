#!/usr/bin/env bash
# PayFlow 版本运行器
# 用法:
#   ./run.sh v0            # 跑 V0 demo
#   ./run.sh v1            # 跑 V1 demo + 测试
#   ./run.sh test-all      # 跑全部版本的测试
set -e

cd "$(dirname "$0")"

# 保证已编译
if [ ! -d build ] || [ -z "$(ls -A build 2>/dev/null | grep -v classpath.txt)" ]; then
    echo "==> build/ 为空，先执行编译"
    ./build.sh
fi

CP="build"
BASE="com.yc.payflow"

case "$1" in
  v0)
    echo "==> V0 屎山版 Demo（双 11 雪崩事故代码原景重现）"
    java -cp "$CP" ${BASE}.v0shipan.OrderDemo
    ;;
  v1)
    echo "==> V1 特性版 Demo（封装 + 多态）"
    java -cp "$CP" ${BASE}.v1feature.OrderDemo
    echo ""
    echo "==> V1 单元测试"
    java -cp "$CP" ${BASE}.v1feature.WalletTest
    java -cp "$CP" ${BASE}.v1feature.VipPolicyTest
    ;;
  v2)
    echo "==> V2 结构版 Demo（通知双层 + 支付装饰器）"
    java -cp "$CP" ${BASE}.v2structure.OrderDemo
    echo ""
    echo "==> V2 单元测试"
    java -cp "$CP" ${BASE}.v2structure.NotifierTest
    java -cp "$CP" ${BASE}.v2structure.PaymentChannelTest
    ;;
  v3)
    echo "==> V3 组合版 Demo（能力接口 + 字段组合）"
    java -cp "$CP" ${BASE}.v3composition.OrderDemo
    echo ""
    echo "==> V3 单元测试"
    java -cp "$CP" ${BASE}.v3composition.ProductTypeTest
    ;;
  v4)
    echo "==> V4 SOLID 版 Demo（PromotionStep SPI）"
    java -cp "$CP" ${BASE}.v4solid.OrderDemo
    echo ""
    echo "==> V4 单元测试"
    java -cp "$CP" ${BASE}.v4solid.SettlementEngineTest
    ;;
  v5)
    echo "==> V5 重构版 Demo（特征化测试锁定 V0 bug）"
    java -cp "$CP" ${BASE}.v5refactor.CharacterizationTest
    ;;
  v6)
    echo "==> V6 可测版 Demo（Clock 注入秒杀折扣）"
    java -cp "$CP" ${BASE}.v6testable.OrderDemo
    echo ""
    echo "==> V6 秒杀折扣测试（<10ms 断言）"
    java -cp "$CP" ${BASE}.v6testable.FlashSaleClockTest
    ;;
  v7)
    echo "==> V7 DDD 版 Demo（限界上下文 + 领域事件）"
    java -cp "$CP" ${BASE}.v7ddd.OrderDemo
    ;;
  test-all)
    echo "==> 跑全部版本的测试"
    for t in \
      ${BASE}.v1feature.WalletTest \
      ${BASE}.v1feature.VipPolicyTest \
      ${BASE}.v2structure.NotifierTest \
      ${BASE}.v2structure.PaymentChannelTest \
      ${BASE}.v3composition.ProductTypeTest \
      ${BASE}.v4solid.SettlementEngineTest \
      ${BASE}.v5refactor.CharacterizationTest \
      ${BASE}.v6testable.FlashSaleClockTest ; do
      echo ""
      echo "--- $t ---"
      java -cp "$CP" $t
    done
    ;;
  *)
    echo "用法: $0 {v0|v1|v2|v3|v4|v5|v6|v7|test-all}"
    exit 1
    ;;
esac
