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
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-debugger,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-vhmsg,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-bonebus,INCLUDE_DIRECTORIES>
        ${PYTHON_INCLUDE_DIRS}
        )


target_link_libraries(SmartBody-bindings-python
        SmartBody
        SmartBody-debugger
        SmartBody-vhmsg
        SmartBody-bonebus
        ${PYTHON_LIBRARIES}
        )

target_link_directories(SmartBody-bindings-python PUBLIC
        {PYTHON_LIBRARY_DIRS})
