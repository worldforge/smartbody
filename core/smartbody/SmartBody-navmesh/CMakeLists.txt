add_library(
        SmartBody-navmesh

        src/sb/SBNavigationMesh.cpp
        src/sb/SBNavigationMeshManager.cpp
)

target_include_directories(SmartBody-navmesh PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sr,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-navmesh
        SmartBody
        sr
        )
