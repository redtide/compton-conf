# picom-conf

## Overview

picom-conf is a configuration tool for X composite manager [picom][1].

![Screenshot of compton-conf running on awesome-wm](https://i.imgur.com/2fSiAx8.png)

## Installation

### Compiling source code

Runtime dependencies are libconfig and qtbase.
Additional build dependencies are CMake and [liblxqt][2] as well as optionally
Git to pull latest VCS checkouts.

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX`
has to be set to `/usr` on most operating systems.

To build run `make`, to install `make install` which accepts variable `DESTDIR` as usual.


[1]: https://github.com/yshui/picom/
[2]: https://github.com/lxqt/liblxqt/
