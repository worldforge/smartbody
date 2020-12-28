add_library(boost-numeric-bindings INTERFACE)

target_include_directories(boost-numeric-bindings INTERFACE
        .)

target_link_libraries(boost-numeric-bindings INTERFACE
        CONAN_PKG::lapack)
