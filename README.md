# Smartbody

SmartBody is a character animation platform originally developed at
the [USC](http://usc.edu/) [Institute for Creative Technologies](http://ict.usc.edu/).
SmartBody provides locomotion, steering, object manipulation, lip syncing, gazing, nonverbal behavior and retargeting in
real time.

This is a port of the original "Smartbody" project. There have been multiple changes to the code base compared to the
original project. However, since the original project hasn't seen any recent development we've retained the name of the
project.

## Install

We use Conan for installation of third party dependencies. Some packages are provided by Conan Center, and some are
provided by the project itself.

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

## Changes from original project

The original SmartBody project is located at https://smartbody.ict.usc.edu/. There hasn't however been much development
lately. This version of SmartBody is __heavily__ altered, with these changes among others.

* Conan used for all dependency management, making sure it's easily buildable on multiple platforms.
* Extensive refactoring of all components into much smaller modules.
* Removal of OpenGL as a core requirement. Demos and tools still require OpenGL, but the core libs doesn't.
* Fixes of a multitude of memory leaks. Too numerous to list.

The main changes are related to the modularization of the package. While previous there was one large component which
provided all functionality there's now multiple smaller ones. This allows us to also separate out the OpenGL
requirements
that previously applies to the core package. It is now possible to rely on SmartBody in an environment where OpenGL
isn't available.

This also means that it's possible to pick and match the components you want. If you don't want BML support for example
that's now optional.

## License

SmartBody is licensed under LGPL.