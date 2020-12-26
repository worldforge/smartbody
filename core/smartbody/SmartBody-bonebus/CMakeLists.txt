add_library(
        SmartBody-bonebus

        src/sb/SBBoneBusManager.cpp
)

target_include_directories(SmartBody-bonebus PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:bonebus,INCLUDE_DIRECTORIES>
        )

target_link_libraries(SmartBody-bonebus PUBLIC
        SmartBody
        bonebus
        )
