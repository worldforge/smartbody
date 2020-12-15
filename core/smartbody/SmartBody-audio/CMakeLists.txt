add_library(
        SmartBody-audio

        src/sbm/sbm_audio.cpp
)

target_include_directories(SmartBody-audio PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:vhcl,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-audio PUBLIC
        SmartBody
        vhcl
        )
