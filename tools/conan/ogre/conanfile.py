import os

from conans import ConanFile, CMake, tools
from conans.tools import os_info, SystemPackageTool


class OgreConan(ConanFile):
    name = 'ogre'
    upstream_version = "1.12.10"
    package_revision = ""
    version = "{0}{1}".format(upstream_version, package_revision)

    generators = ['cmake_paths', 'cmake', 'cmake_find_package']
    settings = 'os', 'arch', 'compiler', 'build_type'
    options = {'shared': [True, False]}
    default_options = {"shared": False}
    url = 'https://www.ogre3d.org'
    license = 'MIT'
    description = ("Object-Oriented Graphics Rendering Engine (OGRE) "
                   "is a scene-oriented, real-time, 3D rendering engine.")
    source_subfolder = 'source_subfolder'
    short_paths = False

    def requirements(self):
        self.requires("zlib/1.2.13")
        self.requires("bzip2/1.0.8")
        self.requires("freetype/2.12.1")
        self.requires("freeimage/3.18.0@smartbody/stable")
        self.requires("assimp/5.2.2")
        self.requires("sdl/2.26.1")
        if os_info.is_linux:
            self.requires("xkbcommon/1.5.0")

    def system_requirements(self):
        if os_info.is_linux:
            if os_info.with_apt:
                installer = SystemPackageTool()
                installer.install("libgl1-mesa-dev")
                installer.install("libxrandr-dev")

    def source(self):
        tools.get("https://github.com/OGRECave/ogre/archive/v{0}.tar.gz".format(self.upstream_version))
        os.rename("ogre-{0}".format(self.upstream_version), self.source_subfolder)

    def build(self):

        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "include(OgreConfigTargets)", """include(OgreConfigTargets)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
conan_basic_setup()
""")
        os.rename("Findassimp.cmake", "FindASSIMP.cmake")
        cmake = CMake(self)

        cmake.definitions['OGRE_BUILD_COMPONENT_PYTHON'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_JAVA'] = 'OFF'
        cmake.definitions['OGRE_BUILD_SAMPLES'] = 'OFF'
        cmake.definitions['OGRE_INSTALL_SAMPLES'] = 'OFF'
        cmake.definitions['OGRE_INSTALL_SAMPLES_SOURCE'] = 'OFF'
        cmake.definitions['OGRE_BUILD_TESTS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_TOOLS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_GL'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_GLES2'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_GL3PLUS'] = "ON"
        # Needed for Ogre Bites.
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_D3D9'] = 'ON' if tools.os_info.is_windows else 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_D3D11'] = 'ON' if tools.os_info.is_windows else 'OFF'
        cmake.definitions['OGRE_INSTALL_DOCS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_BSP'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_CG'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_FREEIMAGE'] = 'ON'
        cmake.definitions['OGRE_BUILD_PLUGIN_EXRCODEC'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_OCTREE'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_PCZ'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_PFX'] = 'ON'
        cmake.definitions['OGRE_BUILD_PLUGIN_STBI'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_DOT_SCENE'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_ASSIMP'] = 'ON'
        cmake.definitions['OGRE_BUILD_DEPENDENCIES'] = 'OFF'
        cmake.definitions['OGRE_CONFIG_ENABLE_ZIP'] = "OFF"
        cmake.definitions['OGRE_BUILD_PLATFORM_APPLE_IOS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_CSHARP'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_HLMS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_TERRAIN'] = 'ON'
        cmake.definitions['OGRE_BUILD_COMPONENT_RTSHADERSYSTEM'] = 'ON'
        cmake.definitions['OGRE_BUILD_COMPONENT_VOLUME'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_PYTHON'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_PROPERTY'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_BITES'] = 'ON'
        cmake.definitions['OGRE_BUILD_COMPONENT_OVERLAY'] = 'ON'
        cmake.definitions['OGRE_BUILD_COMPONENT_OVERLAY_IMGUI'] = 'OFF'
        cmake.definitions['OGRE_CONFIG_THREAD_PROVIDER'] = 'std'
        cmake.definitions['OGRE_BUILD_LIBS_AS_FRAMEWORKS'] = 'OFF'
        cmake.definitions['OGRE_RESOURCEMANAGER_STRICT'] = 'true'
        cmake.definitions['OGRE_NODELESS_POSITIONING'] = 'OFF'
        cmake.definitions['OGRE_CONFIG_THREADS'] = '3'
        cmake.definitions['CMAKE_TOOLCHAIN_FILE'] = 'conan_paths.cmake'
        cmake.definitions['OGRE_STATIC'] = not self.options.shared

        if tools.os_info.is_windows:
            cmake.definitions['OGRE_INSTALL_PDB'] = 'OFF'
        else:
            cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        cmake.configure(source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libdirs = ["lib", "lib/OGRE"]
        self.cpp_info.libs = [
            "OgreBitesStatic",
            "Codec_FreeImageStatic",
            "OgreMeshLodGeneratorStatic",
            "OgreOverlayStatic",
            "OgreTerrainStatic",
            "OgrePagingStatic",
            "OgreRTShaderSystemStatic",
            "Plugin_ParticleFXStatic",
            "RenderSystem_GL3PlusStatic",
            "OgreGLSupportStatic",
            "OgreMainStatic",
            ""]
        if tools.os_info.is_windows:
            self.cpp_info.system_libs = ["Opengl32"]
        elif tools.os_info.is_linux:
            self.cpp_info.system_libs = ["GL", "Xaw", "Xt", "X11", "Xrandr"]
        elif tools.os_info.is_macos:
            self.cpp_info.system_libs = []
            self.cpp_info.frameworks = ["IOKit", "Cocoa", "Carbon", "OpenGL", "CoreVideo"]

        self.cpp_info.includedirs = ["include/OGRE",
                                     "include/OGRE/Overlay",
                                     "include/OGRE/Terrain",
                                     "include/OGRE/Paging",
                                     "include/OGRE/MeshLodGenerator",
                                     "include/OGRE/RTShaderSystem"]

    def package(self):
        pass
