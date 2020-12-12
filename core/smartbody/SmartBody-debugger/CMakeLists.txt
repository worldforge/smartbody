add_library(
        SmartBody-debugger

        src/sb/SBDebuggerClient.cpp
        src/sb/SBDebuggerServer.cpp
        src/sb/SBDebuggerUtility.cpp
)

target_include_directories(SmartBody-debugger PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-renderer-gl,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:vhmsg,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sr,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-debugger
        SmartBody
        SmartBody-renderer-gl
        vhmsg
        sr
        )
