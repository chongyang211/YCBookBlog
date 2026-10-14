add_executable(bench_pipe bench_pipe.cpp)
target_link_libraries(bench_pipe )

add_executable(bench_uds bench_uds.cpp)
target_link_libraries(bench_uds )

add_executable(bench_shm bench_shm.cpp)
target_link_libraries(bench_shm PRIVATE pthread)
if(NOT APPLE)
    target_link_libraries(bench_shm PRIVATE rt)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_executable(bench_mq bench_mq.cpp)
    target_link_libraries(bench_mq PRIVATE rt)
endif()
