# picom-conf

## Overview

picom-conf is a configuration tool for the X composite manager [picom][1].

![Main Screen](picom-conf-2.png)

![Main Screen](picom-conf-1.png)



## Installation

### Compiling source code

Runtime dependencies are libconfig and qtbase.
Additional build dependencies are CMake and [liblxqt][2] as well as optionally
Git to pull latest VCS checkouts.

Build dependecies are Qt5LinguistTools, Qt5Widgets and [lxqt-build-tools](https://github.com/lxqt/lxqt-build-tools).

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX`
has to be set to `/usr` on most operating systems.

To build run `make`, to install `make install` which accepts variable `DESTDIR` as usual.

### Translations

For contributing translations  the [LXQt Weblate Platform](https://translate.lxqt-project.org/projects/redtide/picom-conf/) can be used.

<a href="https://translate.lxqt-project.org/engage/redtide/?utm_source=widget">
<img src="https://translate.lxqt-project.org/widgets/redtide/-/picom-conf/multi-auto.svg" alt="Translation status" />
</a>


[1]: https://github.com/yshui/picom/
[2]: https://github.com/lxqt/liblxqt/
