add_library(SOIL
        image_DXT.c
        image_helper.c
        SOIL.c
        stb_image_aug.c)


target_include_directories(SOIL PUBLIC
        .)
