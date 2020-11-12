
add_subdirectory( vhcl )

if($(SB_NO_BONEBUS))
else()
add_subdirectory( bonebus )
endif()

if($(SB_NO_VHMSG))
else()
add_subdirectory( vhmsg/vhmsg-c )
endif()

#add_subdirectory( kinecttracker )

if(${SB_NO_ASSIMP})
else()
add_subdirectory( assimp-3.1.1)
endif()

