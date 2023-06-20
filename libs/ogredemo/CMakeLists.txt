find_package(ogre CONFIG REQUIRED)

add_executable(ogredemo
        src/OgreDemo.cpp
        src/ParserCOLLADAFast.cpp
        src/SBAssetHandlerCOLLADA.cpp
        src/BasicCharacter.cpp
        src/OgreSmartBodyListener.cpp
        src/OgreSmartBody.cpp
        )

#Needed for loading media assets
target_compile_definitions(ogredemo PRIVATE -DSMARTBODY_DATADIR="${CMAKE_INSTALL_FULL_DATADIR}")
#Needed for dynamically loading SteerLib modules
target_compile_definitions(ogredemo PRIVATE -DSMARTBODY_LIBDIR="${CMAKE_INSTALL_FULL_LIBDIR}")

target_link_libraries(ogredemo
        SmartBody-physics-ode
        SmartBody-control
        SmartBody-navmesh
        SmartBody-serialize
#        SmartBody-steering-steerlib-bml
#        SmartBody-steering-steerlib
        SmartBody-bml
        SmartBody-util
        SmartBody
        ogre::ogre
        )

install(TARGETS ogredemo DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

