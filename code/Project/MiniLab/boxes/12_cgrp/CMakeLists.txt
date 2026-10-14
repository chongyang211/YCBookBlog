if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_executable(mycgrp mycgrp.cpp)
    target_link_libraries(mycgrp )
endif()
