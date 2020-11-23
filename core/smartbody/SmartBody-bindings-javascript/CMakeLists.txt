add_library(
        SmartBody-bindings-javascript
        src/SBJavascript.cpp
        src/SBJavascriptAnimation.cpp
        src/SBJavascriptAttribute.cpp
        src/SBJavascriptCharacter.cpp
        src/SBJavascriptClass.cpp
        src/SBJavascriptMath.cpp
        src/SBJavascriptMesh.cpp
        src/SBJavascriptMotion.cpp
        src/SBJavascriptScene.cpp
        src/SBJavascriptSimulation.cpp
        src/SBJavascriptSkeleton.cpp
        src/SBJavascriptSystem.cpp
)

target_include_directories(SmartBody-bindings-javascript PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-renderer-gl,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-bindings-javascript
        SmartBody
        SmartBody-renderer-gl
        )
