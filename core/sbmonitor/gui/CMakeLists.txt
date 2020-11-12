cmake_minimum_required (VERSION 2.6)
project (sbmonitor)

# build deps
add_subdirectory(../../../lib/vhcl ../../../lib/vhcl)
add_subdirectory(../../../lib/vhmsg/vhmsg-c ../../../lib/vhmsg/vhmsg-c)
add_subdirectory(../lib ../lib)


include_directories(../lib)
include_directories(../../../lib/vhcl/include)
include_directories(../../../lib/vhmsg/vhmsg-c/include)


if (UNIX)
   include_directories(/usr/local/include/activemq-cpp-3.4.2)
   include_directories(/Library/Frameworks/QtGui.framework/Versions/4/Headers)
   include_directories(/Library/Frameworks/QtOpenGL.framework/Versions/4/Headers)
endif (UNIX)



####

set( SBMONITOR_SRC

	main.cpp
	SbmDebuggerForm.cpp
        glwidget.cpp
	Camera.cpp
	FaceDialog.cpp
	BmlCreatorDialog.cpp
	DataViewerDialog.cpp
	SettingsDialog.cpp
	ResourceDialog.cpp
	CommandDialog.cpp
	GLGraphWidget.cpp
	ConnectDialog.cpp
	UtilsDialog.cpp
	EntityCreatorDialog.cpp

	moc_BMLCreatorDialog.cpp
	moc_CommandDialog.cpp
	moc_ConnectDialog.cpp
	moc_DataViewerDialog.cpp
	moc_EntityCreatorDialog.cpp
	moc_FaceDialog.cpp
	moc_glwidget.cpp
	moc_ResourceDialog.cpp
	moc_SbmDebuggerForm.cpp
	moc_SettingsDialog.cpp
	moc_UtilsDialog.cpp

)

####

find_package( Qt4 4.8.0 COMPONENTS QtCore QtGui QtOpenGL REQUIRED )
find_package( OpenGL )


include(${QT_USE_FILE})


message("${QT_LIBRARIES}")
message("${OPENGL_gl_LIBRARY}")
message("${OPENGL_LIBRARIES}")


add_executable(sbmonitor ${SBMONITOR_SRC})


if (MSVC)
   target_link_libraries(test       ../lib/vhcl)
   target_link_libraries(test       ../openal/libs/Win32/OpenAL32)
   target_link_libraries(test       ../openal/libs/Win32/alut)
   target_link_libraries(test       ../libsndfile-1_0_17/libsndfile-1)
   target_link_libraries(test       ../log4cxx/lib/log4cxx)
   target_link_libraries(test       dbghelp)
endif (MSVC)

if (UNIX)
   target_link_libraries(sbmonitor vhcl)
   target_link_libraries(sbmonitor vhmsg)
   target_link_libraries(sbmonitor activemq-cpp)
   target_link_libraries(sbmonitor sbmonitorlib)
   target_link_libraries(sbmonitor ${OPENGL_LIBRARIES})
   target_link_libraries(sbmonitor ${QT_LIBRARIES})
endif ()

##QtCore4.lib;QtGui4.lib;qtmain.lib;QtOpenGL4.lib;pthreadVSE2.lib;activemq-cpp.lib;opengl32.lib;glu32.lib;ws2_32.lib;dbghelp.lib;winmm.lib;rpcrt4.lib;%(AdditionalDependencies)
