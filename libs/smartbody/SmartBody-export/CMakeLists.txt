add_library(
        SmartBody-export

        src/sbm/SceneExporter.cpp
)

target_include_directories(SmartBody-export PUBLIC
        src
        )


target_link_libraries(SmartBody-export PUBLIC
        CONAN_PKG::zlib
        CONAN_PKG::minizip
        SmartBody-serialize
        SmartBody-steering-steerlib
        SmartBody-renderer-gl
        )
