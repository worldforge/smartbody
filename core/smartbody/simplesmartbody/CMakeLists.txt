add_executable(simplesmartbody
        simplesmartbody.cpp
        )

target_link_libraries(simplesmartbody
        SmartBody
        SmartBody-bindings-python
        pthread
        )


if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(simplesmartbody)
endif ()
