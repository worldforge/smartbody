add_library(
        SmartBody-control

        src/sbm/ControlCommands.cpp
        src/sbm/MiscCommands.cpp
        src/sbm/mcontrol_callbacks.cpp
        src/sbm/text_speech_commands.cpp
)

target_include_directories(SmartBody-control PUBLIC
        src
        )


target_link_libraries(SmartBody-control PUBLIC
        SmartBody-bonebus
        SmartBody-navmesh
        SmartBody-vhmsg
        SmartBody-audio
        SmartBody-bml
        SmartBody-steering-steerlib
        SmartBody
        )
