if (${GOOGLE_PROFILER})
	add_definitions ( -DUSE_GOOGLE_PROFILER )
endif()

if (${SB_NO_PYTHON})
set (PYTHON_INCLUDE
)
else()
set (PYTHON_INCLUDE
	/usr/include/python2.7
        )
endif()

if (${USE_ASSIMP})
set (ASSIMP_INCLUDE
)
else()
set (ASSIMP_INCLUDE
        ../../lib/assimp-3.1.1/include
        )
endif()

include_directories(
	steersuite-1.3/steerlib/include
	steersuite-1.3/pprAI/include
	steersuite-1.3/external
	${PYTHON_INCLUDE}
	${ASSIMP_INCLUDE}
)

LINK_DIRECTORIES(
)


add_subdirectory( SmartBody )
add_subdirectory( steersuite-1.3 )

add_subdirectory( simplesmartbody)
add_subdirectory( sbgui )
#add_subdirectory( sbgui/external/Pinocchio )

