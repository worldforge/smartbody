add_library(
        SmartBody-renderer-gl-bindings-python

        src/SBRendererGLPython.cpp
)

target_include_directories(SmartBody-renderer-gl-bindings-python PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody-renderer-gl,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-renderer-gl-bindings-python
        SmartBody-renderer-gl
        SmartBody
        sr
        ${PYTHON_LIBRARIES}
        )
target_link_libraries(SmartBody-renderer-gl-bindings-python PUBLIC
        ${PYTHON_LIBRARY_DIRS})
