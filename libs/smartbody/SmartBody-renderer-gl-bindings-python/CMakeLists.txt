add_library(
        SmartBody-renderer-gl-bindings-python

        src/SBRendererGLPython.cpp
)

target_include_directories(SmartBody-renderer-gl-bindings-python PUBLIC
        src
        )


target_link_libraries(SmartBody-renderer-gl-bindings-python PUBLIC
        SmartBody-renderer-gl
        SmartBody
        sr
        Python3::Python
        )
