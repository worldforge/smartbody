include_directories("include" "../steerlib/include")

add_library(
        pprAI
        SHARED
        src/PPRAIModule.cpp
        src/PPRAgent.cpp
)

target_include_directories(pprAI PUBLIC
        include
        $<TARGET_PROPERTY:steerlib,INCLUDE_DIRECTORIES>)

target_link_libraries(pprAI
        steerlib
        )

install(TARGETS pprAI DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})


