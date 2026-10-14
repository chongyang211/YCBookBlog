if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_executable(box_ns box_ns.cpp)
    target_link_libraries(box_ns )
endif()
