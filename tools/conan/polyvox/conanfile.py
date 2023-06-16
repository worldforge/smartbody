import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain, CMake
from conan.tools.files import get, collect_libs, replace_in_file
from conan.tools.microsoft import is_msvc


class Conan(ConanFile):
    name = "polyvox"
    version = "0.2.1"
    license = "MIT"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://bitbucket.org/volumesoffun/polyvox"
    description = "PolyVox is a library for storing, manipulating, and displaying volumetric representations of objects. It is being developed primarily for " \
                  "use in computer games, but is also directly applicable to certain types of scientific and medical visualisation."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    user = "smartbody"
    package_type = "library"

    def source(self):
        get(self, "https://bitbucket.org/volumesoffun/polyvox/get/v{0}.tar.gz".format(self.version), strip_root=True)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['BUILD_SHARED_LIBS'] = self.options.shared
        tc.variables['ENABLE_EXAMPLES'] = "OFF"
        if not is_msvc(self):
            tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        if not self.options.shared:
            replace_in_file(self, os.path.join(self.source_folder, "CMakeLists.txt"), """SET(LIBRARY_TYPE "DYNAMIC" """,
                            """SET(LIBRARY_TYPE "STATIC" """)
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # Seems to be packaged differently on Windows and Linux. Haven't looked closer, this seems to fix it though.
        if self.settings.os == "Linux":
            self.cpp_info.includedirs = [
                os.path.join("include", "PolyVoxCore"),
                os.path.join("include", "PolyVoxUtil")
            ]
            self.cpp_info.libs = collect_libs(self)
        else:
            self.cpp_info.libdirs = [
                "PolyVoxCore/lib",
                "PolyVoxUtil/lib"]
            self.cpp_info.libs = [
                "PolyVoxCore",
                "PolyVoxUtil"]
            self.cpp_info.includedirs = [
                os.path.join("PolyVoxCore", "include"),
                os.path.join("PolyVoxUtil", "include")]
