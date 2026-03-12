cmake_minimum_required(VERSION 4.1)
project(bstk_root_patcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(bstk_root_patcher main.cpp manifest.rc
        fs_utils/fs_utils.cpp
        fs_utils/fs_utils.hpp
        log_utils/log_utils.hpp
        scanner/scanner.cpp
        scanner/scanner.hpp
        log_utils/сonsole_ui.hpp)

if(MINGW)
    target_link_options(bstk_root_patcher PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )

    target_link_libraries(bstk_root_patcher PRIVATE advapi32)
endif()


target_include_directories(bstk_root_patcher PRIVATE
        "./"
)
