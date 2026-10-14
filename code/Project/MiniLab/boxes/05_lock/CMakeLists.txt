add_executable(box_lock main.cpp)
target_link_libraries(box_lock PRIVATE pthread)

add_executable(box_banker main_banker.cpp)
target_link_libraries(box_banker )
