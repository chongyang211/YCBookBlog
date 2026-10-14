file(GLOB SRC CONFIGURE_DEPENDS "*.cpp")
add_executable(box_vm ${SRC})
target_link_libraries(box_vm )
