from conan import ConanFile, tools
from conan.tools.cmake import CMakeDeps, CMakeToolchain, CMake
from conan.tools.files import get, collect_libs, patch, rmdir, rm, copy, export_conandata_patches
from conan.tools.files import replace_in_file
from conan.tools.microsoft import is_msvc
import os

required_conan_version = ">=1.43.0"


class FltkConan(ConanFile):
    name = "fltk"
    version = "1.3.8"
    description = "Fast Light Toolkit is a cross-platform C++ GUI toolkit"
    topics = ("fltk", "gui")
    homepage = "https://www.fltk.org"
    url = "https://github.com/conan-io/conan-center-index"
    license = "LGPL-2.0-custom"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_gl": [True, False],
        "with_threads": [True, False],
        "with_gdiplus": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_gl": True,
        "with_threads": True,
        "with_gdiplus": True,
    }
    user = "smartbody"
    package_type = "library"
    exports = ["CMakeLists.txt"]

    def export_sources(self):
        export_conandata_patches(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['OPTION_BUILD_SHARED_LIBS'] = self.options.shared
        tc.variables['FLTK_BUILD_TEST'] = False
        tc.variables['FLTK_BUILD_EXAMPLES'] = False
        tc.variables['OPTION_USE_XFT'] = False
        tc.variables['OPTION_USE_GL'] = self.options.with_gl
        tc.variables['OPTION_USE_THREADS'] = self.options.with_threads
        tc.variables['OPTION_BUILD_HTML_DOCUMENTATION'] = False
        tc.variables['OPTION_BUILD_PDF_DOCUMENTATION'] = False
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        else:
            del self.options.with_gdiplus

    def configure(self):
        if self.options.shared:
            del self.options.fPIC

    def requirements(self):
        self.requires("zlib/1.2.13")
        self.requires("libjpeg/9c")
        self.requires("libpng/1.6.40")
        if self.settings.os == "Linux":
            self.requires("opengl/system")
            self.requires("glu/system@#ee50fc2ade0e1c0c8ebb3be3ea60e0f5")
            self.requires("fontconfig/2.14.2")
            self.requires("xorg/system")
            self.requires("freetype/2.13.0")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def build(self):
        for apatch in self.conan_data.get("patches", {}).get(self.version, []):
            patch(self, **apatch)

        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, pattern="COPYING", dst=os.path.join(self.package_folder, "licenses"), src=self.source_folder)
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "share"))
        rmdir(self, os.path.join(self.package_folder, "FLTK.framework"))
        rmdir(self, os.path.join(self.package_folder, "CMake"))
        rm(self, "fltk-config*", os.path.join(self.package_folder, "bin"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "fltk")
        self.cpp_info.set_property("cmake_target_name", "fltk::fltk")

        self.cpp_info.names["cmake_find_package"] = "fltk"
        self.cpp_info.names["cmake_find_package_multi"] = "fltk"

        if self.options.shared and self.settings.os == "Windows":
            self.cpp_info.defines.append("FL_DLL")
        self.cpp_info.libs = collect_libs(self)
        if self.settings.os in ("Linux", "FreeBSD"):
            if self.options.with_threads:
                self.cpp_info.system_libs.extend(['pthread', 'dl'])
            if self.options.with_gl:
                self.cpp_info.system_libs.extend(['GL', 'GLU'])
        if self.settings.os == "Macos":
            self.cpp_info.frameworks = ['Cocoa', 'OpenGL', 'IOKit', 'Carbon', 'CoreFoundation', 'CoreVideo']
        if self.settings.os == "Windows":
            self.cpp_info.system_libs = [
                "gdi32",
                "imm32",
                "msimg32",
                "ole32",
                "oleaut32",
                "uuid",
            ]
            if self.options.get_safe("with_gdiplus"):
                self.cpp_info.system_libs.append("gdiplus")
