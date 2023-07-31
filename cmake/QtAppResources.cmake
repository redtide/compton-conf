#=======================================================================================================
# QtAppResources.cmake
#
# Configure and Installs:
# - About information markdown file
# - Appstream metainfo
# - Desktop file
# - Icons
# - Translations
#
#   TODO: Windows and macOS
#=======================================================================================================
set(PROJECT_RESOURCES
    resources/about.info.md.in
    resources/resources.qrc
)
include(GNUInstallDirs)
#=======================================================================================================
# Configure files
#=======================================================================================================
# "resources/about.md" is a gitignored file configured by cmake and then added
# to the Qt resource file, so not needed in the build directory.
configure_file("resources/about.info.md.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/about.md" @ONLY
)
if (UNIX AND NOT APPLE)
    # TODO :Create a "description.html" file from PROJECT_DESCRIPTION
    #       And insert its content into "PROJECT_APPDATA_FILE", see
    # (https://freedesktop.org/software/appstream/docs/chap-Metadata.html#tag-description)
    # and a git tags string list from function to PROJECT_RELEASES variable
    # (https://freedesktop.org/software/appstream/docs/chap-Metadata.html#tag-releases)
    set(APPDATA_FILE_IN_ "resources/freedesktop/application.appdata.xml.in")
    set(DESKTOP_FILE_IN_ "resources/freedesktop/application.desktop.in")
    set(ICON_FILE_       "resources/icons/application.icon")

    set(PROJECT_APPDATA_FILE_NAME "${PROJECT_APPSTREAM_ID}.appdata.xml")
    set(DESKTOP_FILE_IN "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_ID}.desktop.in")

    list(APPEND PROJECT_RESOURCES ${APPDATA_FILE_IN_})
    list(APPEND PROJECT_RESOURCES ${DESKTOP_FILE_IN_})

    configure_file(${APPDATA_FILE_IN_} "${PROJECT_APPDATA_FILE_NAME}" @ONLY)
    configure_file(${DESKTOP_FILE_IN_} "${DESKTOP_FILE_IN}" @ONLY)

    unset(${APPDATA_FILE_IN_})
    unset(${DESKTOP_FILE_IN_})

    # Application icon might be optional if using only freedesktop theme icons
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ICON_FILE_}")
        list(APPEND PROJECT_RESOURCES ${ICON_FILE_})
        set(PROJECT_ICON_FILE_PATH "${CMAKE_INSTALL_FULL_DATADIR}/icons/hicolor/scalable/apps")
        configure_file(${ICON_FILE_}
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_ICON_FILE_NAME}" COPYONLY
        )
    endif()
    unset(ICON_FILE_)
#=======================================================================================================
# Translations
#=======================================================================================================
    include(Translate)
    qtls_translate(PROJECT_QM_FILES
        SOURCES             ${PROJECT_SOURCES} ${PROJECT_UI_FILES}
        TEMPLATE            "${PROJECT_ID}"
        TRANSLATION_DIR     "${PROJECT_TRANSLATIONS_DIR}"
        INSTALL_DIR         "${CMAKE_INSTALL_DATADIR}/${PROJECT_ID}/translations"
    )
    qtls_translate_desktop(PROJECT_DESKTOP_FILES
        DESKTOP_FILE_STEM   "${PROJECT_APPSTREAM_ID}"
        SOURCES             "${DESKTOP_FILE_IN}"
        TRANSLATION_DIR     "${PROJECT_TRANSLATIONS_DIR}"
    )
    unset(DESKTOP_FILE_IN)
    file(GLOB PROJECT_TRANSLATION_SOURCES "resources/translations/*")
    source_group("Translation Sources" FILES ${PROJECT_TRANSLATION_SOURCES})
#=======================================================================================================
# Install
#=======================================================================================================
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_APPDATA_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo"
    )
    install(FILES "${PROJECT_DESKTOP_FILES}"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    )
endif()
if(EXISTS "resources/icons/application.icon")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_ICON_FILE_NAME}"
        DESTINATION "${PROJECT_ICON_FILE_PATH}"
    )
endif()

source_group("Resource Files" FILES ${PROJECT_RESOURCES})
