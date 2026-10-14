file(GLOB SRC CONFIGURE_DEPENDS "*.cpp")
add_executable(box_proc ${SRC})
target_link_libraries(box_proc PRIVATE pthread)
