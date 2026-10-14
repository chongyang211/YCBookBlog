add_executable(echo_select echo_select.cpp)
target_link_libraries(echo_select )

add_executable(echo_poll echo_poll.cpp)
target_link_libraries(echo_poll )

add_executable(stress_client stress_client.cpp)
target_link_libraries(stress_client PRIVATE pthread)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_executable(echo_epoll_lt echo_epoll_lt.cpp)
    target_link_libraries(echo_epoll_lt )

    add_executable(echo_epoll_et echo_epoll_et.cpp)
    target_link_libraries(echo_epoll_et )
endif()
