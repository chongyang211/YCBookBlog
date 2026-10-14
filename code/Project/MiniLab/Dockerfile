# Dockerfile for MiniLab
# 为 macOS/Windows 提供 Linux 容器化环境
#
# 用法：
#   docker build -t minilab .
#   docker run --rm -it --privileged -v $PWD:/work minilab bash
#
#   注意：
#   - 盒 ⑩（内核模块）在容器里跑不了（需要宿主内核头并 insmod）
#   - 盒 ⑪⑫ 需要 --privileged
#
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    python3 python3-pip \
    stress-ng iproute2 lsof \
    curl ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir pandas matplotlib

WORKDIR /work

CMD ["bash"]
