add_library(
        SmartBody-steering-steerlib

        src/sb/SBSteerAgent.cpp
        src/sb/SBSteerManager.cpp

        src/sbm/PPRAISteeringAgent.cpp
        src/sbm/SteerSuiteEnginerDriver.cpp
)

target_include_directories(SmartBody-steering-steerlib PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:steerlib,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:pprAI,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-steering-steerlib PUBLIC
        SmartBody
        steerlib
        pprAI
        )
