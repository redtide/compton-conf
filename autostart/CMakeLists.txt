#===============================================================================
# Translations
#===============================================================================
file(GLOB DESKTOP_FILES_IN *.desktop.in)
lxqt_translate_desktop(DESKTOP_FILES SOURCES ${DESKTOP_FILES_IN} USE_YAML)
add_custom_target(autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})
#===============================================================================
# XDG Autostart
#===============================================================================
install(FILES
    ${DESKTOP_FILES}
    DESTINATION "${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart"
    COMPONENT Runtime
)
