import fnmatch
import shutil

from conans import ConanFile, CMake, tools, MSBuild, AutoToolsBuildEnvironment
import os


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
    generators = "cmake"
    source_subfolder = "activemq-cpp"
    build_subfolder = "build_subfolder"
    requires = ["apr/1.7.0", "openssl/1.1.1q", "zlib/1.2.13"]
    exports_sources = ["CMakeLists.txt", "config.tmpl.h", "patches/*"]
    platforms = {"x86": "Win32",
                 "x86_64": "x64"}

    def source(self):
        tools.get("https://github.com/apache/activemq-cpp/archive/activemq-cpp-{0}.tar.gz".format(self.version))
        os.rename("activemq-cpp-activemq-cpp-{0}/activemq-cpp/src/main".format(self.version), self.source_subfolder)
        self._apply_patches('patches', "")

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_folder=self.build_subfolder, source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = [os.path.join("include", "activemq-cpp")]

    def _apply_patches(self, source, dest):
        shutil.copy('CMakeLists.txt', self.source_subfolder)
        shutil.copy('config.tmpl.h', self.source_subfolder)
        for root, _dirnames, filenames in os.walk(source):
            for filename in fnmatch.filter(filenames, '*.patch'):
                print("Applying path {}.".format(filename))
                patch_file = os.path.join(root, filename)
                dest_path = os.path.join(dest, os.path.relpath(root, source))
                tools.patch(base_path=dest_path, patch_file=patch_file)
