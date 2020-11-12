## SmartBody
include_directories( ./SmartBody)
LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/lib )
LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/bin )

add_definitions( "-DdDOUBLE" )

add_definitions( "-Dfopen64=fopen" )
add_definitions( "-Dfseeko64=fseeko" )
add_definitions( "-Dftello64=ftello" )



set(SB_EXPORTS 1)
set(SMARTBODY_DLL_EXPORTS 1)
set(SMARTBODY_C_DLL_EXPORTS 1)
set(_COMPILING_STEERLIB 1)


find_library (LIBSNDFILE sndfile )
find_library (ALUT alut )
find_library (OPENAL openal)
find_library (GLEW GLEW)


set( MORE_LIBS
	${LIBSNDFILE}
	${ALUT}
	${OPENAL}
	${GLEW}
	rt
)

find_package(OpenGL REQUIRED)



set( SMARTBODY_LIB_EXT_NANOFLANN_SRC

        ./SmartBody/external/nanoflann/nanoflann.hpp

)

set( SMARTBODY_LIB_EXT_PARSER_SRC

        ./SmartBody/external/parser/Bchart.cpp
        ./SmartBody/external/parser/BchartSm.cpp
        ./SmartBody/external/parser/Bst.cpp
        ./SmartBody/external/parser/ChartBase.cpp
        ./SmartBody/external/parser/ClassRule.cpp
        ./SmartBody/external/parser/CntxArray.cpp
        ./SmartBody/external/parser/CombineBests.cpp
        ./SmartBody/external/parser/ECArgs.cpp
        ./SmartBody/external/parser/Edge.cpp
        ./SmartBody/external/parser/EdgeHeap.cpp
        ./SmartBody/external/parser/edgeSubFns.cpp
        ./SmartBody/external/parser/EgsFromTree.cpp
        ./SmartBody/external/parser/ewDciTokStrm.cpp
        ./SmartBody/external/parser/extraMain.cpp
        ./SmartBody/external/parser/FBinaryArray.cpp
        ./SmartBody/external/parser/Feat.cpp
        ./SmartBody/external/parser/Feature.cpp
        ./SmartBody/external/parser/FeatureTree.cpp
        ./SmartBody/external/parser/fhSubFns.cpp
        ./SmartBody/external/parser/Field.cpp
        ./SmartBody/external/parser/FullHist.cpp
        ./SmartBody/external/parser/GotIter.cpp
        ./SmartBody/external/parser/headFinder.cpp
        ./SmartBody/external/parser/headFinderCh.cpp
        ./SmartBody/external/parser/InputTree.cpp
        ./SmartBody/external/parser/Item.cpp
        ./SmartBody/external/parser/Link.cpp
        ./SmartBody/external/parser/MeChart.cpp
        ./SmartBody/external/parser/Params.cpp
        ./SmartBody/external/parser/ParseStats.cpp
        ./SmartBody/external/parser/SentRep.cpp
        ./SmartBody/external/parser/Term.cpp
        ./SmartBody/external/parser/TimeIt.cpp
        ./SmartBody/external/parser/UnitRules.cpp
        ./SmartBody/external/parser/utils.cpp
        ./SmartBody/external/parser/ValHeap.cpp
)

set( SMARTBODY_LIB_EXT_ZLIB_SRC
	
	./SmartBody/external/zlib-1.2.5/adler32.c
	./SmartBody/external/zlib-1.2.5/compress.c
	./SmartBody/external/zlib-1.2.5/crc32.c
	./SmartBody/external/zlib-1.2.5/deflate.c
	./SmartBody/external/zlib-1.2.5/example.c
	./SmartBody/external/zlib-1.2.5/gzclose.c
	./SmartBody/external/zlib-1.2.5/gzlib.c
	./SmartBody/external/zlib-1.2.5/gzread.c
	./SmartBody/external/zlib-1.2.5/gzwrite.c
	./SmartBody/external/zlib-1.2.5/infback.c
	./SmartBody/external/zlib-1.2.5/inffast.c
	./SmartBody/external/zlib-1.2.5/inflate.c
	./SmartBody/external/zlib-1.2.5/inftrees.c
	./SmartBody/external/zlib-1.2.5/ioapi.c
	./SmartBody/external/zlib-1.2.5/trees.c
	./SmartBody/external/zlib-1.2.5/uncompr.c
	./SmartBody/external/zlib-1.2.5/unzip.c
	./SmartBody/external/zlib-1.2.5/zip.c
	./SmartBody/external/zlib-1.2.5/zutil.c
)

