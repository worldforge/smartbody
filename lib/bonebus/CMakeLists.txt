cmake_minimum_required (VERSION 2.8)
project(bonebus)


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()


include_directories(include)
include_directories(../vhcl/include)


if (MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)


set ( SRC
   src/bonebus.cpp
   src/bonebus-c.cpp
)


add_library( bonebus
   STATIC 
   ${SRC}
)
