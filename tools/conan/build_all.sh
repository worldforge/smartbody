#!/usr/bin/env bash
set -e
DIR=$(dirname -- "$0")
if [ "$(uname -s)" == "Linux" ]; then
  conan create "${DIR}"/libxft --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
fi
conan create "${DIR}"/fltk --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan create "${DIR}"/ogre --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan create "${DIR}"/activemq-cpp --build missing
conan create "${DIR}"/alut --build missing
#lapack isn't required and doesn't build on all archs
#conan create lapack --build missing
conan create "${DIR}"/polyvox --build missing
conan create "${DIR}"/ticpp --build missing
