include_directories( /usr/include/OGRE )
include_directories( ../../lib/bonebus/include )
include_directories( /Users/feng/SmartBodyDev/SbmOSX/trunk/core/smartbody/sbm/Frameworks/Ogre.framework/Versions/1.7.4/Headers )
include_directories( /Users/feng/SmartBodyDev/OgreSDK_1.7/include/ )


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
find_library (COREAUDIO_LIBRARY CoreAudio)
find_library (AUDIOUNIT_LIBRARY AudioUnit)
find_library (CORESERVICES_LIBRARY CoreServices)
find_library (IOKIT_LIBRARY IOKit)
find_library (FOUNDATION_LIBRARY Foundation)
find_library (COCOA_LIBRARY Cocoa)
find_library (X11_LIBRARY X11)
find_library (FESTIVAL Festival ../../../lib/festival/festival/src/lib)
find_library (ESTBASE estbase ../../../lib/festival/speech_tools/lib)
find_library (ESTSTRING eststring ../../../lib/festival/speech_tools/lib)
find_library (ESTOOLS estools ../../../lib/festival/speech_tools/lib)
find_library (OGREMAIN Ogre /Users/feng/SmartBodyDev/ogre_src_v1-8-0/build/lib/relwithdebinfo/) 
#find_library (OGREMAIN Ogre /Users/feng/SmartBodyDev/OgreSDK/lib/release/) 
find_library (OGREMAIN_17 Ogre /Users/feng/SmartBodyDev/OgreSDK_1.7/lib/release/) 
find_library (OGREMAIN_17_r Ogre /Users/feng/SmartBodyDev/ogre_src_v1-7-4/build/lib/RelWithDebInfo/)
find_library (OISLIB OIS /Users/feng/SmartBodyDev/ogre_src_v1-8-0/build/lib/relwithdebinfo/)
endif()

set (BUILD_OGRE TRUE)
if (${BUILD_OGRE})
set (OGRE_LIBS
	${OGREMAIN_17}
	#${OGREMAIN_17_r}
	${OISLIB}		
)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
link_directories(/usr/X11R6/lib)
set( MORE_LIBS
        ${COREAUDIO_LIBRARY}
        ${AUDIOUNIT_LIBRARY}
        ${CORESERVICES_LIBRARY}
	${IOKIT_LIBRARY}
	${FOUNDATION_LIBRARY}
	${COCOA_LIBRARY}
	${X11_LIBRARY}
	GL
        ${LIBSNDFILE}
        ${ALUT}
        ${OPENAL}
        ${FESTIVAL}
        ${ESTBASE}
        ${ESTSTRING}
        ${ESTOOLS}
)
else()
set( MORE_LIBS
        ${LIBSNDFILE}
        ${ALUT}
        ${OPENAL}
)

endif()


add_executable( ogreviewer

	src/BoneBusHandler.cpp
	src/OgreFrameListener.cpp
	src/OgreRenderer.cpp
	src/SBListener.cpp
	src/main.cpp
	src/ini.c
)

target_link_libraries( ogreviewer

	SmartBody
	xerces-c
	vhcl
	vhmsg	
	activemq-cpp
	OgreMain
	OIS
	alut
	sndfile
	GLEW
	${OGRE_LIBS}
	bonebus
	${MORE_LIBS}

)

set( EXEC_LIST

       ogreviewer 
	)

set( APP_LIBRARY_SOURCE_PATHS

        ${PROJECT_SOURCE_DIR}/core/ogre-viewer/bin
        ${PROJECT_SOURCE_DIR}/core/smartbody/sbm/bin
)

set_target_properties(

        ${EXEC_LIST}

	PROPERTIES

	## link with install tree:
	INSTALL_RPATH                   "${APP_LIBRARY_SOURCE_PATHS}"
	BUILD_WITH_INSTALL_RPATH        TRUE
)


install(
	TARGETS ogreviewer
	RUNTIME DESTINATION core/ogre-viewer
)
	
