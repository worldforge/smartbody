if (${GOOGLE_PROFILER})
    add_definitions(-DUSE_GOOGLE_PROFILER)
endif ()

include_directories(
        ${PROJECT_SOURCE_DIR}/external/steersuite-1.3/steerlib/include
        ${PROJECT_SOURCE_DIR}/external/steersuite-1.3/pprAI/include
)

add_subdirectory(SmartBody)
add_subdirectory(simplesmartbody)
add_subdirectory(sbgui)

