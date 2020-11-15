import fnmatch

from conans import ConanFile, tools, MSBuild, AutoToolsBuildEnvironment
import os


class Conan(ConanFile):
    name = "activemq-cpp"
    version = "3.9.0"
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
    requires = ["apr/1.7.0", "openssl/1.0.2u", "zlib/1.2.11"]
    exports_sources = ["patches*"]

    def source(self):
        tools.get("https://github.com/apache/activemq-cpp/archive/activemq-cpp-{0}.tar.gz".format(self.version))
        os.rename("activemq-cpp-activemq-cpp-{0}/activemq-cpp".format(self.version), self.source_subfolder)
        self._apply_patches('patches', "")

    def build(self):
        with tools.chdir(self.source_subfolder):
            if self.settings.compiler == "Visual Studio":
                msbuild = MSBuild(self)
                msbuild.build(os.path.join("vs2010-build", "activemq-cpp.sln"),
                              platforms=self.platforms,
                              toolset=self.settings.compiler.toolset)
            else:
                self.run("./autogen.sh")
                autotools = AutoToolsBuildEnvironment(self)
                args = (['--enable-shared', '--disable-static']
                        if self.options.shared else
                        ['--enable-static', '--disable-shared'])
                autotools.configure(args=args)
                autotools.make()
                autotools.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = [os.path.join("include", "activemq-cpp-{}".format(self.version))]

    @staticmethod
    def _apply_patches(source, dest):
        for root, _dirnames, filenames in os.walk(source):
            for filename in fnmatch.filter(filenames, '*.patch'):
                print("Applying path {}.".format(filename))
                patch_file = os.path.join(root, filename)
                dest_path = os.path.join(dest, os.path.relpath(root, source))
                tools.patch(base_path=dest_path, patch_file=patch_file)