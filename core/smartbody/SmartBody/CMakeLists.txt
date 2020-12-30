if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions("-Dfopen64=fopen")
    add_definitions("-Dfseeko64=fseeko")
    add_definitions("-Dftello64=ftello")
endif ()


set(SB_EXPORTS 1)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIOUNIT_LIBRARY AudioUnit)
    find_library(CORESERVICES_LIBRARY CoreServices)
endif ()


if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MORE_LIBS
            ${COREAUDIO_LIBRARY}
            ${AUDIOUNIT_LIBRARY}
            ${CORESERVICES_LIBRARY}
            )
else ()

    set(MORE_LIBS
            rt
            )
endif ()



set(SMARTBODY_LIB_ME_SRC

        src/controllers/me_controller_context.cpp
        src/controllers/me_controller_context_proxy.cpp
        src/controllers/me_controller.cpp
        src/controllers/me_controller_tree_root.cpp
        src/controllers/me_ct_blend.cpp
        src/controllers/me_ct_channel_writer.cpp
        src/controllers/me_ct_container.cpp
        src/controllers/me_ct_curve_writer.cpp
        src/controllers/me_ct_interpolator.cpp
        src/controllers/me_ct_motion.cpp
        src/controllers/me_ct_periodic_replay.cpp
        src/controllers/me_ct_pose.cpp
        src/controllers/me_ct_scheduler2.cpp
        src/controllers/me_ct_time_shift_warp.cpp
        src/controllers/me_ct_unary.cpp
        src/controllers/me_default_prune_policy.cpp
        src/controllers/me_spline_1d.cpp
        src/controllers/me_ct_pose_postprocessing.cpp
        src/controllers/me_ct_generic_hand.cpp
        )

set(SMARTBODY_LIB_SK_SRC

        src/sk/sk_channel_array.cpp
        src/sk/sk_channel.cpp
        src/sk/sk_joint.cpp
        src/sk/sk_joint_euler.cpp
        src/sk/sk_joint_pos.cpp
        src/sk/sk_joint_quat.cpp
        src/sk/sk_joint_swing_twist.cpp
        src/sk/sk_motion.cpp
        src/sk/sk_motion_io.cpp
        src/sk/sk_posture.cpp
        src/sk/sk_scene.cpp
        src/sk/sk_skeleton.cpp
        src/sk/sk_skeleton_io.cpp
        src/sk/sk_vec_limits.cpp
        )

set(SMARTBODY_LIB_SR_SRC
        src/sr/planner/sk_blend_cfg.cpp
        src/sr/planner/sk_pos_planner.cpp
        src/sr/planner/sk_blend_cfg_manager.cpp
        src/sr/planner/sr_cfg_planner.cpp
        src/sr/planner/sk_blend_planner.cpp
        src/sr/planner/sr_cfg_manager.cpp
        src/sr/planner/sk_pos_cfg.cpp
        src/sr/planner/sr_cfg_planner_lazy.cpp
        src/sr/planner/sr_cfg_path.cpp
        src/sr/planner/sr_cfg_tree.cpp
        src/sr/planner/sk_pos_cfg_manager.cpp
        )

