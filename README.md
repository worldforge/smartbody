# Smartbody

Smartbody provides tools for simulating human body movement.

This is a port of the original "Smartbody" project. There have been multiple changes to the code base compared to the
original project. However, since the original project hasn't seen any recent development we've retained the name of the
project.


## Install

We use Conan for installation of third party dependencies. Some packages are provided by Conan Center, and some are
provided by th project itself.

To install our own packages, first do
```shell
tools/conan/build_all.sh
```

Once that's done the code itself can be built through
```shell
mkdir build && cd build
conan install .. --build missing
cmake ..
make -j all
```