set( SMARTBODY_LIB_EXT_RECAST_SRC

        ./SmartBody/external/recast/Recast.cpp
        ./SmartBody/external/recast/RecastAlloc.cpp
        ./SmartBody/external/recast/RecastArea.cpp
        ./SmartBody/external/recast/RecastContour.cpp
        ./SmartBody/external/recast/RecastFilter.cpp
        ./SmartBody/external/recast/RecastLayers.cpp
        ./SmartBody/external/recast/RecastMesh.cpp
        ./SmartBody/external/recast/RecastMeshDetail.cpp
        ./SmartBody/external/recast/RecastRasterization.cpp
        ./SmartBody/external/recast/RecastRegion.cpp
        ./SmartBody/external/recast/DetourAlloc.cpp
        ./SmartBody/external/recast/DetourCommon.cpp
        ./SmartBody/external/recast/DetourNavMesh.cpp
        ./SmartBody/external/recast/DetourNavMeshBuilder.cpp
        ./SmartBody/external/recast/DetourNavMeshQuery.cpp
        ./SmartBody/external/recast/DetourNode.cpp
)

set( SMARTBODY_LIB_EXT_SOIL_SRC

        ./SmartBody/external/SOIL/image_DXT.c
        ./SmartBody/external/SOIL/image_helper.c
        ./SmartBody/external/SOIL/SOIL.c
        ./SmartBody/external/SOIL/stb_image_aug.c
	./SmartBody/external/SOIL/stbi_DDS_aug_c.h
)

set( SMARTBODY_LIB_EXT_PERLIN_SRC

        ./SmartBody/external/perlin/perlin.cpp
)

set( SMARTBODY_LIB_ME_SRC

        ./SmartBody/controllers/me_controller_context.cpp
        ./SmartBody/controllers/me_controller_context_proxy.cpp
        ./SmartBody/controllers/me_controller.cpp
        ./SmartBody/controllers/me_controller_tree_root.cpp
        ./SmartBody/controllers/me_ct_adshr_envelope.cpp
        ./SmartBody/controllers/me_ct_blend.cpp
        ./SmartBody/controllers/me_ct_channel_writer.cpp
        ./SmartBody/controllers/me_ct_container.cpp
        ./SmartBody/controllers/me_ct_curve_writer.cpp
        ./SmartBody/controllers/me_ct_interpolator.cpp
pp
        ./SmartBody/controllers/me_ct_motion.cpp
        ./SmartBody/controllers/me_ct_periodic_replay.cpp
        ./SmartBody/controllers/me_ct_pose.cpp
        ./SmartBody/controllers/me_ct_scheduler2.cpp
        ./SmartBody/controllers/me_ct_time_shift_warp.cpp
        ./SmartBody/controllers/me_ct_unary.cpp
        ./SmartBody/controllers/me_default_prune_policy.cpp
        ./SmartBody/controllers/me_spline_1d.cpp
	./SmartBody/controllers/me_ct_pose_postprocessing.cpp
)

set( SMARTBODY_LIB_SK_SRC

        ./SmartBody/sk/sk_channel_array.cpp
        ./SmartBody/sk/sk_channel.cpp
        ./SmartBody/sk/sk_joint.cpp
        ./SmartBody/sk/sk_joint_euler.cpp
        ./SmartBody/sk/sk_joint_pos.cpp
        ./SmartBody/sk/sk_joint_quat.cpp
        ./SmartBody/sk/sk_joint_swing_twist.cpp
        ./SmartBody/sk/sk_motion.cpp
        ./SmartBody/sk/sk_motion_io.cpp
        ./SmartBody/sk/sk_posture.cpp
        ./SmartBody/sk/sk_scene.cpp
        ./SmartBody/sk/sk_skeleton.cpp
        ./SmartBody/sk/sk_skeleton_io.cpp
        ./SmartBody/sk/sk_vec_limits.cpp
)

