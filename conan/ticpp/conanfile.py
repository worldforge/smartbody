from conans import ConanFile, CMake, tools
import os
from os import path


class Conan(ConanFile):
    name = "ticpp"
    version = "2.5.3"
    license = "MIT"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://github.com/wxFormBuilder/ticpp"
    description = "TiCPP is short for the official name TinyXML++. It is a completely new interface to TinyXML that uses MANY of the C++ strengths."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    sha1 = "eb79120ea16b847ce9f483a298a394050f463d6b"
    source_subfolder = "ticpp-{}".format(sha1)

    def source(self):
        tools.get("https://github.com/wxFormBuilder/ticpp/archive/{}.zip".format(self.sha1))

        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "project(ticpp)", """project(ticpp)
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

    def package(self):
        self.copy("*.h", dst="include/tinyxml", src=self.source_subfolder)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.so.*", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
