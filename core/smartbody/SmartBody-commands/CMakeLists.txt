add_library(
        SmartBody-speech-remote

        src/sbm/remote_speech.cpp
        src/sbm/local_speech.cpp
)

target_include_directories(SmartBody-speech-remote PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-debugger,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-vhmsg,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sr,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-speech-remote PUBLIC
        SmartBody
        SmartBody-debugger
        SmartBody-vhmsg
        sr
        )