set( SMARTBODY_LIB_SR_SRC

        ./SmartBody/sr/sr_alg.cpp
        ./SmartBody/sr/sr_array.cpp
        ./SmartBody/sr/sr_box.cpp
        ./SmartBody/sr/sr_buffer.cpp
        ./SmartBody/sr/sr_camera.cpp
        ./SmartBody/sr/sr_color.cpp
        ./SmartBody/sr/sr.cpp
        ./SmartBody/sr/sr_cylinder.cpp
        ./SmartBody/sr/sr_euler.cpp
        ./SmartBody/sr/sr_event.cpp
        ./SmartBody/sr/sr_geo2.cpp
        ./SmartBody/sr/sr_gl.cpp
        ./SmartBody/sr/sr_gl_render_funcs.cpp
        ./SmartBody/sr/sr_hash_table.cpp
        ./SmartBody/sr/sr_input.cpp
        ./SmartBody/sr/sr_light.cpp
        ./SmartBody/sr/sr_line.cpp
        ./SmartBody/sr/sr_lines.cpp
        ./SmartBody/sr/sr_mat.cpp
        ./SmartBody/sr/sr_material.cpp
        ./SmartBody/sr/sr_model.cpp
        ./SmartBody/sr/sr_model_export_iv.cpp
        ./SmartBody/sr/sr_model_import_obj.cpp
        ./SmartBody/sr/sr_output.cpp
        ./SmartBody/sr/sr_path_array.cpp
        ./SmartBody/sr/sr_plane.cpp
        ./SmartBody/sr/sr_points.cpp
        ./SmartBody/sr/sr_polygon.cpp
        ./SmartBody/sr/sr_polygons.cpp
        ./SmartBody/sr/sr_quat.cpp
        ./SmartBody/sr/sr_sa_bbox.cpp
        ./SmartBody/sr/sr_sa.cpp
        ./SmartBody/sr/sr_random.cpp
        ./SmartBody/sr/sr_sa_event.cpp
        ./SmartBody/sr/sr_sa_gl_render.cpp
        ./SmartBody/sr/sr_sa_render_mode.cpp
        ./SmartBody/sr/sr_shared_ptr.cpp
        ./SmartBody/sr/sr_sn.cpp
	./SmartBody/sr/sr_sn_editor.cpp
        ./SmartBody/sr/sr_sn_group.cpp
        ./SmartBody/sr/sr_sn_matrix.cpp
        ./SmartBody/sr/sr_sn_shape.cpp
        ./SmartBody/sr/sr_sn_colorsurf.cpp
        ./SmartBody/sr/sr_sphere.cpp
        ./SmartBody/sr/sr_spline.cpp
        ./SmartBody/sr/sr_string_array.cpp
        ./SmartBody/sr/sr_string.cpp
        ./SmartBody/sr/sr_timer.cpp
        ./SmartBody/sr/sr_trackball.cpp
        ./SmartBody/sr/sr_tree.cpp
        ./SmartBody/sr/sr_triangle.cpp
        ./SmartBody/sr/sr_vec2.cpp
        ./SmartBody/sr/sr_vec.cpp
        ./SmartBody/sr/sr_viewer.cpp
        ./SmartBody/sr/planner/sk_blend_cfg.cpp
        ./SmartBody/sr/planner/sk_pos_planner.cpp
        ./SmartBody/sr/planner/sk_blend_cfg_manager.cpp
        ./SmartBody/sr/planner/sr_cfg_planner.cpp
        ./SmartBody/sr/planner/sk_blend_planner.cpp
        ./SmartBody/sr/planner/sr_cfg_manager.cpp
        ./SmartBody/sr/planner/sk_pos_cfg.cpp
        ./SmartBody/sr/planner/sr_cfg_planner_lazy.cpp
        ./SmartBody/sr/planner/sr_cfg_path.cpp
        ./SmartBody/sr/planner/sr_cfg_tree.cpp
        ./SmartBody/sr/planner/sk_pos_cfg_manager.cpp
)

