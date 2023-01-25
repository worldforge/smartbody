add_library(
        SmartBody-bml-bindings-python

        src/SBBMLProcessorPython.cpp
)

target_include_directories(SmartBody-bml-bindings-python PUBLIC
        src
        )


target_link_libraries(SmartBody-bml-bindings-python PUBLIC
        SmartBody-bml
        Python3::Python
        )
