#===============================================================================
# Editable project configuration
#
# Essential, non translatable application information (except DESCRIPTION).
# Translatable strings are passed via code.
#===============================================================================
string(TOLOWER ${PROJECT_NAME} PROJECT_ID)
list(APPEND PROJECT_CATEGORIES "Settings;DesktopSettings;Qt;X-XFCE-SettingsDialog;X-XFCE-PersonalSettings") # Freedesktop menu categories
list(APPEND PROJECT_KEYWORDS   "picom;compositor;settings;configurator")
set(PROJECT_AUTHOR_NAME        "Andrea Zanellato")
set(PROJECT_AUTHOR_EMAIL       "redtid3@gmail.com") # Used also for organization email
set(PROJECT_COPYRIGHT_YEAR     "2021-2023")  # TODO: from git
set(PROJECT_DESCRIPTION        "Configure Picom window effects")
set(PROJECT_ORGANIZATION_NAME  "qtilities")  # Might be equal to PROJECT_AUTHOR_NAME
set(PROJECT_ORGANIZATION_URL   "${PROJECT_ORGANIZATION_NAME}.github.io")
set(PROJECT_REPOSITORY_BRANCH  "master")
set(PROJECT_REPOSITORY_URL     "https://github.com/${PROJECT_ORGANIZATION_NAME}/${PROJECT_ID}")
set(PROJECT_HOMEPAGE_URL       "${PROJECT_REPOSITORY_URL}")
set(PROJECT_SPDX_ID            "LGPL-2.1-or-later")
set(PROJECT_TRANSLATIONS_DIR   "resources/translations")
#===============================================================================
# Appstream
#===============================================================================
set(PROJECT_APPSTREAM_SPDX_ID  "CC0-1.0")
set(PROJECT_APPSTREAM_ID       "${PROJECT_ID}")
set(PROJECT_ICON_FORMAT        "freedesktop")
if(UNIX AND NOT APPLE)
    set(PROJECT_ICON_FILE_NAME "preferences-system")
elseif(APPLE)
    # TODO: macOS and Windows
else()
endif()
#===============================================================================
# Adapt to CMake variables
#===============================================================================
set(${PROJECT_NAME}_DESCRIPTION  "${PROJECT_DESCRIPTION}")
set(${PROJECT_NAME}_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