set(SMARTBODY_LIB_SBM_SRC

        src/sbm/BMLDefs.cpp

        src/sbm/Heightfield.cpp
        src/sbm/lin_win.cpp
        src/sbm/action_unit.cpp

        src/controllers/me_ct_basic_locomotion.cpp
        src/controllers/me_ct_ccd_IK.cpp
        src/controllers/me_ct_constraint.cpp
        src/controllers/me_ct_data_driven_reach.cpp
        src/controllers/me_ct_data_interpolation.cpp
        src/controllers/me_ct_example_body_reach.cpp
        src/controllers/me_ct_examples.cpp
        src/controllers/me_ct_eyelid.cpp
        src/controllers/me_ct_face.cpp
        src/controllers/me_ct_gaze_alg.cpp
        src/controllers/me_ct_gaze.cpp
        src/controllers/me_ct_gaze_joint.cpp
        src/controllers/me_ct_gaze_keymap.cpp
        src/controllers/me_ct_gaze_target.cpp
        src/controllers/me_ct_IK.cpp
        src/controllers/me_ct_IK_scenario.cpp
        src/controllers/me_ct_jacobian_IK.cpp
        src/controllers/me_ct_limb.cpp
        src/controllers/me_ct_locomotion_func.cpp
        src/controllers/me_ct_new_locomotion.cpp
        src/controllers/me_ct_motion_example.cpp
        src/controllers/me_ct_motion_parameter.cpp
        src/controllers/me_ct_motion_player.cpp
        src/controllers/me_ct_motion_timewarp.cpp
        src/controllers/me_ct_param_animation.cpp
        src/controllers/me_ct_param_animation_utilities.cpp
        src/controllers/me_ct_reach.cpp
        src/controllers/me_ct_hand.cpp
        src/controllers/me_ct_motion_profile.cpp
        src/controllers/me_ct_barycentric_interpolation.cpp
        src/controllers/me_ct_inverse_interpolation.cpp
        src/controllers/me_ct_reach_IK.cpp
        src/controllers/me_ct_simple_gaze.cpp
        src/controllers/me_ct_tether.cpp
        src/controllers/me_ct_ublas.cpp
        src/controllers/me_ct_breathing.cpp
        src/controllers/me_ct_breathing_interface.cpp
        src/controllers/me_ct_physics_controller.cpp
        src/controllers/me_ct_noise_controller.cpp
        src/controllers/me_ct_motion_recorder.cpp
        src/controllers/me_ct_motion_graph.cpp
        src/controllers/RealTimeLipSyncController.cpp

        src/controllers/MeCtBlendEngine.cpp
        src/controllers/me_ct_motion_blend.cpp
        src/controllers/MotionAnalysis.cpp

        src/sbm/ParserBVH.cpp
        src/sbm/ParserASFAMC.cpp
        src/sbm/ParserFBX.cpp
        src/sbm/remote_speech.cpp
        src/controllers/MeCtBodyReachState.cpp
        src/controllers/MeCtReachEngine.cpp
        src/controllers/me_ct_saccade.cpp
        src/sbm/local_speech.cpp
        src/sbm/sbm_speech_audiofile.cpp
        src/sbm/sbm_speech.cpp
        src/sbm/sbm_speech_impl_skeleton.cpp
        src/sbm/sbm_test_cmds.cpp
        src/sbm/sr_arg_buff.cpp
        src/sbm/sr_cmd_line.cpp
        src/sbm/sr_cmd_seq.cpp
        src/sbm/sr_hash_map.cpp
        src/sbm/sr_linear_curve.cpp
        src/sbm/sr_spline_curve.cpp
        src/sbm/sr_synch_points.cpp
        src/sbm/text_speech.cpp
        src/sbm/time_profiler.cpp
        src/sbm/time_regulator.cpp
        src/sbm/xercesc_utils.cpp


        src/sbm/sr_path_list.cpp

        src/sb/sbm_character.cpp
        src/sb/sbm_pawn.cpp

        src/sb/SBAttribute.cpp
        src/sb/SBAttributeManager.cpp
        src/sb/SBObject.cpp
        src/sb/SBObserver.cpp
        src/sb/SBSubject.cpp
        src/sb/DefaultAttributeTable.cpp

        src/sb/SBScene.cpp
        src/sbm/KinectProcessor.cpp
        src/controllers/me_ct_data_receiver.cpp

        src/sb/SBScript.cpp
        src/sb/SBCharacter.cpp
        src/sb/SBPawn.cpp
        src/sb/SBJoint.cpp
        src/sb/SBSkeleton.cpp
        src/sb/SBController.cpp
        src/sb/SBSimulationManager.cpp
        src/sb/SBAnimationState.cpp
        src/sb/SBAnimationTransition.cpp
        src/sb/SBAnimationTransitionRule.cpp
        src/sb/SBAnimationStateManager.cpp
        src/sb/SBMotionGraph.cpp
#        src/sb/SBSteerManager.cpp
#        src/sb/SBSteerAgent.cpp
        src/sb/SBReachManager.cpp
        src/sb/SBReach.cpp
        src/sb/SBHandSynthesis.cpp
        src/sb/SBHandConfiguration.cpp
        src/sb/SBHandConfigurationManager.cpp
        src/sb/SBGestureMapManager.cpp
        src/sb/SBGestureMap.cpp
        src/sb/SBServiceManager.cpp
        src/sb/SBService.cpp
        src/sb/SBMotion.cpp
        src/sb/SBFaceDefinition.cpp
        src/sb/SBPhysicsManager.cpp
        src/sb/SBCollisionManager.cpp
        src/sb/SBParser.cpp
        src/sb/SBParseNode.cpp
        src/sb/SBJointMap.cpp
        src/sb/SBJointMapManager.cpp
        src/sb/SBPhoneme.cpp
        src/sb/SBPhonemeManager.cpp
        src/sb/SBBehaviorSet.cpp
        src/sb/SBBehaviorSetManager.cpp
        src/sb/SBMotionBlendBase.cpp
        src/sb/nvbg.cpp
        src/sb/MiniBrain.cpp
        src/sb/SBColObject.cpp
        src/sb/SBPhysicsSim.cpp
        src/sb/SBEvent.cpp
        src/sb/PABlend.cpp
        src/sb/SBRetarget.cpp
        src/sb/SBRetargetManager.cpp
        src/sb/SBAsset.cpp
        src/sb/SBAssetManager.cpp
        src/sb/SBAssetStore.cpp
        src/sb/SBSpeechManager.cpp
        src/sb/SBCommandManager.cpp

        src/sb/SBAssetHandler.cpp
        src/sb/SBBehavior.cpp
        src/sb/SBRealtimeManager.cpp
        src/sb/SBRigNode.cpp
        src/sbm/SteerPath.cpp


        )

add_library(
        SmartBody
        ${SMARTBODY_LIB_ME_SRC}
        ${SMARTBODY_LIB_SK_SRC}
        ${SMARTBODY_LIB_SR_SRC}
        ${SMARTBODY_LIB_SBM_SRC}
)

target_include_directories(SmartBody PUBLIC
        src
)

target_link_libraries(SmartBody PUBLIC
        sr
        parser
        perlin
        math
        util
        boost-numeric-bindings
        rapidxml
        vhcl
        ${MORE_LIBS}
        CONAN_PKG::nanoflann
        CONAN_PKG::stb
        CONAN_PKG::xerces-c
        CONAN_PKG::boost
        CONAN_PKG::eigen
        )
