file(GLOB SRC CONFIGURE_DEPENDS "*.cpp")
add_executable(box_coro ${SRC})
target_link_libraries(box_coro PRIVATE pthread)
# macOS 的 ucontext 需要 _XOPEN_SOURCE，并静默弃用警告
if(APPLE)
    target_compile_definitions(box_coro PRIVATE _XOPEN_SOURCE=600)
    target_compile_options(box_coro PRIVATE -Wno-deprecated-declarations)
endif()
