add_definitions( -DENABLE_GUI )

find_package(OpenGL REQUIRED)

# osx c++ version >= clang 4.2.0
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		if (NOT (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.2.0))
			add_definitions(-Wno-tautological-compare)
		endif()
	endif()
endif()

include_directories ( "." )

add_library(
        steerlib
        SHARED

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

	../external/tinyxml/ticpp.cpp
	../external/tinyxml/tinyxml.cpp
	../external/tinyxml/tinyxmlparser.cpp
	../external/tinyxml/tinystr.cpp
	../external/tinyxml/tinyxmlerror.cpp
)


set( EXEC_LIST

       steerlib
)


target_link_libraries( steerlib
	${OPENGL_LIBRARIES}
	dl
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

