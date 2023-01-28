add_library(
        SmartBody-navmesh-bindings-python

        src/SBNavmeshPython.cpp
)

target_include_directories(SmartBody-navmesh-bindings-python PUBLIC
        src
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-navmesh-bindings-python PUBLIC
        Python3::Python
        SmartBody-navmesh
        )

target_link_directories(SmartBody-navmesh-bindings-python PUBLIC
        {PYTHON_LIBRARY_DIRS})

