SRCS := $(shell find src -name "*.java")
JAVAC := javac
JAVA := java
FLAGS := -d out -encoding UTF-8 --release 17

.PHONY: all compile run clean demo-race demo-deadlock demo-queue demo-pool demo-batch demo-bench

all: compile

compile:
	$(JAVAC) $(FLAGS) $(SRCS)

run: compile
	$(JAVA) -cp out com.orders.cli.Main

# 各阶段演示（对应教程 5 个造 BUG 现场 + 性能对比）
demo-race: compile
	$(JAVA) -cp out com.orders.cli.RaceDemo

demo-deadlock: compile
	$(JAVA) -cp out com.orders.cli.DeadlockDemo

demo-queue: compile
	$(JAVA) -cp out com.orders.cli.QueueDemo

demo-pool: compile
	$(JAVA) -cp out com.orders.cli.PoolDemo

demo-batch: compile
	$(JAVA) -cp out com.orders.cli.BatchDemo

demo-bench: compile
	$(JAVA) -cp out com.orders.cli.InventoryBenchmark

clean:
	rm -rf out
