add_library(
        SmartBody-debugger

        src/sb/SBDebuggerClient.cpp
        src/sb/SBDebuggerServer.cpp
        src/sb/SBDebuggerUtility.cpp
)

target_include_directories(SmartBody-debugger PUBLIC
        src
        )


target_link_libraries(SmartBody-debugger PUBLIC
        SmartBody
        SmartBody-renderer-gl
        SmartBody-export
        vhmsg
        )
