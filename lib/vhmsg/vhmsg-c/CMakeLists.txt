cmake_minimum_required (VERSION 2.8)
project(vhmsg)


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()


include_directories(include)
include_directories(../../vhcl/include)

if (APPLE)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-value")
endif ()

if (MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)


set ( SRC
   src/HttpUtility.cpp
   src/vhmsg.cpp
   src/vhmsg-tt.cpp
)

add_library( vhmsg
   STATIC 
   ${SRC}
)

if(APPLE OR UNIX) 

	FIND_LIBRARY(VHCL_LIBRARY vhcl HINTS ${CMAKE_INSTALL_PREFIX}/lib )
	MESSAGE( STATUS "VHCL_LIBRARY:         " ${VHCL_LIBRARY} )

	if ( VHCL_LIBRARY )
	
	else ()
		set(VHCL_LIBRARY "vhcl")
	endif ()
	
	MESSAGE( STATUS "VHCL_LIBRARY:         " ${VHCL_LIBRARY} )

    FIND_LIBRARY(ACTIVEMQ_LIBRARY activemq-cpp HINTS ${CMAKE_SOURCE_DIR}/lib )

	add_library( vhmsgShared
		SHARED 
		${SRC}
	)
	set_target_properties(vhmsgShared PROPERTIES OUTPUT_NAME vhmsg)

	target_link_libraries(vhmsg
		${VHCL_LIBRARY}
		${ACTIVEMQ_LIBRARY}
	)

	target_link_libraries(vhmsgShared 
		${VHCL_LIBRARY}
		${ACTIVEMQ_LIBRARY}
	)

	install (TARGETS vhmsg vhmsgShared
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		INCLUDES DESTINATION include
	)

	install (DIRECTORY include/ 
		DESTINATION include
	)
endif()
