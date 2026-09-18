#!/usr/bin/env bash
# OrderForge 版本运行器
# 用法: ./run.sh {v0|v1|v2|v3|v4|metrics|report|test-all}
set -e
cd "$(dirname "$0")"

if [ ! -d build ] || [ -z "$(ls -A build 2>/dev/null | grep -v sources.txt)" ]; then
    echo "==> build/ 为空，先执行编译"
    ./build.sh
fi

CP="build"
BASE="com.yc.orderforge"

case "$1" in
  v0)
    echo "==> V0 上帝类 Demo（六职责 400 行，3 个真实 bug 现场）"
    java -cp "$CP" ${BASE}.v0god.OrderDemo
    ;;
  v1)
    echo "==> V1 SRP 手术 Demo（六件事拆六个类 + 库存补偿）"
    java -cp "$CP" ${BASE}.v1srp.OrderDemo
    echo ""
    echo "==> V1 单元测试"
    java -cp "$CP" ${BASE}.v1srp.V1SrpTest
    ;;
  v2)
    echo "==> V2 OCP 手术 Demo（折扣 if-else → 策略链，修复 BUG 1）"
    java -cp "$CP" ${BASE}.v2ocp.OrderDemo
    echo ""
    echo "==> V2 单元测试"
    java -cp "$CP" ${BASE}.v2ocp.V2OcpTest
    ;;
  v3)
    echo "==> V3 LSP+ISP 手术 Demo（支付契约 + 角色接口）"
    java -cp "$CP" ${BASE}.v3lspisp.OrderDemo
    echo ""
    echo "==> V3 单元测试"
    java -cp "$CP" ${BASE}.v3lspisp.PaymentContractTest
    java -cp "$CP" ${BASE}.v3lspisp.RoleInterfaceTest
    ;;
  v4)
    echo "==> V4 DIP+LOD 手术 Demo（依赖注入 + 门面，修复 BUG 2/3）"
    java -cp "$CP" ${BASE}.v4diplod.OrderDemo
    echo ""
    echo "==> V4 单元测试"
    java -cp "$CP" ${BASE}.v4diplod.V4DipLodTest
    ;;
  metrics)
    echo "==> 圈复杂度 + 坏味道扫描（V0 体检报告）"
    java -cp "$CP" ${BASE}.metrics.MetricsReport
    ;;
  report)
    echo "==> 终局验收：度量前后对比 + 三改实验"
    java -cp "$CP" ${BASE}.report.FinalReport
    ;;
  test-all)
    echo "==> 跑全部版本的测试"
    for t in \
      ${BASE}.v1srp.V1SrpTest \
      ${BASE}.v2ocp.V2OcpTest \
      ${BASE}.v3lspisp.PaymentContractTest \
      ${BASE}.v3lspisp.RoleInterfaceTest \
      ${BASE}.v4diplod.V4DipLodTest ; do
      echo ""
      echo "--- $t ---"
      java -cp "$CP" $t
    done
    ;;
  *)
    echo "用法: $0 {v0|v1|v2|v3|v4|metrics|report|test-all}"
    exit 1
    ;;
esac
