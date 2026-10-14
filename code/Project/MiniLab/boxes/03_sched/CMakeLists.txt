file(GLOB SRC CONFIGURE_DEPENDS "*.cpp")
add_executable(box_sched ${SRC})
target_link_libraries(box_sched )
