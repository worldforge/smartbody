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

        src/sbm/mcontrol_debugger.cpp
)

target_include_directories(SmartBody-bindings-python PUBLIC
        src
        )


target_link_libraries(SmartBody-bindings-python PUBLIC
        SmartBody-debugger
        SmartBody-vhmsg
        SmartBody-bonebus
        SmartBody
        Python3::Python
        )
