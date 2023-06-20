cmake_minimum_required(VERSION 3.2)
#We want to use the first Python version that matches the version constraint.
cmake_policy(SET CMP0094 NEW)
project(SmartBody)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include(GNUInstallDirs)
include(CMakeDependentOption)

#We don't provide Python though Conan (currently), since the user must have Python in their system to
# run Conan anyway. However, we might look into perhaps doing it anyways (as we then could pin the Python version).
#Another reason why we can't currently supply Python through Conan via the CPython dependency is that SmartBody uses
#boost::python for its bindings, and the Conan boost::python package is coded to only work with the system installed
#Python environment. So even if we would interact with a Conan provided CPython environment, the boost::python
#library would be bound to the system one. Which of course results in a festival of linking issues.
#Once/if the Conan provided boost::python installation uses CPython instead this might be changed.
find_package(Python3 COMPONENTS Development REQUIRED)

if (WIN32)
    string(REPLACE .lib .dll PYTHON_DLL ${Python3_LIBRARIES})
    install(FILES ${Python3_RUNTIME_LIBRARY_RELEASE} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif ()

#Gperftools should only be used when developing locally, so we don't provide this through Conan.
find_package(Gperftools 2.5)
cmake_dependent_option(GOOGLE_PROFILER "Use GPerftools for profiling." FALSE "GPERFTOOLS_FOUND" FALSE)
if (GOOGLE_PROFILER)
    message(STATUS "Using GPerftools for profiling")
    include_directories(${GPERFTOOLS_INCLUDE_DIRS})
    link_libraries(${GPERFTOOLS_LIBRARIES})
endif ()


#Temporarily disable the boost warning messages, as they spam the output.
add_compile_definitions(BOOST_ALLOW_DEPRECATED_HEADERS)
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)
add_compile_definitions(BOOST_NO_AUTO_PTR)

if (WIN32)
    #Pinocchio uses the old "hashmap" headers, which MSVC reports as an error. Make it stop.
    add_compile_definitions(_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)
    #We need to define PINOCCHIO_EXPORTS since we inherit from some Pinocchio classes.
    add_compile_definitions(PINOCCHIO_EXPORTS)
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
add_subdirectory(libs)

#Install all data
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/smartbody)

