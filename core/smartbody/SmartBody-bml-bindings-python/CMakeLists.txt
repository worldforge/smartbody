add_library(
        SmartBody-bml-bindings-python

        src/SBBMLProcessorPython
)

target_include_directories(SmartBody-bml-bindings-python PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody-bml,INCLUDE_DIRECTORIES>
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-bml-bindings-python PUBLIC
        SmartBody-bml
        ${PYTHON_LIBRARIES}
        )

target_link_directories(SmartBody-bml-bindings-python PUBLIC
        {PYTHON_LIBRARY_DIRS})
