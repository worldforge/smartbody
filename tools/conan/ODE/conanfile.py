from conans import ConanFile, CMake, tools
import os


class Conan(ConanFile):
    name = "ODE"
    version = "0.16.2"
    license = "LGPL-2.1"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://bitbucket.org/odedevs/ode"
    description = "A free, industrial quality library for simulating articulated rigid body dynamics - for example ground vehicles, legged creatures, " \
                  "and moving objects in VR environments. It's fast, flexible & robust. Built-in collision detection."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    source_subfolder = "odedevs-ode-0b5080331d95"

    def source(self):
        tools.get("https://bitbucket.org/odedevs/ode/get/{0}.tar.gz".format(self.version))

        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "project(ODE)", """project(ODE)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")

    def build(self):
        cmake = CMake(self)

        cmake.definitions['BUILD_SHARED_LIBS'] = self.options.shared
        if not tools.os_info.is_windows:
            cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        cmake.configure(source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if not self.settings.os == "Windows":
            self.cpp_info.cxxflags = ["-pthread"]
