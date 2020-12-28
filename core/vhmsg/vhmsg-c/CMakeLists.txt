add_library(vhmsg
        src/HttpUtility.cpp
        src/vhmsg.cpp
        src/vhmsg-tt.cpp
        )

target_include_directories(vhmsg PUBLIC
        include
        )

target_link_libraries(vhmsg PUBLIC
        vhcl
        CONAN_PKG::activemq-cpp)
