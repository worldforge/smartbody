
add_library(
        SmartBody-renderer-ogre1
        src/OgreRenderScene.cpp
)

target_include_directories(SmartBody-renderer-ogre1 PUBLIC
        src
        )


target_link_libraries(SmartBody-renderer-ogre1 PUBLIC
        CONAN_PKG::ogre
        )
