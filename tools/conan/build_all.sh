#!/usr/bin/env bash
set -e
conan create activemq-cpp smartbody/stable --build missing
conan create alut smartbody/stable --build missing
conan create freeimage smartbody/stable --build missing
conan create lapack smartbody/stable --build missing
conan create ODE smartbody/stable --build missing
conan create polyvox smartbody/stable --build missing
conan create ticpp smartbody/stable --build missing
conan create ogre smartbody/stable --build missing
