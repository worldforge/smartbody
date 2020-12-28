add_library(SmartBody-c-dll SHARED
        src/smartbody-c-dll.cpp
)

target_include_directories(SmartBody-c-dll PUBLIC
        src
)

target_link_libraries(SmartBody-c-dll
        SmartBody-renderer-gl
        SmartBody-debugger
        SmartBody-bindings-python
        SmartBody-control
        )
