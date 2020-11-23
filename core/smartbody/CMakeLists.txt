#Needed for loading media assets
add_definitions(-DSMARTBODY_DATADIR="${CMAKE_INSTALL_FULL_DATADIR}")
#Needed for dynamically loading SteerLib modules
add_definitions(-DSMARTBODY_LIBDIR="${CMAKE_INSTALL_FULL_LIBDIR}")

if (${GOOGLE_PROFILER})
    add_definitions(-DUSE_GOOGLE_PROFILER)
endif ()

include_directories(
        ${PROJECT_SOURCE_DIR}/external/steersuite-1.3/steerlib/include
        ${PROJECT_SOURCE_DIR}/external/steersuite-1.3/pprAI/include
)

add_subdirectory(util)
add_subdirectory(math)
add_subdirectory(sr)
add_subdirectory(SmartBody)
add_subdirectory(SmartBody-bindings-python)
add_subdirectory(SmartBody-bindings-javascript)
add_subdirectory(Smartbody-dll)
add_subdirectory(SmartBody-renderer-gl)
add_subdirectory(SmartBody-debugger)
#add_subdirectory(SmartBody-vhmsg)
add_subdirectory(simplesmartbody)
add_subdirectory(sbgui)

