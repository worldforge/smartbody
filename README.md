# Smartbody

[![Build all](https://github.com/worldforge/smartbody/actions/workflows/cmake.yml/badge.svg)](https://github.com/worldforge/smartbody/actions/workflows/cmake.yml)

SmartBody is a character animation platform originally developed at
the [USC](http://usc.edu/) [Institute for Creative Technologies](http://ict.usc.edu/).
SmartBody provides locomotion, steering, object manipulation, lip syncing, gazing, nonverbal behavior and retargeting in
real time.

This is a port of the original "Smartbody" project, now managed by
the [Worldforge project](https://www.worldforge.org/). There have been multiple changes to the code base compared to the
original project. However, since the original project hasn't seen any recent development we've retained the name of "
Smartbody".

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

## Components

The project is made up of different libraries, many of which interact. If you're building this with the
intention of including it into a larger external work you probably want to pick and choose which ones.

If you however just want to check it out you should start by building and running the "sbgui" component, which is a
complete suite of SmartBody which includes pretty much all subcomponents as well as a GUI.

## Changes from original project

The original SmartBody project is located at https://smartbody.ict.usc.edu/. There hasn't however been much development
lately. This version of SmartBody is __heavily__ altered, with these changes among others.

* Conan used for all dependency management, making sure it's easily buildable on multiple platforms.
* Extensive refactoring of all components into much smaller modules.
* Removal of OpenGL as a core requirement. Demos and tools still require OpenGL, but the core libs doesn't.
* Fixes of a multitude of memory leaks. Too numerous to list. Code now conforms to C++14 standard.

The main changes are related to the modularization of the package. While previous there was one large component which
provided all functionality there's now multiple smaller ones. This allows us to also separate out the OpenGL
requirements
that previously applies to the core package. It is now possible to rely on SmartBody in an environment where OpenGL
isn't available.

This also means that it's possible to pick and match the components you want. If you don't want BML support for example
that's now optional.

## TODO

A lot of effort has been put into migrating the code from its original state into where it's now there's a lot of work
still to be done.

* The code both compiles and runs on Linux. But while it compiles on Windows it currently (2023-01-28) crashes when it's
  run. Help with getting it to run on Windows would be greatly appreciated.
* MacOS is a valid target, but the new code base has never been built on this platform. As with Windows, help here would
  be appreciated.
* While the code base now is split up into different components there's been no work done in making sure it can easily
  be used as a third party library this way. We provide a bare-bones Conan file which builds everything at all, with all
  dependencies. This should be updated to instead allow for certain components to be built. To exemplify, the current
  Conan setup requires both OGRE and FLTK to be built, while these frameworks would probably not be of interest for
  someone aiming to incorporate Smartbody as a component into a game engine.

## License

SmartBody is licensed under LGPL.

This is a somewhat unfortunate license as it prevents Smartbody to be used in game development. The main issue is that
Smartbody is out of the question for the vast majority of game developers, as it would mean that they then never could
support consoles, unless their games were licenced under the GPL (which is unrealistic).
This is due to the fact that consoles lack the ability to link shared libraries. All code must be statically compiled,
which then requires all code to also be GPL compatible.

If possible a MIT license would be more suitable.
