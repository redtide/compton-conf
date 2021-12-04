cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(picom-conf)

set(QT_MINIMUM_VERSION "5.12.0")

option(UPDATE_TRANSLATIONS "Update source translations" OFF)

set(PICOM_CONF_MAJOR_VERSION 0)
set(PICOM_CONF_MINOR_VERSION 17)
set(PICOM_CONF_PATCH_VERSION 0)
set(PICOM_CONF_VERSION ${PICOM_CONF_MAJOR_VERSION}.${PICOM_CONF_MINOR_VERSION}.${PICOM_CONF_PATCH_VERSION})

add_definitions(-DPICOM_CONF_VERSION="${PICOM_CONF_VERSION}")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(LXQT_CMAKE_MODULES_DIR "${CMAKE_MODULE_PATH}/LXQTBuildTools")

include(LXQTBuildTools/LXQtTranslateDesktop)
include(LXQTBuildTools/LXQtTranslateTs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5DBus ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5LinguistTools ${QT_MINIMUM_VERSION} REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED libconfig)
include(GNUInstallDirs REQUIRED)
include(LXQTBuildTools/LXQtCompilerSettings NO_POLICY_SCOPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS")

set(QTX_LIBRARIES Qt5::Widgets Qt5::DBus)

include_directories(
  ${LIBCONFIG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)
set(SOURCE_FILES_
    collapsiblepane.hpp
    collapsiblepane.cpp
    maindialog.h
    maindialog.cpp
    picom-conf.cpp
)
set(UI_FILES_
    maindialog.ui
)
set(DESKTOP_FILES_IN
    picom-conf.desktop.in
)
lxqt_translate_ts(QM_FILES_
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${SOURCE_FILES_}
        ${UI_FILES_}
    INSTALL_DIR
        ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations
)
lxqt_translate_desktop(DESKTOP_FILES SOURCES ${DESKTOP_FILES_IN} USE_YAML)

qt5_wrap_ui(UI_FILES ${UI_FILES_})
set_property(SOURCE ${UI_FILES} PROPERTY SKIP_AUTOMOC ON)

set(PICOM_CONF_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/picom-conf")
set(PICOM_CONF_AUTOSTART_DIR "${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart" CACHE STRING "XDG autostart directory")

add_definitions(-DPICOM_CONF_DATA_DIR="${PICOM_CONF_DATA_DIR}")

add_executable(picom-conf
  ${SOURCE_FILES_}
  ${UI_FILES_}
  ${QM_FILES_}
  ${DESKTOP_FILES}
)
target_link_libraries(picom-conf
  ${QTX_LIBRARIES}
  ${LIBCONFIG_LDFLAGS}
)
install(TARGETS picom-conf RUNTIME DESTINATION bin)
install(FILES picom.conf.example DESTINATION ${CMAKE_INSTALL_DATADIR}/picom-conf)
install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/)

add_subdirectory(autostart)

message(STATUS "
Project name:                   ${CMAKE_PROJECT_NAME}
Version:                        ${PICOM_CONF_VERSION}
Build type:                     ${CMAKE_BUILD_TYPE}
Install prefix:                 ${CMAKE_INSTALL_PREFIX}
Configuration directory:        ${PICOM_CONF_DATA_DIR}
Autostart directory:            ${PICOM_CONF_AUTOSTART_DIR}

Qt version:                     ${Qt5Core_VERSION_STRING}

Compiler CXX debug flags:       ${CMAKE_CXX_FLAGS_DEBUG}
Compiler CXX release flags:     ${CMAKE_CXX_FLAGS_RELEASE}
Compiler CXX min size flags:    ${CMAKE_CXX_FLAGS_MINSIZEREL}
")
