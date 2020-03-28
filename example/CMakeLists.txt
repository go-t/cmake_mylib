cmake_minimum_required(VERSION 3.0.0)
project(mylib VERSION 0.1.0)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/../install)
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

if(TARGET mylib)
else()
    find_package(mylib HINTS "${CMAKE_INSTALL_PREFIX}")
endif()

add_executable(mylib_example example.cpp)
target_link_libraries(mylib_example mylib)
install(TARGETS mylib_example)
