add_executable(simplesmartbody
        simplesmartbody.cpp
        )

target_link_libraries(simplesmartbody
        SmartBody
        SmartBody-bindings-python
        )
