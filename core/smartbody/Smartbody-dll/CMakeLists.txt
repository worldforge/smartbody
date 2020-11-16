add_library(
        SmartBody-c-dll
        src/smartbody-c-dll.cpp
)

target_include_directories(SmartBody-c-dll PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SmartBody-bindings-python,INCLUDE_DIRECTORIES>)

#if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
#    conan_target_link_libraries(SmartBody)
#endif ()

target_link_libraries(SmartBody-c-dll
        Smartbody
        Smartbody-bindings-python
        )
