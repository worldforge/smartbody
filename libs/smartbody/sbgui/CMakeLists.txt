if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOUNIT_LIBRARY AudioUnit)
    find_library(CORESERVICES_LIBRARY CoreServices)
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

set(SBM_FLTK_SRC
        src/glfont2.cpp
        src/GenericViewer.cpp
        src/sbm_main.cpp
        src/ppm.cpp
        src/pic.cpp
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
        src/pythonbind/PythonSetup.cpp
        )

add_executable(sbgui
        ${SBM_FLTK_SRC}
        )

target_link_libraries(sbgui
        SmartBody-renderer-gl-bindings-python
        SmartBody-bindings-python
        SmartBody-navmesh-bindings-python
        SmartBody-bml-bindings-python
        SmartBody-renderer-gl
        SmartBody-debugger
        SmartBody-physics-ode
        SmartBody-control
        SmartBody-navmesh
        SmartBody-vhmsg
        SmartBody-bonebus
        SmartBody-serialize
        SmartBody-steering-steerlib-bml
        SmartBody-steering-steerlib
        SmartBody-export
        SmartBody-bml
        SmartBody
        polyvox::polyvox
        fltk::fltk
        jpge
        pinocchio
        ${MORE_LIBS}
        )

target_include_directories(sbgui PUBLIC
        src
        )


install(TARGETS sbgui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

