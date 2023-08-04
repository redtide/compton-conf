# picom-conf

## Overview

picom-conf is a configuration tool for X composite manager [picom].

![Main window - shadows](resources/screenshot.png)
![Main window - window types](resources/screenshot-2.png)

## Dependencies

Runtime:

- Qt5 base
- [libconfig]

Build:

- CMake
- Qt5 Linguist Tools
- [Qtilitools] CMake modules
- Git (optional, to pull latest VCS checkouts)

## Build

`CMAKE_BUILD_TYPE` is usually set to `Release`, though `None` might be a [valid alternative].<br>
`CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.<br>
Using `sudo make install` is discouraged, instead use the system package manager where possible.

```bash
cmake -B build -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr -W no-dev
cmake --build build --verbose
DESTDIR="$(pwd)/package" cmake --install build
```

## Packages

[![Packaging status]](https://repology.org/project/picom-conf/versions)

## Translations

For contributing translations the [LXQt Weblate] platform can be used.

[![Translation status]](https://translate.lxqt-project.org/widgets/qtilities/)


[picom]:              https://github.com/yshui/picom/
[libconfig]:          https://github.com/hyperrealm/libconfig/
[Qtilitools]:         https://github.com/qtilities/qtilitools/
[valid alternative]:  https://wiki.archlinux.org/title/CMake_package_guidelines#Fixing_the_automatic_optimization_flag_override
[Packaging status]:   https://repology.org/badge/vertical-allrepos/picom-conf.svg
[LXQt Weblate]:       https://translate.lxqt-project.org/projects/qtilities/picom-conf/
[Translation status]: https://translate.lxqt-project.org/widgets/qtilities/-/picom-conf/multi-auto.svg
