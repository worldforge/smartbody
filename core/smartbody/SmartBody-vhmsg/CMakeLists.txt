add_library(
        SmartBody-vhmsg

        src/sb/SBVHMsgManager.cpp
)

target_include_directories(SmartBody-vhmsg PUBLIC
        src
        )


target_link_libraries(SmartBody-vhmsg PUBLIC
        SmartBody
        sr
        vhmsg
        vhcl
        )
