add_library(
        SmartBody-export

        src/sbm/SceneExporter.cpp
)

target_include_directories(SmartBody-export PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-serialize,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-steering-steerlib,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-renderer-gl,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-export PUBLIC
        SmartBody
        SmartBody-serialize
        SmartBody-steering-steerlib
        SmartBody-renderer-gl
        )
