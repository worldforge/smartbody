add_library(
        SmartBody-navmesh-bindings-python

        src/SBNavmeshPython.cpp
)

target_include_directories(SmartBody-navmesh-bindings-python PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody-navmesh,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-navmesh-bindings-python PUBLIC
        SmartBody-navmesh
        SmartBody
        sr
        ${PYTHON_LIBRARIES}
        )

target_link_directories(SmartBody-navmesh-bindings-python PUBLIC
        {PYTHON_LIBRARY_DIRS})
