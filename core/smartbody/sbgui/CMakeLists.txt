if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOUNIT_LIBRARY AudioUnit)
    find_library(CORESERVICES_LIBRARY CoreServices)
endif ()


if (${BUILD_OGRE})
    find_path(OGRE_INCLUDE_PATH Ogre.h)
    include_directories(${OGRE_INCLUDE_PATH})
    set(OGREMAINLIB OgreMain)
    set(OISLIB OIS)
    set(OGRE_LIBS
            ${OGREMAINLIB}
            ${OISLIB}
            #${RTSHADERLIB}
            )

    if (${SB_NO_VHMSG})
        set(VHMSG_LIB
                )
    else ()
        set(VHMSG_LIB
                vhmsg
                ${ACTIVEMQLIB}
                activemq-cpp
                )
    endif ()

    if (${SB_NO_BONEBUS})
        set(BONEBUS_LIB
                )
    else ()
        set(BONEBUS_LIB
                bonebus
                )
    endif ()


    set(OGRE_SRCS
            src/EmbeddedOgre.cpp
            src/ExampleFrameListener.cpp
            src/FLTKOgreViewer.cpp
            src/OgreFrameListener.cpp
            src/SBOgreListener.cpp
            )
else ()
    set(OGRE_LIBS
            )
    set(OGRE_SRCS
            )
    add_definitions(-DNO_OGRE_VIEWER_CMD)
endif ()


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    link_directories(/usr/X11R6/lib)
    if (${BUILD_FESTIVAL})
        set(FESTIVAL_LIBS
                ${FESTIVAL}
                ${ESTBASE}
                ${ESTSTRING}
                ${ESTOOLS}
                )
    else ()
        set(FESTIVAL_LIBS)
    endif ()
    set(MORE_LIBS
            ${COREAUDIO_LIBRARY}
            ${AUDIOUNIT_LIBRARY}
            ${CORESERVICES_LIBRARY}
            ${IOKIT_LIBRARY}
            ${FOUNDATION_LIBRARY}
            ${COCOA_LIBRARY}
            )
else ()
    set(MORE_LIBS)

endif ()


# osx c++ version >= clang 4.2.0
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.2.0))
            add_definitions(-Wno-deprecated-writable-strings)
            add_definitions(-Wno-format-security)
        endif ()
    endif ()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
        if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7.3))
            add_definitions(-Wno-format)
            add_definitions(-Wno-format-security)
            add_definitions(-Wno-write-strings)
        endif ()
    endif ()
endif ()


set(SBM_FLTK_SRC

        src/sbm_main.cpp
        src/ppm.cpp
        src/pic.cpp
        #	src/glfont2.cpp
        src/CommandWindow.cpp
        src/ObjectManipulationHandle.cpp
        src/PawnPosControl.cpp
        src/PositionControl.cpp
        src/RotationControl.cpp
        src/RootWindow.cxx
        src/ExportWindow.cpp
        src/fltk_viewer.cpp
        src/SBWindowListener.cpp
        src/SBSelectionManager.cpp
        src/SBInterfaceListener.cpp
        ${OGRE_SRCS}
        src/pic.cpp
        src/ppm.cpp
        src/bmlviewer/BehaviorBlock.cpp
        src/bmlviewer/BehaviorEditorWidget.cpp
        src/bmlviewer/BehaviorWindow.cpp
        src/channelbufferviewer/GlChartView.cpp
        src/channelbufferviewer/GlChartViewArchive.cpp
        src/channelbufferviewer/GlChartViewCoordinate.cpp
        src/channelbufferviewer/channelbufferWindow.cpp
        src/nle/NonLinearEditor.cpp
        src/nle/NonLinearEditorWidget.cpp
        src/panimationviewer/PanimationWindow.cpp
        src/panimationviewer/ParamAnimBlock.cpp
        src/panimationviewer/ParamAnimEditorWidget.cpp
        src/panimationviewer/ParamAnimRunTimeEditor.cpp
        src/panimationviewer/ParamAnimScriptEditor.cpp
        src/panimationviewer/ParamAnimStateEditor.cpp
        src/panimationviewer/ParamAnimTransitionEditor.cpp
        src/panimationviewer/ParamAnimTransitionEditor2.cpp
        src/panimationviewer/ParamAnimTransitionCreatorWidget.cpp
        src/panimationviewer/ParameterGroup.cpp
        src/panimationviewer/ParameterVisualization.cpp
        src/panimationviewer/Parameter3DVisualization.cpp
        src/panimationviewer/ParamAnimStateCreatorWidget.cpp
        src/panimationviewer/ParamAnimAutoMarkingEditor.cpp
        src/panimationviewer/ParamAnimParameterEditor.cpp
        src/CharacterCreatorWindow.cpp
        src/bmlcreator/AttributeWindow.cpp
        src/bmlcreator/TextEditor.cpp
        src/bmlcreator/BMLCreatorWindow.cpp

        src/resourceViewer/Fl_Tree_Horizontal.cpp
        src/resourceViewer/TreeItemInfoWidget.cpp
        src/resourceViewer/ResourceWindow.cpp
        src/resourceViewer/TreeInfoObject.cpp
        src/resourceViewer/AttributeEditor.cpp

        src/visemeviewer/VisemeCurve.cpp
        src/visemeviewer/VisemeCurveEditor.cpp
        src/visemeviewer/VisemeViewerWindow.cpp
        src/visemeviewer/VisemeRunTimeWindow.cpp
        src/visemeviewer/VisemeRTCurveViewer.cpp
        src/visemeviewer/ImageSequencePlayer.cpp

        src/retargetviewer/RetargetViewer.cpp
        src/retargetcreator/RetargetCreatorWindow.cpp
        src/autorig/SBAutoRigManager.cpp
        src/autorig/VoxelizerWindow.cpp
        src/autorig/AutoRigViewer.cpp

        src/monitorviewer/MonitorConnectWindow.cpp
        src/motioneditor/MotionEditorWindow.cpp
        src/jointmapviewer/JointMapViewer.cpp
        src/jointmapviewer/RetargetStepWindow.cpp

        src/posecreator/PoseCreator.cpp

        src/faceviewer/FaceViewer.cpp
        src/faceshiftviewer/FaceShiftViewer.cpp
        src/FLTKListener.cpp
        src/SBGUIManager.cpp
        src/SBBaseRenderer.cpp
        src/SBRenderer.cpp
        src/SBFrameBufferObject.cpp
        src/Session.cpp

        src/flu/Flu_Button.cpp
        src/flu/Flu_Collapsable_Group.cpp
        src/flu/FluSimpleString.cpp

        src/pythonbind/SBPythonAutorig.cpp
        )

add_executable(sbgui
        ${SBM_FLTK_SRC}
        )

target_link_libraries(sbgui
        SmartBody
        SmartBody-renderer-gl
        SmartBody-bindings-python
        SmartBody-debugger
        SmartBody-renderer-gl-bindings-python
        SmartBody-physics-ode
        jpge
        pinocchio
        ${MORE_LIBS}
        ${OGRE_LIBS}
        pthread
        )

target_include_directories(sbgui PUBLIC
        src
        $<TARGET_PROPERTY:util,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-renderer-gl,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-bindings-python,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-bindings-python,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-renderer-gl-bindings-python,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-physics-ode,INCLUDE_DIRECTORIES>
        )

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(sbgui)
endif ()


install(TARGETS sbgui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

