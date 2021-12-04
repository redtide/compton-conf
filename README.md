# picom-conf

## Overview

picom-conf is a configuration tool for X composite manager [picom][1].

![Main window - shadows](resources/screenshots/picom-conf-1.png)
![Main window - window types](resources/screenshots/picom-conf-2.png)

## Build

Runtime dependencies are Qt5 base and [libconfig][2].
Build dependencies are CMake, Qt5LinguistTools
and optionally Git to pull latest VCS checkouts.

CMake variable `CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -B build
make -C build
make DESTDIR="$(pwd)/package" install -C build
```

## Packages

[![Packaging status](https://repology.org/badge/vertical-allrepos/picom-conf.svg)][3]

## Translations

For contributing translations the [LXQt Weblate Platform][4] can be used.

[![Translation status](https://translate.lxqt-project.org/widgets/redtide/-/picom-conf/multi-auto.svg)][5]


[1]: https://github.com/yshui/picom/
[2]: https://github.com/hyperrealm/libconfig/
[3]: https://repology.org/project/picom-conf/versions
[4]: https://translate.lxqt-project.org/projects/redtide/picom-conf/
[5]: https://translate.lxqt-project.org/engage/redtide/?utm_source=widget
