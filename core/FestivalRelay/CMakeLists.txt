include_directories( ../../lib/festival/festival/src/include )
include_directories( ../../lib/festival/festival/src/modules/VHDuration)
include_directories( ../../lib/festival/speech_tools/include )

find_library(FESTIVALLIB Festival ../../lib/festival/festival/src/lib )
find_library(ESTTOOLS estools ../../lib/festival/speech_tools/lib )
find_library(ESTSTRING eststring ../../lib/festival/speech_tools/lib )
find_library(ESTBASE estbase ../../lib/festival/speech_tools/lib )

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
find_library (COREAUDIO_LIBRARY CoreAudio)
find_library (AUDIOUNIT_LIBRARY AudioUnit)
find_library (CORESERVICES_LIBRARY CoreServices)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set( MORE_LIBS
        ${COREAUDIO_LIBRARY}
        ${AUDIOUNIT_LIBRARY}
        ${CORESERVICES_LIBRARY}
)
else()
set( MORE_LIBS
	rt
)
endif()

add_executable( festivalrelay

	src/FestivalRelay.cpp
)

target_link_libraries( festivalrelay

	${FESTIVALLIB}
	${ESTTOOLS}
	${ESTSTRING}
	${ESTBASE}
	xerces-c
	vhcl
	vhmsg	
	activemq-cpp
	ncurses
	pthread
	${MORE_LIBS}
)

install(
	TARGETS festivalrelay
	RUNTIME DESTINATION core/FestivalRelay
)
	
