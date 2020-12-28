add_library(
        SmartBody-bonebus

        src/sb/SBBoneBusManager.cpp
)

target_include_directories(SmartBody-bonebus PUBLIC
        src
        )

target_link_libraries(SmartBody-bonebus PUBLIC
        SmartBody
        bonebus
        )
