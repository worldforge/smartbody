add_library(
        SmartBody-navmesh-bindings-python

        src/SBNavmeshPython.cpp
)

message(STATUS ${PYTHON_INCLUDE_DIRS})

target_include_directories(SmartBody-navmesh-bindings-python PUBLIC
        src
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-navmesh-bindings-python PUBLIC
        ${PYTHON_LIBRARIES}
        SmartBody-navmesh
        )

target_link_directories(SmartBody-navmesh-bindings-python PUBLIC
        {PYTHON_LIBRARY_DIRS})

