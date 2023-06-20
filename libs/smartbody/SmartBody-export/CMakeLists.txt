find_package(ZLIB CONFIG REQUIRED)
find_package(minizip CONFIG REQUIRED)

add_library(
        SmartBody-export

        src/sbm/SceneExporter.cpp
)

target_include_directories(SmartBody-export PUBLIC
        src
        )


target_link_libraries(SmartBody-export PUBLIC
        ZLIB::ZLIB
        minizip::minizip
        SmartBody-serialize
        SmartBody-steering-steerlib
        SmartBody-renderer-gl
        )
