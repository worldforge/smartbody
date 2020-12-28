add_library(
        math
        src/gwiz_cmdl.cpp
        src/gwiz_math.cpp
        src/gwiz_spline.cpp)

target_include_directories(math PUBLIC
        src
        )


target_link_libraries(math
        util)