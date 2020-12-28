add_library(
        SmartBody-renderer-gl-bindings-python

        src/SBRendererGLPython.cpp
)

target_include_directories(SmartBody-renderer-gl-bindings-python PUBLIC
        src
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-renderer-gl-bindings-python PUBLIC
        SmartBody-renderer-gl
        SmartBody
        sr
        ${PYTHON_LIBRARIES}
        )
target_link_libraries(SmartBody-renderer-gl-bindings-python PUBLIC
        ${PYTHON_LIBRARY_DIRS})
