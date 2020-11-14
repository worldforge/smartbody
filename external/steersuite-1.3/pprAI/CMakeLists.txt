include_directories("include" "../steerlib/include")


add_library(
        pprAI
        src/PPRAIModule.cpp
        src/PPRAgent.cpp
)


set(EXEC_LIST

        pprAI
        )

target_link_libraries(pprAI

        steerlib
        )


#set_target_properties(
#        ${EXEC_LIST}
#        PROPERTIES
#        ## link with install tree:
#        INSTALL_RPATH "${APP_LIBRARY_SOURCE_PATHS}"
#        BUILD_WITH_INSTALL_RPATH TRUE
#)
#
#install(
#        TARGETS ${EXEC_LIST}
#        DESTINATION core/smartbody/sbgui/bin
#
#)

