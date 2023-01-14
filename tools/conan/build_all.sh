#!/usr/bin/env bash
set -e
#We would want to use the version of fltk provided by Conan Center, but that generates linker issues.
conan create fltk smartbody/stable --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan create freeimage smartbody/stable --build missing
conan create ogre smartbody/stable --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan create ODE smartbody/stable --build missing
conan create activemq-cpp smartbody/stable --build missing
conan create alut smartbody/stable --build missing
conan create lapack smartbody/stable --build missing
conan create polyvox smartbody/stable --build missing
conan create ticpp smartbody/stable --build missing

