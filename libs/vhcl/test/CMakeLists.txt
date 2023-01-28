cmake_minimum_required (VERSION 2.6)
project (test)

# build vhcl
add_subdirectory(.. ..)


include_directories(../include)


find_library (LIBSNDFILE sndfile )
find_library (ALUT alut )
find_library (OPENAL openal)


add_executable(test test.cpp testempty.cpp)


if (MSVC)
   target_link_libraries(test       ../lib/vhcl)
   target_link_libraries(test       ../openal/libs/Win32/OpenAL32)
   target_link_libraries(test       ../openal/libs/Win32/alut)
   target_link_libraries(test       ../libsndfile-1_0_17/libsndfile-1)
   target_link_libraries(test       dbghelp)
endif (MSVC)

if (UNIX)
   target_link_libraries(test vhcl)
   target_link_libraries(test ${ALUT})
   target_link_libraries(test ${LIBSNDFILE})
   target_link_libraries(test ${OPENAL})
endif ()
