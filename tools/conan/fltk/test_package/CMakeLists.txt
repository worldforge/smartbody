include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example example.cxx)
target_link_libraries(example ${CONAN_LIBS})
