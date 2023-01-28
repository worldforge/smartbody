from conans import ConanFile
from conans.tools import os_info


# TODO: make this much more modular and make most non-libs dependencies optional.
class SmartBodyConan(ConanFile):
    name = "SmartBody"
    version = "1.0.0"
    settings = "os", "arch", "compiler", "build_type"
    generators = ["cmake"]
    default_options = {
        "boost:without_python": False,
        "*:static": True  # This shouldn't perhaps be hard coded?
    }

    def requirements(self):
        self.requires("ogre/1.12.10@smartbody/stable")
        self.requires("ticpp/2.5.3@smartbody/stable")
        self.requires("recastnavigation/cci.20200511")
        # self.requires("lapack/3.7.1@smartbody/stable")
        self.requires("fltk/1.3.8@smartbody/stable")
        self.requires("ncurses/6.2")
        self.requires("polyvox/0.2.1@smartbody/stable")
        # self.requires("assimp/5.0.1")
        self.requires("stb/20200203")
        self.requires("protobuf/3.21.9")
        self.requires("xerces-c/3.2.4")
        self.requires("glew/2.1.0")
        self.requires("alut/1.1.0@smartbody/stable")
        self.requires("libsndfile/1.0.31")
        self.requires("ODE/0.16.2@smartbody/stable")
        self.requires("activemq-cpp/3.9.5@smartbody/stable")
        self.requires("boost/1.81.0")
        self.requires("zlib/1.2.13")
        self.requires("minizip/1.2.13")
        # self.requires("openssl/1.0.2u")
        self.requires("glm/0.9.9.8")
        self.requires("nanoflann/1.3.2")
        self.requires("eigen/3.3.9")
        # Resolve "'fontconfig/2.13.93' requires 'expat/2.4.6' while 'wayland/1.21.0' requires 'expat/2.5.0'"
        self.requires("expat/2.5.0", override=False)
        # Resolve "'cpython/3.10.0' requires 'openssl/1.1.1l' while 'pulseaudio/14.2' requires 'openssl/1.1.1q'"
        self.requires("openssl/1.1.1q", override=False)
        # Resolve "'cpython/3.10.0' requires 'libffi/3.2.1' while 'wayland/1.21.0' requires 'libffi/3.4.3'"
        self.requires("libffi/3.4.3", override=False)
        # Resolve "'libxft/2.3.6' requires 'freetype/2.12.1' while 'ogre/1.12.10@smartbody/stable' requires 'freetype/2.11.1'"
        self.requires("freetype/2.12.1", override=False)
        # Resolve "'fltk/1.3.8@smartbody/stable' requires 'libpng/1.6.39' while 'freetype/2.12.1' requires 'libpng/1.6.37'"
        self.requires("libpng/1.6.39", override=False)
        if os_info.is_linux:
            # Resolve "'openal/1.22.2' requires 'libalsa/1.2.5.1' while 'sdl/2.26.1' requires 'libalsa/1.2.7.2'"
            self.requires("libalsa/1.2.7.2", override=False)
        if os_info.is_windows:
            self.requires("pthreads4w/3.0.0")


def imports(self):
    self.copy("*.dll", src="bin", dst="bin")
    self.copy("*.dylib", src="lib", dst="bin")
