import fnmatch
import os
import shutil

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain, CMake
from conan.tools.files import get, collect_libs, patch


class Conan(ConanFile):
    name = "activemq-cpp"
    version = "3.9.5"
    license = "Apache"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://github.com/apache/activemq-cpp"
    description = "ActiveMQ CPP is a messaging library that can use multiple protocols to talk to a MOM (e.g. ActiveMQ)."
    topics = ()
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    requires = ["apr/1.7.0", "openssl/1.1.1q", "zlib/1.2.13"]
    exports_sources = ["CMakeLists.txt", "config.tmpl.h", "patches/*"]
    platforms = {"x86": "Win32",
                 "x86_64": "x64"}
    user = "smartbody"
    package_type = "library"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def layout(self):
        cmake_layout(self, src_folder="activemq-cpp/src/main".format(self.version))

    def source(self):
        get(self, "https://github.com/apache/activemq-cpp/archive/activemq-cpp-{0}.tar.gz".format(self.version),
            strip_root=True, destination=self.folders.base_source)

    def build(self):
        self._apply_patches(self.folders.base_source, self.source_folder)
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.includedirs = [os.path.join("include", "activemq-cpp")]

    def _apply_patches(self, source, dest):
        shutil.copy(os.path.join(source, 'CMakeLists.txt'), dest)
        shutil.copy(os.path.join(source, 'config.tmpl.h'), dest)
        for root, _dirnames, filenames in os.walk(os.path.join(source, 'patches')):
            for filename in fnmatch.filter(filenames, '*.patch'):
                print("Applying patch {}.".format(filename))
                patch_file = os.path.join(root, filename)
                patch(self, base_path=dest, patch_file=patch_file)
