add_library(
        SmartBody-navmesh-bindings-python

        src/SBNavmeshPython.cpp
)

target_include_directories(SmartBody-navmesh-bindings-python PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody-navmesh,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-navmesh-bindings-python
        SmartBody-navmesh
        SmartBody
        sr
        )
