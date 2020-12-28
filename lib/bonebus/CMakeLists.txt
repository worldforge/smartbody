if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)

add_library(bonebus
        src/bonebus.cpp
        src/bonebus-c.cpp)

target_include_directories(bonebus PUBLIC
        include)

target_link_libraries(bonebus
        vhcl)
