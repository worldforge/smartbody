cmake_minimum_required(VERSION 3.2)
project(SmartBody)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include(GNUInstallDirs)
include(CMakeDependentOption)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    message(STATUS "Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
    #We need to copy all .dll files that Conan installed too.
    #TODO: make sure that Conan only builds static libraries on Windows.
    if (WIN32)
        file(GLOB DLL_FILES "${CMAKE_BINARY_DIR}/bin/*.dll")
        install(FILES ${DLL_FILES} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
    endif ()

    #We need to call on the "protoc" command to regenerate from the .proto files when they change.
    set(PROTOC_CMD ${CONAN_BIN_DIRS_PROTOBUF}/protoc)
else ()
    #TODO: check for correct libs so distros can package this.
    message(FATAL_ERROR "Building without Conan is currently not supported.")
endif (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")

#We don't provide Python though Conan (currently), since the user must have Python in their system to
# run Conan anyway. However, we might look into perhaps doing it anyways (as we then could pin the Python version)
find_package(PythonLibs 3.3 REQUIRED)


#Gperftools should only be used when developing locally, so we don't provide this through Conan.
find_package(Gperftools 2.5)
cmake_dependent_option(GOOGLE_PROFILER "Use GPerftools for profiling." FALSE "GPERFTOOLS_FOUND" FALSE)
if (GOOGLE_PROFILER)
    message(STATUS "Using GPerftools for profiling")
    include_directories(${GPERFTOOLS_INCLUDE_DIRS})
    link_libraries(${GPERFTOOLS_LIBRARIES})
endif()


#Temporarily disable the boost warning messages, as they spam the output.
add_compile_definitions(BOOST_ALLOW_DEPRECATED_HEADERS)
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)
add_compile_definitions(BOOST_NO_AUTO_PTR)

if (WIN32)
    #Pinocchio uses the old "hashmap" headers, which MSVC reports as an error. Make it stop.
    add_compile_definitions(_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)
endif ()

#We need to load plugins
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#TODO: upgrade code to handle newer version of Ogre.
option(BUILD_OGRE "Build OGRE renderer." FALSE)
if (BUILD_OGRE)
    MESSAGE(STATUS "OGRE renderer will be built.")
else ()
    MESSAGE(STATUS "OGRE renderer will not be built.")
endif (BUILD_OGRE)


set(BUILD_FESTIVAL FALSE)

# uses the Assimp asset importer
#set(SB_NO_ASSIMP TRUE)


add_subdirectory(external)
add_subdirectory(core)

#Install all data
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/smartbody)

