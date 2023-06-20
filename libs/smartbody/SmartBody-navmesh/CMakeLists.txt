find_package(recastnavigation CONFIG REQUIRED)

add_library(
        SmartBody-navmesh

        src/sb/SBNavigationMesh.cpp
        src/sb/SBNavigationMeshManager.cpp
)

target_include_directories(SmartBody-navmesh PUBLIC
        src
        )


target_link_libraries(SmartBody-navmesh PUBLIC
        recastnavigation::recastnavigation
        SmartBody
        )
