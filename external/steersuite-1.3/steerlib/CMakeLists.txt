#add_definitions(-DENABLE_GUI)
find_package(ticpp CONFIG REQUIRED)

# osx c++ version >= clang 4.2.0
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.2.0))
            add_definitions(-Wno-tautological-compare)
        endif ()
    endif ()
endif ()


add_library(steerlib
        src/AgentMetricsCollector.cpp
        src/CompositeTechnique02.cpp
        src/PerformanceProfiler.cpp
        src/StateMachine.cpp
        src/BenchmarkEngine.cpp
        src/DrawLib.cpp
        src/RecFilePlayerModule.cpp
        src/SteeringCommand.cpp
        src/BoxObstacle.cpp
        src/DynamicLibrary.cpp
        src/RecFileReader.cpp
        src/TestCasePlayerModule.cpp
        src/Camera.cpp
        src/GridDatabase2D.cpp
        src/RecFileWriter.cpp
        src/TestCaseReader.cpp
        src/CircleObstacle.cpp
        src/HighResCounter.cpp
        src/SimulationEngine.cpp
        src/TestCaseReaderPrivate.cpp
        src/Clock.cpp
        src/MemoryMapper.cpp
        src/SimulationMetricsCollector.cpp
        src/TestCaseWriter.cpp
        src/CommandLineParser.cpp
        src/Misc.cpp
        src/SimulationOptions.cpp
        src/ThreadedTaskManager.cpp
        src/CompositeTechnique01.cpp
        src/OrientedBoxObstacle.cpp
        src/SimulationRecorderModule.cpp
        src/XMLParser.cpp
)

target_include_directories(steerlib PUBLIC
        include
        )




target_link_libraries(steerlib PUBLIC
        mersenne
        ticpp::ticpp
        )
