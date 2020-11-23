add_library(
        SmartBody-vhmsg

       # src/sb/SBVHMsgManager.cpp
)

target_include_directories(SmartBody-vhmsg PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-debugger,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sr,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-vhmsg
        SmartBody
        SmartBody-debugger
        sr
        )
