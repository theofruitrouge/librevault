from conans import ConanFile, CMake
import os


class LibrevaultConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    #requires = "spdlog/latest@signal9/stable"
    #requires = "docopt.cpp/latest@signal9/stable"
    generators = "cmake"
    #default_options

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