set( SMARTBODY_LIB_SBM_SRC

        ./SmartBody/sbm/BMLDefs.cpp
        ./SmartBody/bml/behavior_scheduler_constant_speed.cpp
        ./SmartBody/bml/behavior_scheduler.cpp
        ./SmartBody/bml/behavior_scheduler_fixed.cpp
		./SmartBody/bml/behavior_scheduler_gesture.cpp
        ./SmartBody/bml/behavior_span.cpp
        ./SmartBody/bml/bml_animation.cpp
        ./SmartBody/bml/bml_gesture.cpp
        ./SmartBody/bml/bml_bodyreach.cpp
        ./SmartBody/bml/bml_constraint.cpp
        ./SmartBody/bml/bml.cpp
        ./SmartBody/bml/bml_event.cpp
        ./SmartBody/bml/bml_face.cpp
        ./SmartBody/bml/bml_gaze.cpp
        ./SmartBody/bml/bml_general_param.cpp
        ./SmartBody/bml/bml_grab.cpp
        ./SmartBody/bml/bml_interrupt.cpp
        ./SmartBody/bml/bml_locomotion.cpp
        ./SmartBody/bml/bml_processor.cpp
        ./SmartBody/bml/bml_quickdraw.cpp
        ./SmartBody/bml/bml_reach.cpp
        ./SmartBody/bml/bml_speech.cpp
        ./SmartBody/bml/bml_sync_point.cpp
        ./SmartBody/bml/bml_target.cpp
        ./SmartBody/bml/bml_saccade.cpp
        ./SmartBody/bml/bml_noise.cpp
		./SmartBody/bml/bml_states.cpp
        ./SmartBody/sbm/GenericViewer.cpp
        ./SmartBody/sbm/gwiz_cmdl.cpp
        ./SmartBody/sbm/gwiz_math.cpp
        ./SmartBody/sbm/gwiz_spline.cpp
        ./SmartBody/sbm/Heightfield.cpp
        ./SmartBody/sbm/lin_win.cpp
        ./SmartBody/sbm/mcontrol_callbacks.cpp
        ./SmartBody/sbm/action_unit.cpp

        ./SmartBody/controllers/me_ct_basic_locomotion.cpp
        ./SmartBody/controllers/me_ct_ccd_IK.cpp
        ./SmartBody/controllers/me_ct_constraint.cpp
        ./SmartBody/controllers/me_ct_data_driven_reach.cpp
        ./SmartBody/controllers/me_ct_data_interpolation.cpp
        ./SmartBody/controllers/me_ct_example_body_reach.cpp
        ./SmartBody/controllers/me_ct_examples.cpp
        ./SmartBody/controllers/me_ct_eyelid.cpp
        ./SmartBody/controllers/me_ct_face.cpp
        ./SmartBody/controllers/me_ct_gaze_alg.cpp
        ./SmartBody/controllers/me_ct_gaze.cpp
        ./SmartBody/controllers/me_ct_gaze_joint.cpp
        ./SmartBody/controllers/me_ct_gaze_keymap.cpp
        ./SmartBody/controllers/me_ct_gaze_target.cpp
        ./SmartBody/controllers/me_ct_IK.cpp
        ./SmartBody/controllers/me_ct_IK_scenario.cpp
        ./SmartBody/controllers/me_ct_jacobian_IK.cpp
        ./SmartBody/controllers/me_ct_limb.cpp
        ./SmartBody/controllers/me_ct_locomotion_func.cpp
        ./SmartBody/controllers/me_ct_motion_example.cpp
        ./SmartBody/controllers/me_ct_motion_parameter.cpp
        ./SmartBody/controllers/me_ct_motion_player.cpp
        ./SmartBody/controllers/me_ct_motion_timewarp.cpp
        ./SmartBody/controllers/me_ct_param_animation.cpp
        ./SmartBody/controllers/me_ct_param_animation_utilities.cpp
        ./SmartBody/controllers/me_ct_quick_draw.cpp
        ./SmartBody/controllers/me_ct_reach.cpp
        ./SmartBody/controllers/me_ct_hand.cpp
        ./SmartBody/controllers/me_ct_motion_profile.cpp
        ./SmartBody/controllers/me_ct_barycentric_interpolation.cpp
        ./SmartBody/controllers/me_ct_inverse_interpolation.cpp
        ./SmartBody/controllers/me_ct_reach_IK.cpp
        ./SmartBody/controllers/me_ct_simple_gaze.cpp
        ./SmartBody/controllers/me_ct_tether.cpp
        ./SmartBody/controllers/me_ct_ublas.cpp
        ./SmartBody/controllers/me_ct_breathing.cpp
        ./SmartBody/controllers/me_ct_breathing_interface.cpp
        ./SmartBody/controllers/me_ct_physics_controller.cpp
        ./SmartBody/controllers/me_ct_noise_controller.cpp
        ./SmartBody/controllers/me_ct_motion_recorder.cpp

        ./SmartBody/controllers/MeCtBlendEngine.cpp
        ./SmartBody/controllers/me_ct_motion_blend.cpp
        ./SmartBody/controllers/MotionAnalysis.cpp

        ./SmartBody/sbm/ParserBVH.cpp
        ./SmartBody/sbm/ParserASFAMC.cpp
        ./SmartBody/sbm/ParserFBX.cpp
        ./SmartBody/sbm/ParserOgre.cpp
        ./SmartBody/sbm/ParserCOLLADAFast.cpp
        ./SmartBody/sbm/ParserOpenCOLLADA.cpp
        ./SmartBody/sbm/remote_speech.cpp
        ./SmartBody/sbm/sbm_audio.cpp
        ./SmartBody/controllers/MeCtBodyReachState.cpp
        ./SmartBody/controllers/MeCtReachEngine.cpp
        ./SmartBody/controllers/me_ct_saccade.cpp
        ./SmartBody/sbm/local_speech.cpp
## UNUSED:
##      sbm/sbm_bml.cpp
        ./SmartBody/sbm/sbm_deformable_mesh.cpp
        ./SmartBody/sbm/sbm_speech_audiofile.cpp
        ./SmartBody/sbm/sbm_speech.cpp
        ./SmartBody/sbm/sbm_speech_impl_skeleton.cpp
        ./SmartBody/sbm/sbm_test_cmds.cpp
        ./SmartBody/sbm/sr_arg_buff.cpp
        ./SmartBody/sbm/sr_cmd_line.cpp
        ./SmartBody/sbm/sr_cmd_seq.cpp
        ./SmartBody/sbm/sr_hash_map.cpp
        ./SmartBody/sbm/sr_linear_curve.cpp
        ./SmartBody/sbm/sr_spline_curve.cpp
        ./SmartBody/sbm/sr_synch_points.cpp
        ./SmartBody/sbm/text_speech.cpp
        ./SmartBody/sbm/time_profiler.cpp
        ./SmartBody/sbm/time_regulator.cpp
        ./SmartBody/sbm/xercesc_utils.cpp


        ./SmartBody/sbm/ODEPhysicsSim.cpp

        ./SmartBody/sbm/GPU/SbmTexture.cpp
        ./SmartBody/sbm/GPU/SbmDeformableMeshGPU.cpp
        ./SmartBody/sbm/GPU/TBOData.cpp
        ./SmartBody/sbm/GPU/VBOData.cpp
        ./SmartBody/sbm/GPU/SbmShader.cpp
        ./SmartBody/sbm/PPRAISteeringAgent.cpp
        ./SmartBody/sbm/SteerSuiteEnginerDriver.cpp
        ./SmartBody/sbm/sr_path_list.cpp

        ./SmartBody/sb/sbm_character.cpp
        ./SmartBody/sb/sbm_pawn.cpp

        ./SmartBody/sb/SBAttribute.cpp
        ./SmartBody/sb/SBAttributeManager.cpp
        ./SmartBody/sb/SBObject.cpp
        ./SmartBody/sb/SBObserver.cpp
        ./SmartBody/sb/SBSubject.cpp
        ./SmartBody/sb/DefaultAttributeTable.cpp

        ./SmartBody/sb/SBScene.cpp
        ./SmartBody/sbm/KinectProcessor.cpp
        ./SmartBody/controllers/me_ct_data_receiver.cpp

        ./SmartBody/sb/SBScript.cpp
        ./SmartBody/sb/SBCharacter.cpp
        ./SmartBody/sb/SBPawn.cpp
        ./SmartBody/sb/SBJoint.cpp
        ./SmartBody/sb/SBSkeleton.cpp
        ./SmartBody/sb/SBController.cpp
        ./SmartBody/sb/SBSimulationManager.cpp
        ./SmartBody/sb/SBBmlProcessor.cpp
        ./SmartBody/sb/SBAnimationState.cpp
        ./SmartBody/sb/SBAnimationTransition.cpp
        ./SmartBody/sb/SBAnimationTransitionRule.cpp
        ./SmartBody/sb/SBAnimationStateManager.cpp
        ./SmartBody/sb/SBSteerManager.cpp
        ./SmartBody/sb/SBSteerAgent.cpp
        ./SmartBody/sb/SBReachManager.cpp
        ./SmartBody/sb/SBReach.cpp
        ./SmartBody/sb/SBGestureMapManager.cpp
        ./SmartBody/sb/SBGestureMap.cpp
        ./SmartBody/sb/SBServiceManager.cpp
        ./SmartBody/sb/SBService.cpp
        ./SmartBody/sb/SBMotion.cpp
        ./SmartBody/sb/SBFaceDefinition.cpp
        ./SmartBody/sb/SBPhysicsManager.cpp
        ./SmartBody/sb/SBBoneBusManager.cpp
        ./SmartBody/sb/SBCollisionManager.cpp
        ./SmartBody/sb/SBParser.cpp
        ./SmartBody/sb/SBParseNode.cpp
        ./SmartBody/sb/SBJointMap.cpp
        ./SmartBody/sb/SBJointMapManager.cpp
        ./SmartBody/sb/SBPhoneme.cpp
        ./SmartBody/sb/SBPhonemeManager.cpp
        ./SmartBody/sb/SBBehaviorSet.cpp
        ./SmartBody/sb/SBBehaviorSetManager.cpp
        ./SmartBody/sb/SBMotionBlendBase.cpp
        ./SmartBody/sb/nvbg.cpp
        ./SmartBody/sb/MiniBrain.cpp
        ./SmartBody/sb/SBColObject.cpp
        ./SmartBody/sb/SBPhysicsSim.cpp
        ./SmartBody/sb/SBEvent.cpp
        ./SmartBody/sb/PABlend.cpp
        ./SmartBody/sb/SBRetarget.cpp
        ./SmartBody/sb/SBRetargetManager.cpp
        ./SmartBody/sb/SBNavigationMesh.cpp
        ./SmartBody/sb/SBAsset.cpp
        ./SmartBody/sb/SBAssetManager.cpp
        ./SmartBody/sb/SBAsset.cpp
        ./SmartBody/sb/SBSpeechManager.cpp
        ./SmartBody/sb/SBVHMsgManager.cpp
        ./SmartBody/sb/SBCommandManager.cpp
        ./SmartBody/sb/SBWSPManager.cpp
        ./SmartBody/sb/SBNavigationMeshManager.cpp

        ./SmartBody/sb/SBPython.cpp
        ./SmartBody/sb/SBPythonAnimation.cpp
        ./SmartBody/sb/SBPythonAttribute.cpp
        ./SmartBody/sb/SBPythonCharacter.cpp
        ./SmartBody/sb/SBPythonMath.cpp
        ./SmartBody/sb/SBPythonMotion.cpp
        ./SmartBody/sb/SBPythonScene.cpp
        ./SmartBody/sb/SBPythonSimulation.cpp
        ./SmartBody/sb/SBPythonSkeleton.cpp
        ./SmartBody/sb/SBPythonSystem.cpp
        ./SmartBody/sb/SBPythonClass.cpp
		
        ./SmartBody/sb/SBAssetHandler.cpp
        ./SmartBody/sb/SBAssetHandlerCOLLADA.cpp
        ./SmartBody/sb/SBAssetHandlerSk.cpp
        ./SmartBody/sb/SBAssetHandlerSkm.cpp
	./SmartBody/sb/SBAssetHandlerAsf.cpp
	./SmartBody/sb/SBAssetHandlerAmc.cpp
	./SmartBody/sb/SBAssetHandlerOgre.cpp
	./SmartBody/sb/SBAssetHandlerObj.cpp
	./SmartBody/sb/SBAssetHandlerBvh.cpp
	./SmartBody/sb/SBAssetHandlerSkb.cpp
        ./SmartBody/sb/SBBehavior.cpp
        ./SmartBody/sb/SBDebuggerServer.cpp
        ./SmartBody/sb/SBDebuggerClient.cpp
        ./SmartBody/sb/SBDebuggerUtility.cpp
        ./SmartBody/sbm/SteerPath.cpp
        ./SmartBody/sbm/MiscCommands.cpp
		
		./SmartBody/sb/smartbody-dll.cpp
		./SmartBody/sb/smartbody-c-dll.cpp
		
)

