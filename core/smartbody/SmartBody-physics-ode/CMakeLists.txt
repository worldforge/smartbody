add_library(
        SmartBody-physics-ode

        src/sbm/ODEPhysicsSim.cpp
       # src/sb/SBVHMsgManager.cpp
)

target_include_directories(SmartBody-physics-ode PUBLIC
        src
        )


target_link_libraries(SmartBody-physics-ode PUBLIC
        ode
        SmartBody
        sr
        )
