cmake_minimum_required( VERSION 2.8 )
project( SmartBody )

set( CMAKE_INSTALL_PREFIX "../" )
set( BUILD_OGRE TRUE)
set( GOOGLE_PROFILER FALSE)
set( BUILD_FESTIVAL FALSE)

# uses the Assimp asset importer
set( SB_NO_ASSIMP TRUE)

# uses Python as a scripting language
#set( SB_NO_PYTHON FALSE)

# uses the Virtual Human Message system
#set( SB_NO_VHMSG TRUE)

# uses the bonebus network 
#set( SB_NO_BONEBUS TRUE)

# uses the Open Dynamics Engine (ODE) physics library
#set( SB_NO_ODE_PHYSICS TRUE)


#add_definitions(	-D__linux__	-fPIC -m32 -Ignore -fpermissive -g3 )
#add_definitions(	-g3 -fPIC -fpermissive -std=c++11)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(	-O3 -D__APPLE__ -fPIC -fpermissive )
#add_definitions(	-g3 -D__APPLE__ -fPIC -fpermissive )
else()
add_definitions(	-O3 -D__linux__ -fPIC -fpermissive -std=c++11)
endif()
#add_definitions(	-v -W -Wall -Wextra -pedantic )

set ( ACTIVEMQ_VERSION activemq-cpp-3.9.0)


if (${SB_NO_VHMSG})
add_definitions(-DSB_NO_VHMSG)
set (VHMSG_INCLUDE
)	
else()
set (VHMSG_INCLUDE
	lib/vhmsg/vhmsg-c/include
	include/${ACTIVEMQ_VERSION}
)
endif()

if (${SB_NO_BONEBUS})
add_definitions(-DSB_NO_BONEBUS)
set (BONEBUS_INCLUDE
)	
else()
set (BONEBUS_INCLUDE
	lib/bonebus/include
	)
endif()

if (${SB_NO_PYTHON})
add_definitions(-DSB_NO_PYTHON)
endif()

if (${SB_NO_ODE_PHYSICS})
add_definitions(-DSB_NO_ODE_PHYSICS)
endif()

include_directories(

	${VHMSG_INCLUDE}
	${BONEBUS_INCLUDE}
	lib/vhcl/include
	include
)


link_directories(
)

add_subdirectory( lib )
add_subdirectory( core )


