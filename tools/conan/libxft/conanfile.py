from conan import ConanFile
from conan.tools.files import apply_conandata_patches, export_conandata_patches, get, rm, rmdir, copy

from conan.tools.gnu import AutotoolsToolchain, Autotools, AutotoolsDeps, PkgConfigDeps
from conan.tools.layout import basic_layout

required_conan_version = ">=1.52.0"


class libxftConan(ConanFile):
    name = "libxft"
    version = "2.3.6"
    description = 'X FreeType library'
    topics = ("libxft", "x11", "xorg")
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://www.x.org/wiki/"
    license = "X11"

    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    user = "smartbody"
    package_type = "library"

    def layout(self):
        basic_layout(self, src_folder="src")

    def export_sources(self):
        export_conandata_patches(self)

    def requirements(self):
        self.requires("xorg/system")
        self.requires("freetype/2.13.0", transitive_headers=True)
        self.requires("fontconfig/2.14.2")

    def build_requirements(self):
        self.build_requires("pkgconf/1.9.3")
        self.build_requires("xorg-macros/1.19.3")
        self.build_requires("libtool/2.4.7")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def configure(self):
        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd
        if self.options.shared:
            del self.options.fPIC

    def generate(self):
        tc = AutotoolsToolchain(self)
        tc.configure_args.extend(["--disable-dependency-tracking"])
        if self.options.shared:
            tc.configure_args.extend(["--disable-static", "--enable-shared"])
        else:
            tc.configure_args.extend(["--disable-shared", "--enable-static"])
        tc.generate()

        deps = AutotoolsDeps(self)
        deps.generate()

        pc = PkgConfigDeps(self)
        pc.generate()

    def build(self):
        apply_conandata_patches(self)
        autotools = Autotools(self)
        autotools.configure()
        autotools.make()

    def package(self):
        copy(self, pattern="LICENSE", dst="licenses", src=self.source_folder)
        copy(self, pattern="COPYING", dst="licenses", src=self.source_folder)
        autotools = Autotools(self)
        autotools.install(args=["-j1"])
        rm(self, "*.la", f"{self.package_folder}/lib", recursive=True)
        rmdir(self, f"{self.package_folder}/lib/pkgconfig")
        rmdir(self, f"{self.package_folder}/share")

    def package_info(self):
        self.cpp_info.names['pkg_config'] = "Xft"
        self.cpp_info.set_property("pkg_config_name", "xft")
        self.cpp_info.libs = ["Xft"]
