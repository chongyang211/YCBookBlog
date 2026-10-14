# 盒 ⑩ · 内核模块 myecho

> 对应文档：§18 盒 ⑩

字符设备 `/dev/myecho`：写什么就能从同一节点读回来。

## 使用

```bash
# 环境
sudo apt install build-essential linux-headers-$(uname -r)

# 编译 & 加载
make
sudo insmod myecho.ko
dmesg | tail -3

# 测试
sudo chmod 666 /dev/myecho
echo "hello" > /dev/myecho
cat /dev/myecho

# 卸载
sudo rmmod myecho
```

或一键：

```bash
make test        # 自动 build + install + echo + cat
```

## ⚠️ 警告

- **只在虚拟机里玩**（Vagrantfile 已配好 Ubuntu 22.04）
- 内核崩了 `dmesg -w` 看调用栈
- 卸载前必须确保 `/dev/myecho` 没有被打开（`lsof` 查）

## 用户态测试

本盒不含用户态测试程序；并发安全验证参见文档 §18 Step 10.3 的 `myecho_test.cpp`。
