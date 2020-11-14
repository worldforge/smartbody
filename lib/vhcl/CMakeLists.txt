if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-fPIC)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    FIND_PATH(OPENAL_INCLUDE_DIR al.h)
endif ()


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

if (APPLE OR UNIX)

#    if (BUILD_DYNAMIC)
#        add_library(vhclShared
#                ${SRC}
#                )
#        set_target_properties(vhclShared PROPERTIES OUTPUT_NAME vhcl)
#    endif ()

#    if (BUILD_STATIC)
#        target_link_libraries(
#                vhcl
#                ${OPEN_AL_LIBRARY}
#                ${FREE_ALUT_LIBRARY}
#                ${SND_FILE_LIBRARY}
#        )
#    endif ()
#
#    if (BUILD_DYNAMIC)
#        target_link_libraries(
#                vhclShared
#                ${OPEN_AL_LIBRARY}
#                ${FREE_ALUT_LIBRARY}
#                ${SND_FILE_LIBRARY}
#        )
#    endif ()

#    if (BUILD_DYNAMIC)
#        install(TARGETS vhclShared
#                ARCHIVE DESTINATION lib
#                LIBRARY DESTINATION lib
#                INCLUDES DESTINATION include
#                )
#    endif ()

#    if (BUILD_STATIC)
#        install(TARGETS vhcl
#                ARCHIVE DESTINATION lib
#                LIBRARY DESTINATION lib
#                INCLUDES DESTINATION include
#                )
#    endif ()
#
#    if (BUILD_STATIC OR BUILD_DYNAMIC)
#        install(DIRECTORY include/
#                DESTINATION include
#                )
#    endif ()
endif ()

