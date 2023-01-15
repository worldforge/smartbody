from conans import ConanFile


class SmartBodyConan(ConanFile):
    name = "SmartBody"
    version = "1.0.0"
    settings = "os", "arch", "compiler", "build_type"
    requires = [
        # ois/1.5
        "ogre/1.12.10@smartbody/stable",
        "ticpp/2.5.3@smartbody/stable",
        "recastnavigation/cci.20200511",
        # lapack/3.7.1@smartbody/stable
        "openblas/0.3.20",
        "fltk/1.3.8@smartbody/stable",
        "ncurses/6.2",
        "polyvox/0.2.1@smartbody/stable",
        # assimp/5.0.1
        "stb/20200203",
        "protobuf/3.21.9",
        "cpython/3.10.0",
        "xerces-c/3.2.4",
        "glew/2.1.0",
        "alut/1.1.0@smartbody/stable",
        "libsndfile/1.0.31",
        "ODE/0.16.2@smartbody/stable",
        "activemq-cpp/3.9.5@smartbody/stable",
        "boost/1.81.0",
        "zlib/1.2.13",
        "minizip/1.2.13",
        # openssl/1.0.2u
        "glm/0.9.9.8",
        "nanoflann/1.3.2",
        "eigen/3.3.9",
        # Resolve "'fontconfig/2.13.93' requires 'expat/2.4.6' while 'wayland/1.21.0' requires 'expat/2.5.0'"
        "expat/2.5.0",
        # Resolve "'openal/1.22.2' requires 'libalsa/1.2.5.1' while 'sdl/2.26.1' requires 'libalsa/1.2.7.2'"
        "libalsa/1.2.7.2",
        # Resolve "'cpython/3.10.0' requires 'openssl/1.1.1l' while 'pulseaudio/14.2' requires 'openssl/1.1.1q'"
        "openssl/1.1.1q",
        # Resolve "'cpython/3.10.0' requires 'libffi/3.2.1' while 'wayland/1.21.0' requires 'libffi/3.4.3'"
        "libffi/3.4.3",
        # Resolve "'libxft/2.3.6' requires 'freetype/2.12.1' while 'ogre/1.12.10@smartbody/stable' requires 'freetype/2.11.1'"
        "freetype/2.12.1"]

    generators = ["cmake"]

    default_options = {
        "boost:without_python": False,
        "boost:without_numpy": True,  # numpy seems to fail to build, skip it
        "*:static": True
    }

    def imports(self):
        self.copy("*.dll", src="bin", dst="bin")
        self.copy("*.dylib", src="lib", dst="bin")
