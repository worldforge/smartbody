add_library(
        SmartBody-bml

        src/bml/behavior_scheduler_constant_speed.cpp
        src/bml/behavior_scheduler.cpp
        src/bml/behavior_scheduler_fixed.cpp
        src/bml/behavior_scheduler_gesture.cpp
        src/bml/behavior_span.cpp
        src/bml/bml_animation.cpp
        src/bml/bml_gesture.cpp
        src/bml/bml_bodyreach.cpp
        src/bml/bml_constraint.cpp
        src/bml/bml.cpp
        src/bml/bml_event.cpp
        src/bml/bml_face.cpp
        src/bml/bml_gaze.cpp
        src/bml/bml_general_param.cpp
        src/bml/bml_grab.cpp
        src/bml/bml_interrupt.cpp
        src/bml/bml_processor.cpp
        src/bml/bml_reach.cpp
        src/bml/bml_speech.cpp
        src/bml/bml_sync_point.cpp
        src/bml/bml_target.cpp
        src/bml/bml_saccade.cpp
        src/bml/bml_noise.cpp
        src/bml/bml_states.cpp
        src/bml/BMLAnimationObject.cpp
        src/bml/BMLBodyObject.cpp
        src/bml/BMLConstraintObject.cpp
        src/bml/BMLEventObject.cpp
        src/bml/BMLFaceObject.cpp
        src/bml/BMLGazeObject.cpp
        src/bml/BMLGestureObject.cpp
        src/bml/BMLHeadObject.cpp
        src/bml/BMLLocomotionObject.cpp
        src/bml/BMLObject.cpp
        src/bml/BMLReachObject.cpp
        src/bml/BMLSpeechObject.cpp
        src/bml/BMLSaccadeObject.cpp
        src/bml/BMLHandObject.cpp
        src/bml/BMLStateObject.cpp
        src/bml/BMLNoiseObject.cpp

        src/sb/SBBmlProcessor.cpp)


target_include_directories(SmartBody-bml PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-bml PUBLIC
        SmartBody
        )
