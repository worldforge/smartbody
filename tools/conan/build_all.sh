#!/usr/bin/env bash
set -e
#We would want to use the version of fltk provided by Conan Center, but that generates linker issues.
conan create fltk --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan create freeimage --build missing
conan create ogre --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan create ODE --build missing
conan create activemq-cpp --build missing
conan create alut --build missing
#lapack isn't required and doesn't build on all archs
#conan create lapack --build missing
conan create polyvox --build missing
conan create ticpp --build missing

