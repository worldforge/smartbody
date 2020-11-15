cmake_minimum_required(VERSION 3.2)
project(SmartBody)

include(GNUInstallDirs)
include(CMakeDependentOption)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    message(STATUS "Using Conan for dependency resolution.")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
    #We need to copy all .dll files that Conan installed too.
    #TODO: make sure that Conan only builds static libraries on Windows.
    if (WIN32)
        file(GLOB DLL_FILES "${CMAKE_BINARY_DIR}/bin/*.dll")
        install(FILES ${DLL_FILES} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
    endif ()

    set(PROTOC_CMD ${CONAN_BIN_DIRS_PROTOBUF}/protoc)
else ()
    message(FATAL_ERROR "Building without Conan is currently not supported.")
endif (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")

#We don't provide Python though Conan (currently), since the user must have Python in their system to
# run Conan anyway. However, we might look into perhaps doing it anyways (as we then could pin the Python version)
find_package(PythonLibs 3.3 REQUIRED)
link_directories(${PYTHON_LIBRARY_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
link_libraries(${PYTHON_LIBRARIES})


#Make external libs available everywhere.
include_directories(
        ${PROJECT_SOURCE_DIR}/external/boost-numeric-bindings
        ${PROJECT_SOURCE_DIR}/external
)

option(BUILD_STATIC "build static library" ON)
option(BUILD_DYNAMIC "build dynamic library" ON)


#TODO: upgrade code to handle newer version of Ogre.
option(BUILD_OGRE "Build OGRE renderer." FALSE)
if (BUILD_OGRE)
    MESSAGE(STATUS "OGRE renderer will be built.")
else ()
    MESSAGE(STATUS "OGRE renderer will not be built.")
endif (BUILD_OGRE)

set(GOOGLE_PROFILER FALSE)
set(BUILD_FESTIVAL FALSE)

# uses the Assimp asset importer
#set(SB_NO_ASSIMP TRUE)

# uses Python as a scripting language
#set( SB_NO_PYTHON FALSE)

# uses the Virtual Human Message system
#set( SB_NO_VHMSG TRUE)

# uses the bonebus network 
#set( SB_NO_BONEBUS TRUE)

# uses the Open Dynamics Engine (ODE) physics library
#set( SB_NO_ODE_PHYSICS TRUE)


if (${SB_NO_VHMSG})
    add_definitions(-DSB_NO_VHMSG)
    set(VHMSG_INCLUDE
            )
else ()
    set(VHMSG_INCLUDE
            lib/vhmsg/vhmsg-c/include
            )
endif ()

if (${SB_NO_BONEBUS})
    add_definitions(-DSB_NO_BONEBUS)
    set(BONEBUS_INCLUDE
            )
else ()
    set(BONEBUS_INCLUDE
            lib/bonebus/include
            )
endif ()

if (${SB_NO_PYTHON})
    add_definitions(-DSB_NO_PYTHON)
endif ()

if (${SB_NO_ODE_PHYSICS})
    add_definitions(-DSB_NO_ODE_PHYSICS)
endif ()

include_directories(
        ${VHMSG_INCLUDE}
        ${BONEBUS_INCLUDE}
        lib/vhcl/include
        include
)


add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(core)

#Install all data
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/smartbody)

