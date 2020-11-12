
include_directories( 

	../SmartBody/src
	../smartbody-dll/include
)


set( SIMPLESMARTBODY_SRC

	simplesmartbody.cpp 
)

add_executable( simplesmartbody 

	${SIMPLESMARTBODY_SRC}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
find_library (GLEW GLEW)
set( MORE_LIBS
	${GLEW}
)
else()
set( MORE_LIBS
	GLEW
)
endif()

target_link_libraries( simplesmartbody 

	SmartBody
	pthread
	${MORE_LIBS}
)

set( EXEC_LIST

	simplesmartbody
)

set( APP_LIBRARY_BINARY_PATHS

        ${PROJECT_BINARY_DIR}/core/smartbody/sbgui/bin
)
	
set( APP_LIBRARY_SOURCE_PATHS

        ${PROJECT_SOURCE_DIR}/core/smartbody/sbgui/bin
)
	


set_target_properties(

        ${EXEC_LIST}

	PROPERTIES

	## link with install tree:
	INSTALL_RPATH                   "${APP_LIBRARY_SOURCE_PATHS}"
	BUILD_WITH_INSTALL_RPATH        TRUE
)

install( 
	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION core/smartbody/sbgui/bin
)
