add_library(
        SmartBody-control

        src/sbm/ControlCommands.cpp
        src/sbm/MiscCommands.cpp
        src/sbm/mcontrol_callbacks.cpp
        src/sbm/text_speech_commands.cpp
)

target_include_directories(SmartBody-control PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-navmesh,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-vhmsg,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-control
        SmartBody-navmesh
        SmartBody-vhmsg
        SmartBody
        )
