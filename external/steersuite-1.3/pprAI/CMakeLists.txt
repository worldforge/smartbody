add_library(pprAI
        src/PPRAIModule.cpp
        src/PPRAgent.cpp
        )

target_include_directories(pprAI PUBLIC
        include)

target_link_libraries(pprAI
        steerlib
        )

#install(TARGETS pprAI DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})


