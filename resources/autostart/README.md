# picom autostart

This autostart desktop file is a duplicate of the one installed
in some distributions like Arch, so its installation is disabled by default.

To enable it set `PICOM_ENABLE_AUTOSTART` to `ON` cmake configure option:

```bash
cmake -B build -S .\
-D CMAKE_BUILD_TYPE=Release \
-D CMAKE_INSTALL_PREFIX=/usr \
-D PICOM_ENABLE_AUTOSTART=ON
-W no-dev
```
