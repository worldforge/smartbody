from conans import ConanFile, CMake
import os

class TestPackage(ConanFile):
    generators = ['cmake']

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        self.run('bin' + os.sep + 'example', run_environment=True)
