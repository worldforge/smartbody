add_library(
        SmartBody-audio

        src/sbm/sbm_audio.cpp
)

target_include_directories(SmartBody-audio PUBLIC
        src
        )


target_link_libraries(SmartBody-audio PUBLIC
        SmartBody
        vhcl-audio
        )
