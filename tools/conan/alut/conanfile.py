import fnmatch
import os

from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.files import get, collect_libs, patch
from conan.tools.files import replace_in_file
from conan.tools.microsoft import is_msvc


class AlutConan(ConanFile):
    name = "alut"
    description = "Freealut library for OpenAL"
    version = "1.1.0"
    folder = 'freealut-fc814e316c2bfa6e05b723b8cc9cb276da141aae'
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {"shared": False, "fPIC": True}
    exports = ["CMakeLists.txt", "patches*"]
    requires = (
        "openal/1.22.2"
    )
    url = ""
    license = "https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html"
    package_type = "library"
    user = "smartbody"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['BUILD_EXAMPLES'] = False
        tc.variables['BUILD_TESTS'] = False
        tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = True
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def source(self):
        get(self, "https://github.com/vancegroup/freealut/archive/fc814e316c2bfa6e05b723b8cc9cb276da141aae.zip",
            strip_root=True)
        self.apply_patches('patches', self.source_folder)

    def configure(self):
        if is_msvc(self):
            del self.options.fPIC

    def build(self):
        syslibs = ''
        if self.settings.os == 'Windows':
            syslibs = 'Winmm'
        elif self.settings.os == 'Linux':
            syslibs = 'pthread dl'
        alut_cmakelists = "src/CMakeLists.txt"

        replace_in_file(self, alut_cmakelists,
                        'target_link_libraries(alut ${OPENAL_LIBRARY})',
                        'target_link_libraries(alut ${OPENAL_LIBRARY} %s)' % syslibs)
        if not self.options.shared:
            replace_in_file(self, alut_cmakelists,
                            'add_library(alut SHARED ${ALUT_SOURCES}',
                            'add_library(alut ${ALUT_SOURCES}')
        # There are some issues with copying files on macos...
        replace_in_file(self, alut_cmakelists, 'if(NOT WIN32)', 'if(FALSE)')

        cmake = CMake(self)

        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        if not self.options.shared:
            self.cpp_info.defines = ["ALUT_BUILD_LIBRARY=1"]
        self.cpp_info.libs = collect_libs(self)

    def apply_patches(self, source, dest):
        for root, dirnames, filenames in os.walk(source):
            for filename in fnmatch.filter(filenames, '*.patch'):
                patch_file = os.path.join(root, filename)
                dest_path = os.path.join(dest, os.path.relpath(root, source))
                patch(self, base_path=dest_path, patch_file=patch_file)
