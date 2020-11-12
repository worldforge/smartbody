cmake_minimum_required (VERSION 2.8)
project (elbench)


include_directories(../../../../vhcl/include)
include_directories(../../../../vhmsg/vhmsg-c/include)


if (UNIX)
link_directories(/usr/local/lib)
endif ()


add_executable(elbench elbench.cpp)


if (MSVC)
   target_link_libraries(elbench       ../lib/vhcl)
   target_link_libraries(elbench       ../openal/libs/Win32/OpenAL32)
   target_link_libraries(elbench       ../openal/libs/Win32/alut)
   target_link_libraries(elbench       ../libsndfile-1_0_17/libsndfile-1)
   target_link_libraries(elbench       dbghelp)
endif (MSVC)

if (UNIX)
   target_link_libraries(elbench vhcl)
   target_link_libraries(elbench vhmsg)
   target_link_libraries(elbench activemq-cpp)
endif ()
