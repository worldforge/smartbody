cmake_minimum_required (VERSION 2.8)
project(FestivalDLL)


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   add_definitions(-Wno-deprecated-writable-strings)
   add_definitions(-Wno-self-assign-field)
   add_definitions(-Wno-tautological-undefined-compare)
endif()


include_directories(/usr/local/include)
include_directories(../../../lib/vhcl/include/)
include_directories(../../../lib/festival/festival/src/include/)
include_directories(../../../lib/festival/speech_tools/include/)
include_directories(../../../lib/festival/festival/src/modules/VHDuration/)


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   find_library(AUDIO_UNIT AudioUnit)
   find_library(AUDIOTOOLBOX AudioToolbox)
   find_library(COCOA Cocoa)
   find_library(COREAUDIO CoreAudio)
endif()


# set relative path behavior
cmake_policy(SET CMP0015 NEW)

link_directories(/usr/local/lib)
link_directories(../../../lib/festival/speech_tools/lib)
link_directories(../../../lib/festival/festival/src/lib)


set ( SRC
    FestivalDLL.cpp
)


add_library (FestivalDLL SHARED ${SRC})


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_link_libraries(FestivalDLL ${AUDIO_UNIT})
   target_link_libraries(FestivalDLL ${AUDIOTOOLBOX})
   target_link_libraries(FestivalDLL ${COCOA})
   target_link_libraries(FestivalDLL ${COREAUDIO})
endif()

target_link_libraries(FestivalDLL termcap)
target_link_libraries(FestivalDLL xerces-c)
target_link_libraries(FestivalDLL estbase)
target_link_libraries(FestivalDLL estools)
target_link_libraries(FestivalDLL eststring)
target_link_libraries(FestivalDLL Festival)
target_link_libraries(FestivalDLL vhcl)
