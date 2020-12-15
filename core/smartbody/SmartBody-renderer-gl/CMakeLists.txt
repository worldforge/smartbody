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
        src/sbm/SceneExporter.cpp

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
)

target_include_directories(SmartBody-renderer-gl PUBLIC
        src
        $<TARGET_PROPERTY:SmartBody,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:sr,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:rply,INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:SOIL,INCLUDE_DIRECTORIES>
        )


target_link_libraries(SmartBody-renderer-gl PUBLIC
        SmartBody
        sr
        rply
        SOIL
        )
