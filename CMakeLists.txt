cmake_minimum_required(VERSION 3.2)
project(SmartBody)

include(GNUInstallDirs)


message(STATUS "Using Conan for dependency resolution.")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
#We need to copy all .dll files that Conan installed too.
#TODO: make sure that Conan only builds static libraries on Windows.
if (WIN32)
    file(GLOB DLL_FILES "${CMAKE_BINARY_DIR}/bin/*.dll")
    install(FILES ${DLL_FILES} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif ()

if (NOT WIN32)
    #There's an issue with the Conan provided boost libraries in that they will declare library dependencies as "-l" options
    #rather than links to the ".a" files (as we want static libs). To work around it we need to link directly to the
    #boost filesystem lib, otherwise the system one will be used.
    #This should be removed once it's fixed in the Conan recipes.
    link_libraries("${CONAN_LIB_DIRS_BOOST}/libboost_python38.a")
endif ()


#We don't provide Python though Conan (currently), since the user must have Python in their system to
# run Conan anyway. However, we might look into perhaps doing it anyways (as we then could pin the Python version)
find_package(PythonLibs 3.3 REQUIRED)
link_directories(${PYTHON_LIBRARY_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
link_libraries(${PYTHON_LIBRARIES})



#Make external libs available everywhere.
include_directories(${PROJECT_SOURCE_DIR}/external/boost-numeric-bindings
        ${PROJECT_SOURCE_DIR}/external
        )


set(PROTOC_CMD ${CONAN_BIN_DIRS_PROTOBUF}/protoc)


option(BUILD_STATIC "build static library" ON)
option(BUILD_DYNAMIC "build dynamic library" ON)


#set( CMAKE_INSTALL_PREFIX "../" )
set(BUILD_OGRE TRUE)
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


#add_definitions(	-D__linux__	-fPIC -m32 -Ignore -fpermissive -g3 )
#add_definitions(	-g3 -fPIC -fpermissive -std=c++11)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-O3 -D__APPLE__ -fPIC -fpermissive)
    #add_definitions(	-g3 -D__APPLE__ -fPIC -fpermissive )
else ()
    add_definitions(-O3 -D__linux__ -fPIC -fpermissive -std=c++11)
endif ()
#add_definitions(	-v -W -Wall -Wextra -pedantic )




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

