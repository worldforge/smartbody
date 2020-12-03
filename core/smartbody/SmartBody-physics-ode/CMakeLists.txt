add_library(
        SmartBody-physics-ode

        src/sbm/ODEPhysicsSim.cpp
       # src/sb/SBVHMsgManager.cpp
)

target_include_directories(SmartBody-physics-ode PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sr,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-physics-ode
        SmartBody
        sr
        )
