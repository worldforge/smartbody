include_directories( ./src)

add_definitions( "-DdDOUBLE" )

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 
add_definitions( "-Dfopen64=fopen" )
add_definitions( "-Dfseeko64=fseeko" )
add_definitions( "-Dftello64=ftello" )
endif()



set(SB_EXPORTS 1)
set(SMARTBODY_DLL_EXPORTS 1)
set(SMARTBODY_C_DLL_EXPORTS 1)
set(_COMPILING_STEERLIB 1)


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 


find_library (ACTIVEMQLIB activemq-cpp )
find_library (XERCESLIB xerces-c )
find_library (BOOSTPYTHONLIB boost_python27 )
find_library (BOOSTFILESYSTEMLIB boost_filesystem  )
find_library (BOOSTSYSTEMLIB boost_system  )
find_library (BOOSTREGEXLIB boost_regex )
find_library (ODELIB ode PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (PROTOCLIB protoc PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (PROTOBUFLIB protobuf PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (COREAUDIO_LIBRARY CoreAudio)
find_library (AUDIOUNIT_LIBRARY AudioUnit)
find_library (CORESERVICES_LIBRARY CoreServices)
find_library (LIBSNDFILE sndfile )
find_library (ALUT alut )
find_library (OPENAL openal)
find_library (GLEW GLEW)
else()
find_library (ACTIVEMQLIB activemq-cpp PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (XERCESLIB xerces-c PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (BOOSTPYTHONLIB boost_python PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (BOOSTFILESYSTEMLIB boost_filesystem PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (BOOSTSYSTEMLIB boost_system PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (BOOSTREGEXLIB boost_regex PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (ODELIB ode PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (PROTOCLIB protoc PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (PROTOCLIB protobuf PATHS ${CMAKE_SOURCE_DIR}/lib )
#find_library (PROTOBUFLIB protobuf PATHS ${CMAKE_SOURCE_DIR}/lib )
find_library (LIBSNDFILE sndfile )
find_library (ALUT alut )
find_library (OPENAL openal)
endif()


if (${BUILD_FESTIVAL})
find_library (FESTIVAL Festival ../../../lib/festival/festival/src/lib)
find_library (ESTBASE estbase ../../../lib/festival/speech_tools/lib)
find_library (ESTSTRING eststring ../../..//lib/festival/speech_tools/lib)
find_library (ESTOOLS estools ../../../lib/festival/speech_tools/lib)
endif()



# osx c++ version >= clang 4.2.0
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.2.0))
			add_definitions(-Wno-dangling-else)
			add_definitions(-Wno-deprecated-writable-strings)
			add_definitions(-Wno-format)
			add_definitions(-Wno-implicit-function-declaration)
			add_definitions(-Wno-logical-op-parentheses)
			add_definitions(-Wno-parentheses-equality)
			add_definitions(-Wno-switch)
			add_definitions(-Wno-tautological-compare)
			add_definitions(-Wno-unused-value)
		endif()
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
		if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7.3))
			add_definitions(-Wno-format)
		endif()
	endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
if (${BUILD_FESTIVAL})
set( FESTIVAL_LIBS
	${FESTIVAL}
	${ESTBASE}
	${ESTSTRING}
	${ESTOOLS}
)


else()
set( FESTIVAL_LIBS
)
endif()
set( MORE_LIBS
	${COREAUDIO_LIBRARY}
	${AUDIOUNIT_LIBRARY}
	${CORESERVICES_LIBRARY}
	${LIBSNDFILE}
	${ALUT}
	${OPENAL}
	${GLEW}
	${FESTIVAL_LIBS}
)
else()
if (DEFINED ${FLASCC_ROOT})
set( MORE_LIBS
	rt
	)

else()
set( MORE_LIBS
	${LIBSNDFILE}
	${ALUT}
	${OPENAL}
	rt
)       
endif()
endif()

if (${SB_NO_VHMSG})
set( VHMSG_LIB
)
else()
set( VHMSG_LIB
	vhmsg
	${ACTIVEMQLIB}
)
endif()

if (${SB_NO_BONEBUS})
set( BONEBUS_LIB
)
else()
set( BONEBUS_LIB
	bonebus
)
endif()

if (${SB_NO_PYTHON})
set( PYTHON_LIB
)
else()
set( PYTHON_LIB
	${BOOSTPYTHONLIB}
	python2.7
)
endif()

if (${SB_NO_ODE_PHYSICS})
set( ODE_LIB
)
else()
set( ODE_LIB
	${ODELIB}
)
endif()




find_package(OpenGL REQUIRED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set ( PROTOC ${CMAKE_SOURCE_DIR}/bin/protoc)
else()
set ( PROTOC ${CMAKE_SOURCE_DIR}/bin/protoc)
endif()
add_custom_target(protoctarget
		COMMAND ${PROTOC} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/src/protocols --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/src/protocols ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbmotion.proto 
		COMMAND ${PROTOC} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/src/protocols --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/src/protocols ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbutilities.proto 
		COMMAND ${PROTOC} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/src/protocols --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/src/protocols ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbmesh.proto 
		)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set (SBDLL )
set (GCCLIBS )
else()
set (SBDLL smartbody-dll.c )
set (GCCLIBS gcc gcc_s)

endif()

set( SMARTBODY_LIB_EXT_NANOFLANN_SRC

        ./src/external/nanoflann/nanoflann.hpp

)

set( SMARTBODY_LIB_EXT_RPLY_SRC

        ./src/external/rply/rply.c

)
set( SMARTBODY_LIB_EXT_PARSER_SRC

        ./src/external/parser/Bchart.cpp
        ./src/external/parser/BchartSm.cpp
        ./src/external/parser/Bst.cpp
        ./src/external/parser/ChartBase.cpp
        ./src/external/parser/ClassRule.cpp
        ./src/external/parser/CntxArray.cpp
        ./src/external/parser/CombineBests.cpp
        ./src/external/parser/ECArgs.cpp
        ./src/external/parser/Edge.cpp
        ./src/external/parser/EdgeHeap.cpp
        ./src/external/parser/edgeSubFns.cpp
        ./src/external/parser/EgsFromTree.cpp
        ./src/external/parser/ewDciTokStrm.cpp
        ./src/external/parser/extraMain.cpp
        ./src/external/parser/FBinaryArray.cpp
        ./src/external/parser/Feat.cpp
        ./src/external/parser/Feature.cpp
        ./src/external/parser/FeatureTree.cpp
        ./src/external/parser/fhSubFns.cpp
        ./src/external/parser/Field.cpp
        ./src/external/parser/FullHist.cpp
        ./src/external/parser/GotIter.cpp
        ./src/external/parser/headFinder.cpp
        ./src/external/parser/headFinderCh.cpp
        ./src/external/parser/InputTree.cpp
        ./src/external/parser/Item.cpp
        ./src/external/parser/Link.cpp
        ./src/external/parser/MeChart.cpp
        ./src/external/parser/Params.cpp
        ./src/external/parser/ParseStats.cpp
        ./src/external/parser/SentRep.cpp
        ./src/external/parser/Term.cpp
        ./src/external/parser/TimeIt.cpp
        ./src/external/parser/UnitRules.cpp
        ./src/external/parser/utils.cpp
        ./src/external/parser/ValHeap.cpp
)

set( SMARTBODY_LIB_EXT_ZLIB_SRC
	
	./src/external/zlib-1.2.5/adler32.c
	./src/external/zlib-1.2.5/compress.c
	./src/external/zlib-1.2.5/crc32.c
	./src/external/zlib-1.2.5/deflate.c
	./src/external/zlib-1.2.5/example.c
	./src/external/zlib-1.2.5/gzclose.c
	./src/external/zlib-1.2.5/gzlib.c
	./src/external/zlib-1.2.5/gzread.c
	./src/external/zlib-1.2.5/gzwrite.c
	./src/external/zlib-1.2.5/infback.c
	./src/external/zlib-1.2.5/inffast.c
	./src/external/zlib-1.2.5/inflate.c
	./src/external/zlib-1.2.5/inftrees.c
	./src/external/zlib-1.2.5/ioapi.c
	./src/external/zlib-1.2.5/trees.c
	./src/external/zlib-1.2.5/uncompr.c
	./src/external/zlib-1.2.5/unzip.c
	./src/external/zlib-1.2.5/zip.c
	./src/external/zlib-1.2.5/zutil.c
)

set( SMARTBODY_LIB_EXT_RECAST_SRC

        ./src/external/recast/Recast.cpp
        ./src/external/recast/RecastAlloc.cpp
        ./src/external/recast/RecastArea.cpp
        ./src/external/recast/RecastContour.cpp
        ./src/external/recast/RecastFilter.cpp
        ./src/external/recast/RecastLayers.cpp
        ./src/external/recast/RecastMesh.cpp
        ./src/external/recast/RecastMeshDetail.cpp
        ./src/external/recast/RecastRasterization.cpp
        ./src/external/recast/RecastRegion.cpp
        ./src/external/recast/DetourAlloc.cpp
        ./src/external/recast/DetourCommon.cpp
        ./src/external/recast/DetourNavMesh.cpp
        ./src/external/recast/DetourNavMeshBuilder.cpp
        ./src/external/recast/DetourNavMeshQuery.cpp
        ./src/external/recast/DetourNode.cpp
)

set( SMARTBODY_LIB_EXT_PERLIN_SRC

        ./src/external/perlin/perlin.cpp
)

set( SMARTBODY_LIB_ME_SRC

        ./src/controllers/me_controller_context.cpp
        ./src/controllers/me_controller_context_proxy.cpp
        ./src/controllers/me_controller.cpp
        ./src/controllers/me_controller_tree_root.cpp
        ./src/controllers/me_ct_blend.cpp
        ./src/controllers/me_ct_channel_writer.cpp
        ./src/controllers/me_ct_container.cpp
        ./src/controllers/me_ct_curve_writer.cpp
        ./src/controllers/me_ct_interpolator.cpp
        ./src/controllers/me_ct_motion.cpp
        ./src/controllers/me_ct_periodic_replay.cpp
        ./src/controllers/me_ct_pose.cpp
        ./src/controllers/me_ct_scheduler2.cpp
        ./src/controllers/me_ct_time_shift_warp.cpp
        ./src/controllers/me_ct_unary.cpp
        ./src/controllers/me_default_prune_policy.cpp
        ./src/controllers/me_spline_1d.cpp
        ./src/controllers/me_ct_pose_postprocessing.cpp
        ./src/controllers/me_ct_generic_hand.cpp
)

set( SMARTBODY_LIB_SK_SRC

        ./src/sk/sk_channel_array.cpp
        ./src/sk/sk_channel.cpp
        ./src/sk/sk_joint.cpp
        ./src/sk/sk_joint_euler.cpp
        ./src/sk/sk_joint_pos.cpp
        ./src/sk/sk_joint_quat.cpp
        ./src/sk/sk_joint_swing_twist.cpp
        ./src/sk/sk_motion.cpp
        ./src/sk/sk_motion_io.cpp
        ./src/sk/sk_posture.cpp
        ./src/sk/sk_scene.cpp
        ./src/sk/sk_skeleton.cpp
        ./src/sk/sk_skeleton_io.cpp
        ./src/sk/sk_vec_limits.cpp
)

set( SMARTBODY_LIB_SR_SRC

        ./src/sr/sr_alg.cpp
        ./src/sr/sr_array.cpp
        ./src/sr/sr_box.cpp
        ./src/sr/sr_buffer.cpp
        ./src/sr/sr_camera.cpp
        ./src/sr/sr_color.cpp
        ./src/sr/sr.cpp
        ./src/sr/sr_cylinder.cpp
        ./src/sr/sr_euler.cpp
        ./src/sr/sr_event.cpp
        ./src/sr/sr_geo2.cpp
        ./src/sr/sr_gl.cpp
        ./src/sr/sr_gl_render_funcs.cpp
        ./src/sr/sr_hash_table.cpp
        ./src/sr/sr_input.cpp
        ./src/sr/sr_light.cpp
        ./src/sr/sr_line.cpp
        ./src/sr/sr_lines.cpp
        ./src/sr/sr_mat.cpp
        ./src/sr/sr_material.cpp
        ./src/sr/sr_model.cpp
        ./src/sr/sr_model_export_iv.cpp
        ./src/sr/sr_model_import_obj.cpp
        ./src/sr/sr_model_import_ply.cpp
        ./src/sr/sr_output.cpp
        ./src/sr/sr_path_array.cpp
        ./src/sr/sr_plane.cpp
        ./src/sr/sr_points.cpp
        ./src/sr/sr_polygon.cpp
        ./src/sr/sr_polygons.cpp
        ./src/sr/sr_quat.cpp
        ./src/sr/sr_sa_bbox.cpp
        ./src/sr/sr_sa.cpp
        ./src/sr/sr_random.cpp
        ./src/sr/sr_sa_event.cpp
        ./src/sr/sr_sa_gl_render.cpp
        ./src/sr/sr_sa_render_mode.cpp
        ./src/sr/sr_shared_ptr.cpp
        ./src/sr/sr_sn.cpp
		./src/sr/sr_sn_editor.cpp
        ./src/sr/sr_sn_group.cpp
        ./src/sr/sr_sn_matrix.cpp
        ./src/sr/sr_sn_shape.cpp
        ./src/sr/sr_sn_colorsurf.cpp
        ./src/sr/sr_sphere.cpp
        ./src/sr/sr_spline.cpp
        ./src/sr/sr_string_array.cpp
        ./src/sr/sr_string.cpp
        ./src/sr/sr_timer.cpp
        ./src/sr/sr_trackball.cpp
        ./src/sr/sr_tree.cpp
        ./src/sr/sr_triangle.cpp
        ./src/sr/sr_vec2.cpp
        ./src/sr/sr_vec.cpp
        ./src/sr/sr_viewer.cpp
        ./src/sr/jpge.cpp
        ./src/sr/planner/sk_blend_cfg.cpp
        ./src/sr/planner/sk_pos_planner.cpp
        ./src/sr/planner/sk_blend_cfg_manager.cpp
        ./src/sr/planner/sr_cfg_planner.cpp
        ./src/sr/planner/sk_blend_planner.cpp
        ./src/sr/planner/sr_cfg_manager.cpp
        ./src/sr/planner/sk_pos_cfg.cpp
        ./src/sr/planner/sr_cfg_planner_lazy.cpp
        ./src/sr/planner/sr_cfg_path.cpp
        ./src/sr/planner/sr_cfg_tree.cpp
        ./src/sr/planner/sk_pos_cfg_manager.cpp
)

set( SMARTBODY_LIB_SBM_SRC

        ./src/sbm/BMLDefs.cpp
        ./src/bml/behavior_scheduler_constant_speed.cpp
        ./src/bml/behavior_scheduler.cpp
        ./src/bml/behavior_scheduler_fixed.cpp
		./src/bml/behavior_scheduler_gesture.cpp
        ./src/bml/behavior_span.cpp
        ./src/bml/bml_animation.cpp
        ./src/bml/bml_gesture.cpp
        ./src/bml/bml_bodyreach.cpp
        ./src/bml/bml_constraint.cpp
        ./src/bml/bml.cpp
        ./src/bml/bml_event.cpp
        ./src/bml/bml_face.cpp
        ./src/bml/bml_gaze.cpp
        ./src/bml/bml_general_param.cpp
        ./src/bml/bml_grab.cpp
        ./src/bml/bml_interrupt.cpp
        ./src/bml/bml_locomotion.cpp
        ./src/bml/bml_processor.cpp
        ./src/bml/bml_reach.cpp
        ./src/bml/bml_speech.cpp
        ./src/bml/bml_sync_point.cpp
        ./src/bml/bml_target.cpp
        ./src/bml/bml_saccade.cpp
        ./src/bml/bml_noise.cpp
		./src/bml/bml_states.cpp
		./src/bml/BMLAnimationObject.cpp
		./src/bml/BMLBodyObject.cpp
		./src/bml/BMLConstraintObject.cpp
		./src/bml/BMLEventObject.cpp
		./src/bml/BMLFaceObject.cpp
		./src/bml/BMLGazeObject.cpp
		./src/bml/BMLGestureObject.cpp
		./src/bml/BMLHeadObject.cpp
		./src/bml/BMLLocomotionObject.cpp
		./src/bml/BMLObject.cpp
		./src/bml/BMLReachObject.cpp
		./src/bml/BMLSpeechObject.cpp
		./src/bml/BMLSaccadeObject.cpp
		./src/bml/BMLHandObject.cpp
		./src/bml/BMLStateObject.cpp
		./src/bml/BMLNoiseObject.cpp

        ./src/sbm/GenericViewer.cpp
        ./src/sbm/gwiz_cmdl.cpp
        ./src/sbm/gwiz_math.cpp
        ./src/sbm/gwiz_spline.cpp
        ./src/sbm/Heightfield.cpp
        ./src/sbm/lin_win.cpp
        ./src/sbm/mcontrol_callbacks.cpp
        ./src/sbm/action_unit.cpp

        ./src/controllers/me_ct_basic_locomotion.cpp
        ./src/controllers/me_ct_ccd_IK.cpp
        ./src/controllers/me_ct_constraint.cpp
        ./src/controllers/me_ct_data_driven_reach.cpp
        ./src/controllers/me_ct_data_interpolation.cpp
        ./src/controllers/me_ct_example_body_reach.cpp
        ./src/controllers/me_ct_examples.cpp
        ./src/controllers/me_ct_eyelid.cpp
        ./src/controllers/me_ct_face.cpp
        ./src/controllers/me_ct_gaze_alg.cpp
        ./src/controllers/me_ct_gaze.cpp
        ./src/controllers/me_ct_gaze_joint.cpp
        ./src/controllers/me_ct_gaze_keymap.cpp
        ./src/controllers/me_ct_gaze_target.cpp
        ./src/controllers/me_ct_IK.cpp
        ./src/controllers/me_ct_IK_scenario.cpp
        ./src/controllers/me_ct_jacobian_IK.cpp
        ./src/controllers/me_ct_limb.cpp
        ./src/controllers/me_ct_locomotion_func.cpp
		./src/controllers/me_ct_new_locomotion.cpp
        ./src/controllers/me_ct_motion_example.cpp
        ./src/controllers/me_ct_motion_parameter.cpp
        ./src/controllers/me_ct_motion_player.cpp
        ./src/controllers/me_ct_motion_timewarp.cpp
        ./src/controllers/me_ct_param_animation.cpp
        ./src/controllers/me_ct_param_animation_utilities.cpp
        ./src/controllers/me_ct_reach.cpp
        ./src/controllers/me_ct_hand.cpp
        ./src/controllers/me_ct_motion_profile.cpp
        ./src/controllers/me_ct_barycentric_interpolation.cpp
        ./src/controllers/me_ct_inverse_interpolation.cpp
        ./src/controllers/me_ct_reach_IK.cpp
        ./src/controllers/me_ct_simple_gaze.cpp
        ./src/controllers/me_ct_tether.cpp
        ./src/controllers/me_ct_ublas.cpp
        ./src/controllers/me_ct_breathing.cpp
        ./src/controllers/me_ct_breathing_interface.cpp
        ./src/controllers/me_ct_physics_controller.cpp
        ./src/controllers/me_ct_noise_controller.cpp
        ./src/controllers/me_ct_motion_recorder.cpp
		./src/controllers/me_ct_motion_graph.cpp
		./src/controllers/RealTimeLipSyncController.cpp

        ./src/controllers/MeCtBlendEngine.cpp
        ./src/controllers/me_ct_motion_blend.cpp
        ./src/controllers/MotionAnalysis.cpp

        ./src/sbm/ParserBVH.cpp
        ./src/sbm/ParserASFAMC.cpp
        ./src/sbm/ParserFBX.cpp
        ./src/sbm/ParserOgre.cpp
        ./src/sbm/ParserCOLLADAFast.cpp
        ./src/sbm/ParserOpenCOLLADA.cpp
        ./src/sbm/remote_speech.cpp
        ./src/sbm/sbm_audio.cpp
        ./src/controllers/MeCtBodyReachState.cpp
        ./src/controllers/MeCtReachEngine.cpp
        ./src/controllers/me_ct_saccade.cpp
        ./src/sbm/local_speech.cpp
## UNUSED:
##      sbm/sbm_bml.cpp
        ./src/sbm/sbm_deformable_mesh.cpp
        ./src/sbm/sbm_speech_audiofile.cpp
        ./src/sbm/sbm_speech.cpp
        ./src/sbm/sbm_speech_impl_skeleton.cpp
        ./src/sbm/sbm_test_cmds.cpp
        ./src/sbm/sr_arg_buff.cpp
        ./src/sbm/sr_cmd_line.cpp
        ./src/sbm/sr_cmd_seq.cpp
        ./src/sbm/sr_hash_map.cpp
        ./src/sbm/sr_linear_curve.cpp
        ./src/sbm/sr_spline_curve.cpp
        ./src/sbm/sr_synch_points.cpp
        ./src/sbm/text_speech.cpp
        ./src/sbm/time_profiler.cpp
        ./src/sbm/time_regulator.cpp
        ./src/sbm/xercesc_utils.cpp


        ./src/sbm/ODEPhysicsSim.cpp

        ./src/sbm/GPU/SbmTexture.cpp
        ./src/sbm/GPU/SbmDeformableMeshGPU.cpp
        ./src/sbm/GPU/TBOData.cpp
        ./src/sbm/GPU/VBOData.cpp
        ./src/sbm/GPU/SbmShader.cpp
		./src/sbm/GPU/SbmBlendFace.cpp
        ./src/sbm/PPRAISteeringAgent.cpp
        ./src/sbm/SteerSuiteEnginerDriver.cpp
        ./src/sbm/sr_path_list.cpp

        ./src/sb/sbm_character.cpp
        ./src/sb/sbm_pawn.cpp

        ./src/sb/SBAttribute.cpp
        ./src/sb/SBAttributeManager.cpp
        ./src/sb/SBObject.cpp
        ./src/sb/SBObserver.cpp
        ./src/sb/SBSubject.cpp
        ./src/sb/DefaultAttributeTable.cpp

        ./src/sb/SBScene.cpp
        ./src/sbm/KinectProcessor.cpp
        ./src/controllers/me_ct_data_receiver.cpp

        ./src/sb/SBScript.cpp
        ./src/sb/SBCharacter.cpp
        ./src/sb/SBPawn.cpp
        ./src/sb/SBJoint.cpp
        ./src/sb/SBSkeleton.cpp
        ./src/sb/SBController.cpp
        ./src/sb/SBSimulationManager.cpp
        ./src/sb/SBBmlProcessor.cpp
        ./src/sb/SBAnimationState.cpp
        ./src/sb/SBAnimationTransition.cpp
        ./src/sb/SBAnimationTransitionRule.cpp
        ./src/sb/SBAnimationStateManager.cpp
	./src/sb/SBMotionGraph.cpp
        ./src/sb/SBSteerManager.cpp
        ./src/sb/SBSteerAgent.cpp
        ./src/sb/SBReachManager.cpp
        ./src/sb/SBReach.cpp
        ./src/sb/SBHandSynthesis.cpp
        ./src/sb/SBHandConfiguration.cpp
        ./src/sb/SBHandConfigurationManager.cpp
        ./src/sb/SBGestureMapManager.cpp
        ./src/sb/SBGestureMap.cpp
        ./src/sb/SBServiceManager.cpp
        ./src/sb/SBService.cpp
        ./src/sb/SBMotion.cpp
        ./src/sb/SBFaceDefinition.cpp
        ./src/sb/SBPhysicsManager.cpp
        ./src/sb/SBBoneBusManager.cpp
        ./src/sb/SBCollisionManager.cpp
        ./src/sb/SBParser.cpp
        ./src/sb/SBParseNode.cpp
        ./src/sb/SBJointMap.cpp
        ./src/sb/SBJointMapManager.cpp
        ./src/sb/SBPhoneme.cpp
        ./src/sb/SBPhonemeManager.cpp
        ./src/sb/SBBehaviorSet.cpp
        ./src/sb/SBBehaviorSetManager.cpp
        ./src/sb/SBMotionBlendBase.cpp
        ./src/sb/nvbg.cpp
        ./src/sb/MiniBrain.cpp
        ./src/sb/SBColObject.cpp
        ./src/sb/SBPhysicsSim.cpp
        ./src/sb/SBEvent.cpp
        ./src/sb/PABlend.cpp
        ./src/sb/SBRetarget.cpp
        ./src/sb/SBRetargetManager.cpp
        ./src/sb/SBNavigationMesh.cpp
        ./src/sb/SBAsset.cpp
        ./src/sb/SBAssetManager.cpp
        ./src/sb/SBAsset.cpp
        ./src/sb/SBSpeechManager.cpp
        ./src/sb/SBVHMsgManager.cpp
        ./src/sb/SBCommandManager.cpp
        ./src/sb/SBNavigationMeshManager.cpp

        ./src/sb/SBPython.cpp
        ./src/sb/SBPythonAnimation.cpp
        ./src/sb/SBPythonAttribute.cpp
        ./src/sb/SBPythonCharacter.cpp
        ./src/sb/SBPythonMath.cpp
        ./src/sb/SBPythonMesh.cpp
        ./src/sb/SBPythonMotion.cpp
        ./src/sb/SBPythonScene.cpp
        ./src/sb/SBPythonSimulation.cpp
        ./src/sb/SBPythonSkeleton.cpp
        ./src/sb/SBPythonSystem.cpp
        ./src/sb/SBPythonClass.cpp
		
        ./src/sb/SBAssetHandler.cpp
        ./src/sb/SBAssetHandlerHDR.cpp
        ./src/sb/SBAssetHandlerCOLLADA.cpp
        ./src/sb/SBAssetHandlerSk.cpp
        ./src/sb/SBAssetHandlerSkm.cpp
	./src/sb/SBAssetHandlerAsf.cpp
	./src/sb/SBAssetHandlerAmc.cpp
	./src/sb/SBAssetHandlerOgre.cpp
	./src/sb/SBAssetHandlerObj.cpp
	./src/sb/SBAssetHandlerBvh.cpp
	./src/sb/SBAssetHandlerPly.cpp
        ./src/sb/SBBehavior.cpp
	./src/sb/SBAssetHandlerSkmb.cpp
	./src/sb/SBAssetHandlerSBMeshBinary.cpp
        ./src/sb/SBDebuggerServer.cpp
        ./src/sb/SBDebuggerClient.cpp
        ./src/sb/SBDebuggerUtility.cpp
        ./src/sb/SBRealtimeManager.cpp
	./src/sb/SBRigNode.cpp
	./src/sb/SBUtilities.cpp
        ./src/sbm/SteerPath.cpp
        ./src/sbm/MiscCommands.cpp
		
	${SBDLL}
		
	./src/protocols/sbmotion.pb.cpp
	./src/protocols/sbutilities.pb.cpp
	./src/protocols/sbmesh.pb.cpp
		
)

add_library(
	SmartBody
	SHARED 

	${SMARTBODY_LIB_EXT_SOIL_SRC}

	${SMARTBODY_LIB_EXT_ZLIB_SRC}

        ${SMARTBODY_LIB_EXT_PARSER_SRC}

        ${SMARTBODY_LIB_EXT_RECAST_SRC}

        ${SMARTBODY_LIB_EXT_PERLIN_SRC}

        ${SMARTBODY_LIB_EXT_NANOFLAN_SRC}

        ${SMARTBODY_LIB_EXT_RPLY_SRC}

        ${SMARTBODY_LIB_ME_SRC}

        ${SMARTBODY_LIB_SK_SRC}

        ${SMARTBODY_LIB_SR_SRC}

        ${SMARTBODY_LIB_SBM_SRC}

	

)

if (${GOOGLE_PROFILER})
set (PROFILER_LIBS
	tcmalloc
	profiler
)
else()
set (PROFILER_LIBS "")
endif()

target_link_libraries( SmartBody

	${PROFILER_LIBS}
	${BONEBUS_LIB}
	${VHMSG_LIB}
	${PYTHON_LIB}
	vhcl
	${XERCESLIB}
	${BOOSTFILESYSTEMLIB}
	${BOOSTSYSTEMLIB}
	${BOOSTREGEXLIB}
	blas
	lapack
	${OPENGL_LIBRARIES}
	steerlib
	pprAI
	${ODE_LIB}
	ncurses
	${MORE_LIBS}
	${PROTOCLIB}
	${PROTOBUFLIB}
	${GCCLIBS}
)

add_dependencies(SmartBody protoctarget)

set( EXEC_LIST

       SmartBody
)

set( APP_LIBRARY_SOURCE_PATHS

${CMAKE_SOURCE_DIR}/core/smartbody/sbgui/bin
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
	DESTINATION core/smartbody/sbgui/bin
)

