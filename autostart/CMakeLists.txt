#===============================================================================
# Translations
#===============================================================================
file(GLOB DESKTOP_FILES_IN *.desktop.in)
lxqt_translate_desktop(DESKTOP_FILES SOURCES ${DESKTOP_FILES_IN})
add_custom_target(autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})
#===============================================================================
# XDG Autostart
#===============================================================================
install(FILES
    ${DESKTOP_FILES}
    DESTINATION "${PICOM_XDG_CONFIG_DIR}/autostart"
    COMPONENT Runtime
)
