from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


# TODO: make this much more modular and make most non-libs dependencies optional.
class SmartBodyConan(ConanFile):
    name = "SmartBody"
    version = "1.0.0"
    settings = "os", "arch", "compiler", "build_type"
    default_options = {
        "boost/*:without_python": False,
        "*:static": True
    }

    def generate(self):
        deps = CMakeDeps(self)
        # OGRE provides its own CMake files which we should use
        #deps.set_property("ogre", "cmake_find_mode", "none")
        deps.generate()

        tc = CMakeToolchain(self)
        # We need to do some stuff differently if Conan is in use, so we'll tell the CMake system this.
        tc.variables["CONAN_FOUND"] = "TRUE"
        tc.generate()

    def requirements(self):
        self.requires("ogre/13.4.2@smartbody")
        self.requires("ticpp/2.5.3@smartbody")
        self.requires("recastnavigation/cci.20200511")
        # self.requires("lapack/3.7.1@smartbody/stable")
        self.requires("fltk/1.3.8@smartbody")
        # self.requires("ncurses/6.2") #Used by FestivalRelay, but there's no good Conan 2 version
        self.requires("polyvox/0.2.1@smartbody")
        # self.requires("assimp/5.0.1")
        self.requires("stb/cci.20220909")
        self.requires("protobuf/3.21.9")
        self.requires("xerces-c/3.2.4")
        self.requires("glew/2.1.0")
        self.requires("alut/1.1.0@smartbody")
        self.requires("libsndfile/1.2.0")
        self.requires("ode/0.16.2")
        self.requires("activemq-cpp/3.9.5@smartbody")
        self.requires("boost/1.81.0")
        self.requires("zlib/1.2.13")
        self.requires("minizip/1.2.13")
        # self.requires("openssl/1.0.2u")
        self.requires("glm/0.9.9.8")
        self.requires("nanoflann/1.3.2")
        self.requires("eigen/3.3.9")
        self.requires("openssl/3.1.1", override=True)

        if self.settings.os == "Windows":
            self.requires("pthreads4w/3.0.0")

    def layout(self):
        cmake_layout(self)