add_library(
	SmartBody
	SHARED 

	${SMARTBODY_LIB_EXT_SOIL_SRC}

	${SMARTBODY_LIB_EXT_ZLIB_SRC}

        ${SMARTBODY_LIB_EXT_PARSER_SRC}

        ${SMARTBODY_LIB_EXT_RECAST_SRC}

        ${SMARTBODY_LIB_EXT_PERLIN_SRC}

        ${SMARTBODY_LIB_ME_SRC}

        ${SMARTBODY_LIB_SK_SRC}

        ${SMARTBODY_LIB_SR_SRC}

        ${SMARTBODY_LIB_SBM_SRC}

	

)


target_link_libraries( SmartBody

	#tcmalloc
	#profiler
	bonebus
	vhmsg
	vhcl
	wsp
	xerces-c
	boost_filesystem
	boost_system
	boost_regex
	boost_python
	boost_serialization
	blas
	lapack
	${OPENGL_LIBRARIES}
	activemq-cpp
	steerlib
	pprAI
	ode
	python2.7
	ncurses
	GLEW
	sndfile
	alut
	${MORE_LIBS}
)

set( EXEC_LIST

       SmartBody
)

set( APP_LIBRARY_SOURCE_PATHS

${PROJECT_SOURCE_DIR}/bin
)       

