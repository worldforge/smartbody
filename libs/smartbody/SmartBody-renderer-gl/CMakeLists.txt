find_package(glm CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbmesh.pb.cc ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbutilities.pb.cc
        COMMAND ${CMAKE_COMMAND} -E echo "Building Protobuffer bindings"
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --proto_path=. --cpp_out=. sbmesh.proto
        COMMAND ${Protobuf_PROTOC_EXECUTABLE} --proto_path=. --cpp_out=. sbutilities.proto
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbmesh.proto ${CMAKE_CURRENT_SOURCE_DIR}/src/protocols/sbutilities.proto
)


add_library(
        SmartBody-renderer-gl

        src/sb/SBAssetHandlerObj.cpp
        src/sb/SBAssetHandlerOgre.cpp
        src/sb/SBAssetHandlerCOLLADA.cpp
        src/sb/SBAssetHandlerHDR.cpp
        src/sb/SBAssetHandlerPly.cpp
        src/sb/SBAssetHandlerSBMeshBinary.cpp
        src/sb/SBRenderAssetManager.cpp


        src/sbm/SBTextureAssetProcessor.cpp
        src/sbm/SBRenderScene.cpp
        src/sbm/SBRenderSceneListener.cpp
        src/sbm/sbm_deformable_mesh.cpp
        src/sbm/ParserCOLLADAFast.cpp
        src/sbm/ParserOpenCOLLADA.cpp
        src/sbm/ParserOgre.cpp

        src/sbm/GPU/SbmTexture.cpp
        src/sbm/GPU/SbmDeformableMeshGPU.cpp
        src/sbm/GPU/TBOData.cpp
        src/sbm/GPU/VBOData.cpp
        src/sbm/GPU/SbmShader.cpp
        src/sbm/GPU/SbmBlendFace.cpp

        src/sr/sr_camera.cpp
        src/sr/sr_gl.cpp
        src/sr/sr_gl_render_funcs.cpp
        src/sr/sr_model_import_obj.cpp
        src/sr/sr_model_import_ply.cpp
        src/sr/sr_sa_gl_render.cpp
        src/sr/sr_sn_colorsurf.cpp

        src/protocols/sbmesh.pb.cc
        src/protocols/sbutilities.pb.cc

)

target_include_directories(SmartBody-renderer-gl PUBLIC
        src
        )


target_link_libraries(SmartBody-renderer-gl PUBLIC
        rply
        stb::stb
        jpge
        glm::glm
        GLEW::GLEW
        SmartBody-serialize
        )
