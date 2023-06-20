find_package(alut CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/EHa)
endif (MSVC)


set(SRC
        src/Pow2Assert.cpp
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

if (MSVC)
        target_compile_definitions(vhcl PRIVATE _CRT_SECURE_NO_DEPRECATE)
        target_compile_definitions(vhcl PRIVATE _USRDLL)
        target_compile_definitions(vhcl PRIVATE DEBUGDLL_EXPORTS)
endif (MSVC)

target_include_directories(vhcl PUBLIC
        include
        )


# only for vhcl_crash.cpp
target_include_directories(vhcl PRIVATE
        src/nocturnal/Debug)

target_link_libraries(vhcl PUBLIC)

add_library(vhcl-audio
        src/vhcl_audio.cpp
        )

target_include_directories(vhcl-audio PUBLIC
        include
        )

target_link_libraries(vhcl-audio PUBLIC
        vhcl
        alut::alut
        SndFile::sndfile)
