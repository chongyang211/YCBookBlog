cmake_minimum_required(VERSION 3.16)
project(mini_machine CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -g)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 主可执行文件 (REPL: load/run/step/regs/trace/dump)
file(GLOB MM_SRC CONFIGURE_DEPENDS "src/*.cpp")
add_executable(mmach ${MM_SRC})

# ---- 阶段 ① Amdahl ----
add_executable(test_amdahl tests/test_amdahl.cpp)

# ---- 阶段 ② 存储 ----
add_executable(test_storage tests/test_storage.cpp
    src/storage.cpp src/register.cpp src/memory.cpp)

# ---- 阶段 ③ Word/IEEE754 ----
add_executable(test_word tests/test_word.cpp src/word.cpp)

# ---- 阶段 ④ 指令集 ----
add_executable(test_decoder tests/test_decoder.cpp src/instruction.cpp)
add_executable(masm tools/masm.cpp src/instruction.cpp)
add_executable(mdisas tools/mdisas.cpp src/instruction.cpp)

# ---- 阶段 ⑥ 流水线 & 分支预测 ----
add_executable(test_pipeline
    tests/test_pipeline.cpp
    src/pipeline.cpp
    src/predictor.cpp
    src/instruction.cpp)
add_executable(bench_predictor
    tests/bench_predictor.cpp
    src/pipeline.cpp
    src/predictor.cpp
    src/instruction.cpp)

# ---- 阶段 ⑦ 缓存 ----
add_executable(test_cache
    tests/test_cache.cpp
    src/cache.cpp src/memory.cpp src/storage.cpp)
add_executable(bench_matrix tests/bench_matrix.cpp)
target_compile_options(bench_matrix PRIVATE -O2)   # 关键!别用 -O0, 否则倍数看不明显

# ---- 阶段 ⑧ MESI & 伪共享 ----
add_executable(test_mesi
    tests/test_mesi.cpp
    src/mesi.cpp src/memory.cpp src/storage.cpp)
add_executable(bench_falsesharing_sim
    tests/bench_falsesharing_sim.cpp
    src/mesi.cpp src/memory.cpp src/storage.cpp)
add_executable(bench_falsesharing_real tests/bench_falsesharing_real.cpp)
target_compile_options(bench_falsesharing_real PRIVATE -O2 -pthread)
target_link_libraries  (bench_falsesharing_real PRIVATE pthread)

# ---- 阶段 ⑨ 虚拟内存 ----
add_executable(test_vm
    tests/test_vm.cpp
    src/vm.cpp src/mmu.cpp src/tlb.cpp
    src/memory.cpp src/storage.cpp)

# ---- 阶段 ⑩ 总线 & NUMA ----
add_executable(test_bus
    tests/test_bus.cpp
    src/bus.cpp src/memory.cpp src/storage.cpp)
add_executable(bench_numa
    tests/bench_numa.cpp
    src/numa.cpp src/memory.cpp src/storage.cpp)

# ---- 阶段 ⑪ DMA ----
add_executable(bench_dma
    tests/bench_dma.cpp
    src/dma.cpp src/memory.cpp src/storage.cpp)

# ---- 阶段 ⑫ IVT ----
add_executable(test_ivt
    tests/test_ivt.cpp
    src/ivt.cpp src/cpu.cpp src/alu.cpp src/instruction.cpp src/word.cpp
    src/memory.cpp src/storage.cpp)

# ---- 阶段 ⑬ Reactor ----
add_executable(bench_reactor
    tests/bench_reactor.cpp
    src/reactor.cpp)

# ---- 阶段 ⑭ 毕业答辩 ----
add_executable(graduation
    tests/graduation.cpp
    src/machine.cpp
    src/cpu.cpp src/alu.cpp src/instruction.cpp src/word.cpp
    src/memory.cpp src/storage.cpp src/cache.cpp
    src/mmu.cpp src/vm.cpp src/tlb.cpp
    src/bus.cpp src/dma.cpp
    src/ivt.cpp)
