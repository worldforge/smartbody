include_directories(include)
include_directories(../vhcl/include)

if (MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)

add_library( bonebus
        src/bonebus.cpp
        src/bonebus-c.cpp)
