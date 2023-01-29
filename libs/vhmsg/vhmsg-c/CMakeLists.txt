add_library(vhmsg INTERFACE)

target_include_directories(vhmsg INTERFACE
        include
        )


add_library(vhmsg-client
        src/HttpUtility.cpp
        src/vhmsg-tt.cpp
        src/vhmsg-client.cpp
        )

target_include_directories(vhmsg-client PUBLIC
        include
        )

target_link_libraries(vhmsg-client PUBLIC
        vhmsg
        vhcl
        activemq-cpp::activemq-cpp)

if (WIN32)
    target_link_libraries(vhmsg-client PUBLIC
            pthreads4w::pthreads4w)
endif ()