set_target_properties(
	${EXEC_LIST}
	PROPERTIES
	## link with install tree:
	INSTALL_RPATH                   "${APP_LIBRARY_SOURCE_PATHS}"
	BUILD_WITH_INSTALL_RPATH        TRUE
)

install(

        TARGETS ${EXEC_LIST}
	DESTINATION ./bin
)


###### simple smartbody
include_directories( ./simplesmartbody)
set( SIMPLESMARTBODY_SRC

	./simplesmartbody/simplesmartbody.cpp 
)

add_executable( simplesmartbody 

	${SIMPLESMARTBODY_SRC}
)

target_link_libraries( simplesmartbody 

	SmartBody
	pthread
	python2.7
)

set( EXEC_LIST

	simplesmartbody
)

set( APP_LIBRARY_BINARY_PATHS

        ${PROJECT_BINARY_DIR}/bin
)
	
set( APP_LIBRARY_SOURCE_PATHS

        ${PROJECT_SOURCE_DIR}/bin
)
	


set_target_properties(

        ${EXEC_LIST}

	PROPERTIES

	## link with install tree:
	INSTALL_RPATH                   "${APP_LIBRARY_SOURCE_PATHS}"
	BUILD_WITH_INSTALL_RPATH        TRUE
)

install( 
	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)


