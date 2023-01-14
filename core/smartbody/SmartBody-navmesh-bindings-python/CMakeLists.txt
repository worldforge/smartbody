add_library(
        SmartBody-navmesh-bindings-python

        src/SBNavmeshPython.cpp
)

target_include_directories(SmartBody-navmesh-bindings-python PUBLIC
        src
        )


target_link_libraries(SmartBody-navmesh-bindings-python PUBLIC
        CONAN_PKG::cpython
        SmartBody-navmesh
        )

