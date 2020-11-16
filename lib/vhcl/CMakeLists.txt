include_directories(include)


# only for vhcl_crash.cpp
include_directories(src/nocturnal/Debug)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/EHa)
endif (MSVC)


set(SRC
        src/Pow2Assert.cpp
        src/vhcl_audio.cpp
        src/vhcl_crash.cpp
        src/vhcl_log.cpp
        src/vhcl_math.cpp
        src/vhcl_memory.cpp
        src/vhcl_socket.cpp
        src/vhcl_string.cpp
        src/vhcl_timer.cpp
        )


if (MSVC)
    set(SRC
            ${SRC}
            src/nocturnal/Debug/Exception.cpp
            src/nocturnal/Debug/Mail.cpp
            src/nocturnal/Debug/Memory.cpp
            src/nocturnal/Debug/Symbols.cpp
            )
endif (MSVC)

add_library(vhcl
        ${SRC}
        )


