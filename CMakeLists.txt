cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(PicomConf
    VERSION 0.17.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(PICOM_ENABLE_AUTOSTART "Enable picom autostart using DBus [default: OFF]" OFF)
#===============================================================================
# Qt
#===============================================================================
option(PROJECT_TRANSLATIONS_UPDATE "Update source translations [default: OFF]" OFF)
set(PROJECT_TRANSLATION_TEST_ENABLED 0 CACHE STRING "Whether to enable translation testing [default: 0]")
set(PROJECT_TRANSLATION_TEST "it" CACHE STRING "Country code of language to test in IDE [default: it]")
set(PROJECT_QT_VERSION 5 CACHE STRING "Qt version to use [default: 5]")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(QT NAMES Qt${PROJECT_QT_VERSION})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED DBus LinguistTools Widgets)
find_package(Qtilitools REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS")
#===============================================================================
# Other Dependencies
#===============================================================================
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED libconfig)
#===============================================================================
# Project files
#===============================================================================
set(PROJECT_SOURCES
    src/collapsiblepane.hpp
    src/collapsiblepane.cpp
    src/dialogabout.hpp
    src/dialogabout.cpp
    src/maindialog.hpp
    src/maindialog.cpp
    src/picom-conf.cpp
)
set(PROJECT_UI_FILES
    src/maindialog.ui
    src/dialogabout.ui
)
set(PROJECT_OTHER_FILES
    .github/workflows/build.yml
#   .clang-format
    .editorconfig
    .gitattributes
    .gitignore
    README.md
)
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Misc" FILES ${PROJECT_OTHER_FILES})
source_group("UI" FILES ${PROJECT_UI_FILES})
#===============================================================================
# Resources
#===============================================================================
include(Config.cmake)
include(QtAppResources)
#===============================================================================
# Application executable
#===============================================================================
set(PROJECT_ALL_FILES
    ${PROJECT_DESKTOP_FILES}
    ${PROJECT_RESOURCES}
    ${PROJECT_SOURCES}
    ${PROJECT_OTHER_FILES}
    ${PROJECT_QM_FILES}
    ${PROJECT_TRANSLATION_SOURCES}
    ${PROJECT_UI_FILES}
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_ALL_FILES})
else()
    add_executable(${PROJECT_NAME} ${PROJECT_ALL_FILES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${LIBCONFIG_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Widgets
    Qt::DBus
    ${LIBCONFIG_LDFLAGS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_ID}")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APPLICATION_NAME="${PROJECT_ID}"
    APPLICATION_DISPLAY_NAME="${PROJECT_NAME}"
    ORGANIZATION_NAME="${PROJECT_ORGANIZATION_NAME}"
    ORGANIZATION_DOMAIN="${PROJECT_ORGANIZATION_URL}"
    PROJECT_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_ID}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_TRANSLATION_TEST="${PROJECT_TRANSLATION_TEST}"
    PROJECT_TRANSLATION_TEST_ENABLED=${PROJECT_TRANSLATION_TEST_ENABLED}
)
#===============================================================================
# Install application
#===============================================================================
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES "resources/picom.conf.example"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_ID}"
)
#===============================================================================
# XDG Autostart
#===============================================================================
set(PROJECT_AUTOSTART_DIR "${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart")
if(${PICOM_ENABLE_AUTOSTART})
    qtls_translate_desktop(DESKTOP_FILES
        SOURCES         "resources/autostart/picom.desktop.in"
        TRANSLATION_DIR "resources/autostart/translations"
    )
    add_custom_target(autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})
    install(FILES ${DESKTOP_FILES}
        DESTINATION "${PROJECT_AUTOSTART_DIR}"
        COMPONENT   "Runtime"
    )
endif()
#===============================================================================
# Project information
#===============================================================================
message(STATUS "
Project name:                     ${PROJECT_NAME}
Version:                          ${PROJECT_VERSION}
Qt version:                       ${QT_VERSION}
Build type:                       ${CMAKE_BUILD_TYPE}
Install prefix:                   ${CMAKE_INSTALL_PREFIX}
Autostart directory:              ${PROJECT_AUTOSTART_DIR}
Autostart enabled:                ${PICOM_ENABLE_AUTOSTART}
Update translations before build: ${PROJECT_TRANSLATIONS_UPDATE}
")

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
