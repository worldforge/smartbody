add_library(
        SmartBody-math
        src/gwiz_cmdl.cpp
        src/gwiz_math.cpp
        src/gwiz_spline.cpp)

target_include_directories(SmartBody-math PUBLIC
        src
        )


target_link_libraries(SmartBody-math
        SmartBody-util)