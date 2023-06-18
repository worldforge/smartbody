import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake
from conan.tools.files import get, collect_libs, copy
from conan.tools.microsoft import is_msvc


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
    sha1 = "eb79120ea16b847ce9f483a298a394050f463d6b"
    user = "smartbody"
    package_type = "library"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['BUILD_SHARED_LIBS'] = self.options.shared
        if not is_msvc(self):
            tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'
        tc.generate()

    def source(self):
        get(self, "https://github.com/wxFormBuilder/ticpp/archive/{}.zip".format(self.sha1), strip_root=True)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        print(self.build_folder)
        print(os.getcwd())
        copy(self, "*.h", dst=os.path.join(self.package_folder, "include/tinyxml"), src=self.build_folder)
        copy(self, "*.lib", dst=os.path.join(self.package_folder, "lib"), keep_path=False, src=self.build_folder)
        copy(self, "*.a", dst=os.path.join(self.package_folder, "lib"), keep_path=False, src=self.build_folder)
        copy(self, "*.so", dst=os.path.join(self.package_folder, "lib"), keep_path=False, src=self.build_folder)
        copy(self, "*.so.*", dst=os.path.join(self.package_folder, "lib"), keep_path=False, src=self.build_folder)
        copy(self, "*.dylib", dst=os.path.join(self.package_folder, "lib"), keep_path=False, src=self.build_folder)
        copy(self, "*.dll", dst=os.path.join(self.package_folder, "bin"), keep_path=False, src=self.build_folder)
        print("copied")

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
