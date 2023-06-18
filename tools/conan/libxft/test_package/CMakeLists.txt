cmake_minimum_required(VERSION 2.8.12)
project(test_package C)

find_package(libxft CONFIG REQUIRED)

add_executable(${PROJECT_NAME} test_package.c)
target_link_libraries(${PROJECT_NAME} libxft::libxft)
