add_library(vhmsg
        src/HttpUtility.cpp
        src/vhmsg.cpp
        src/vhmsg-tt.cpp
        )

target_include_directories(vhmsg PUBLIC
        include
        $<TARGET_PROPERTY:vhcl,INCLUDE_DIRECTORIES>
        )
