from conans import ConanFile, CMake, tools
import os


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
    generators = "cmake"
    source_subfolder = "volumesoffun-polyvox-95f0aa22c12d"

    def source(self):
        tools.get("https://bitbucket.org/volumesoffun/polyvox/get/v{0}.tar.gz".format(self.version))

        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "PROJECT(PolyVox)", """PROJECT(PolyVox)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")

    def build(self):

        if not self.options.shared:
            tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), """SET(LIBRARY_TYPE "DYNAMIC" """,
                                  """SET(LIBRARY_TYPE "STATIC" """)

        cmake = CMake(self)

        cmake.definitions['BUILD_SHARED_LIBS'] = self.options.shared
        cmake.definitions['ENABLE_EXAMPLES'] = "OFF"
        if not tools.os_info.is_windows:
            cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        cmake.configure(source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = [os.path.join("include", "PolyVoxCore")]
