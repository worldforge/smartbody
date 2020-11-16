add_executable(simplesmartbody
        simplesmartbody.cpp
        )

target_link_libraries(simplesmartbody
        SmartBody
        SmartBody-bindings-python
        pthread
        )

target_include_directories(simplesmartbody PUBLIC
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-bindings-python,INCLUDE_DIRECTORIES>)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(simplesmartbody)
endif ()
