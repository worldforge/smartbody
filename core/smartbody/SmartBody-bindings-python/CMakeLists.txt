add_library(
        SmartBody-bindings-python
        src/SBPython.cpp
        src/SBPythonAnimation.cpp
        src/SBPythonAttribute.cpp
        src/SBPythonCharacter.cpp
        src/SBPythonMath.cpp
        src/SBPythonMesh.cpp
        src/SBPythonMotion.cpp
        src/SBPythonScene.cpp
        src/SBPythonSimulation.cpp
        src/SBPythonSkeleton.cpp
        src/SBPythonSystem.cpp
        src/SBPythonClass.cpp
)

target_include_directories(SmartBody-bindings-python PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-debugger,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-bindings-python
        SmartBody
        SmartBody-debugger
        )
