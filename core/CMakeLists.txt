if (WIN32)
    add_compile_definitions(SB_EXPORTS)
    add_compile_definitions(_WIN32_WINNT=0x0601) #target Windows 7
    # On Windows there are some headers that define "min" and "max" as preprocessor macros, which messes up compilation. This tells them to stop.
    add_compile_definitions(NOMINMAX)

    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(VC_EXTRA_LEAN)
    add_compile_definitions(WIN32_EXTRA_LEAN)
endif ()

add_subdirectory(vhcl)
add_subdirectory(bonebus)
add_subdirectory(vhmsg/vhmsg-c)
add_subdirectory(smartbody)

#Got linking errors in WIN32, disabled until they've been fixed.
if (NOT WIN32)
    add_subdirectory(ogredemo)
endif()

# not complete yet
#add_subdirectory( ogre-viewer )
#add_subdirectory( FestivalRelay )

