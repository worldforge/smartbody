add_library(
        SmartBody-steering-steerlib-bml

        src/sb/SteeringBml.cpp
        src/bml/bml_locomotion.cpp
)

target_include_directories(SmartBody-steering-steerlib-bml PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody-bml,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-steering-steerlib,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-steering-steerlib-bml PUBLIC
        SmartBody-bml
        SmartBody-steering-steerlib
        )