######### sbgui
include_directories( ./sbgui)

set( SBM_FLTK_SRC

	./sbgui/sbm_main.cpp
	./sbgui/ppm.cpp
	./sbgui/pic.cpp
#	./sbgui/glfont2.cpp
	./sbgui/CommandWindow.cpp
	./sbgui/ObjectManipulationHandle.cpp
	./sbgui/PawnPosControl.cpp
	./sbgui/PositionControl.cpp
	./sbgui/RotationControl.cpp
	./sbgui/RootWindow.cxx
	./sbgui/ExportWindow.cpp
	./sbgui/fltk_viewer.cpp
	./sbgui/SBWindowListener.cpp
	./sbgui/SBSelectionManager.cpp
	${OGRE_SRCS}
	./sbgui/pic.cpp
	./sbgui/ppm.cpp
	./sbgui/bmlviewer/BehaviorBlock.cpp
	./sbgui/bmlviewer/BehaviorEditorWidget.cpp
	./sbgui/bmlviewer/BehaviorWindow.cpp
	./sbgui/channelbufferviewer/GlChartView.cpp
	./sbgui/channelbufferviewer/GlChartViewArchive.cpp
	./sbgui/channelbufferviewer/GlChartViewCoordinate.cpp
	./sbgui/channelbufferviewer/channelbufferWindow.cpp
	./sbgui/nle/NonLinearEditor.cpp
	./sbgui/nle/NonLinearEditorWidget.cpp
	./sbgui/panimationviewer/ErrorVisualization.cpp
	./sbgui/panimationviewer/PanimationWindow.cpp
	./sbgui/panimationviewer/ParamAnimBlock.cpp
	./sbgui/panimationviewer/ParamAnimEditorWidget.cpp
	./sbgui/panimationviewer/ParamAnimRunTimeEditor.cpp
	./sbgui/panimationviewer/ParamAnimScriptEditor.cpp
	./sbgui/panimationviewer/ParamAnimStateEditor.cpp
	./sbgui/panimationviewer/ParamAnimTransitionEditor.cpp
	./sbgui/panimationviewer/ParamAnimTransitionEditor2.cpp
	./sbgui/panimationviewer/ParamAnimTransitionCreatorWidget.cpp
	./sbgui/panimationviewer/ParameterGroup.cpp
	./sbgui/panimationviewer/ParameterVisualization.cpp
	./sbgui/panimationviewer/Parameter3DVisualization.cpp
	./sbgui/panimationviewer/ParamAnimStateCreatorWidget.cpp
	./sbgui/panimationviewer/ParamAnimAutoMarkingEditor.cpp
	./sbgui/panimationviewer/ParamAnimParameterEditor.cpp
	./sbgui/panimationviewer/VisualizationView.cpp
	./sbgui/CharacterCreatorWindow.cpp
	./sbgui/bmlcreator/AttributeWindow.cpp
	./sbgui/bmlcreator/TextEditor.cpp
	./sbgui/bmlcreator/BMLAnimationObject.cpp
	./sbgui/bmlcreator/BMLBodyObject.cpp
	./sbgui/bmlcreator/BMLConstraintObject.cpp
	./sbgui/bmlcreator/BMLCreatorWindow.cpp
	./sbgui/bmlcreator/BMLEventObject.cpp
	./sbgui/bmlcreator/BMLFaceObject.cpp
	./sbgui/bmlcreator/BMLGazeObject.cpp
	./sbgui/bmlcreator/BMLGestureObject.cpp
	./sbgui/bmlcreator/BMLHeadObject.cpp
	./sbgui/bmlcreator/BMLLocomotionObject.cpp
	./sbgui/bmlcreator/BMLObject.cpp
	./sbgui/bmlcreator/BMLReachObject.cpp
	./sbgui/bmlcreator/BMLSpeechObject.cpp
	./sbgui/bmlcreator/BMLSaccadeObject.cpp
	./sbgui/bmlcreator/BMLStateObject.cpp

	./sbgui/resourceViewer/Fl_Tree_Horizontal.cpp
	./sbgui/resourceViewer/TreeItemInfoWidget.cpp
	./sbgui/resourceViewer/ResourceWindow.cpp
	./sbgui/resourceViewer/TreeInfoObject.cpp
	./sbgui/resourceViewer/AttributeEditor.cpp

	./sbgui/visemeviewer/VisemeCurve.cpp
	./sbgui/visemeviewer/VisemeCurveEditor.cpp
	./sbgui/visemeviewer/VisemeViewerWindow.cpp
	./sbgui/visemeviewer/VisemeRunTimeWindow.cpp
	./sbgui/visemeviewer/VisemeRTCurveViewer.cpp
	./sbgui/visemeviewer/ImageSequencePlayer.cpp

	./sbgui/retargetviewer/RetargetViewer.cpp
	./sbgui/retargetcreator/RetargetCreatorWindow.cpp
	./sbgui/autorig/SBAutoRigManager.cpp

	./sbgui/monitorviewer/MonitorConnectWindow.cpp 
	./sbgui/motioneditor/MotionEditorWindow.cpp
	./sbgui/jointmapviewer/JointMapViewer.cpp
	./sbgui/jointmapviewer/RetargetStepWindow.cpp

	./sbgui/faceviewer/FaceViewer.cpp
	./sbgui/FLTKListener.cpp
	./sbgui/SBGUIManager.cpp
	
	./sbgui/flu/Flu_Button.cpp
	./sbgui/flu/Flu_Collapsable_Group.cpp
	./sbgui/flu/FluSimpleString.cpp
)

add_executable( sbgui

	${SBM_FLTK_SRC}
)

if(NOT APPLE )
set ( XFT  "")	
else()
set ( XFT "" ) 
endif()

target_link_libraries( sbgui

	SmartBody
	bonebus
	vhmsg
	vhcl
	boost_filesystem
	boost_system
	boost_regex
	boost_python
	boost_serialization
	fltk_images 
	fltk 
	fltk_gl 
	${OPENGL_LIBRARIES}
	activemq-cpp
	python2.7
    	ncurses	
	pthread
)

set( EXEC_LIST

	sbgui
)

set( APP_LIBRARY_BINARY_PATHS

	${PROJECT_BINARY_DIR}/bin
)

set( APP_LIBRARY_SOURCE_PATHS

	${PROJECT_SOURCE_DIR}/bin
)

set_target_properties( 

	${EXEC_LIST} 

	PROPERTIES 

## link with binary tree:
#	INSTALL_RPATH				"${APP_LIBRARY_BINARY_PATHS}"

## link with install tree:
	INSTALL_RPATH				"${APP_LIBRARY_SOURCE_PATHS}"

	BUILD_WITH_INSTALL_RPATH	TRUE
)

install(

  	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)


