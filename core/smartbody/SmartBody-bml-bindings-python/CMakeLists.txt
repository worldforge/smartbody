add_library(
        SmartBody-bml-bindings-python

        src/SBBMLProcessorPython
)

target_include_directories(SmartBody-bml-bindings-python PUBLIC
        src
        )


target_link_libraries(SmartBody-bml-bindings-python PUBLIC
        SmartBody-bml
        CONAN_PKG::cpython
        )
