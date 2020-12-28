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
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-bindings-python PUBLIC
        SmartBody
        SmartBody-debugger
        SmartBody-vhmsg
        SmartBody-bonebus
        ${PYTHON_LIBRARIES}
        )

target_link_directories(SmartBody-bindings-python PUBLIC
        {PYTHON_LIBRARY_DIRS})
