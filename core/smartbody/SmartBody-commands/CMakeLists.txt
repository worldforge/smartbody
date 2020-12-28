add_library(
        SmartBody-speech-remote

        src/sbm/remote_speech.cpp
        src/sbm/local_speech.cpp
)

target_include_directories(SmartBody-speech-remote PUBLIC
        src
        )


target_link_libraries(SmartBody-speech-remote PUBLIC
        SmartBody
        SmartBody-debugger
        )
