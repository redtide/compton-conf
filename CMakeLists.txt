cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(picom-conf)

set(LXQTBT_MINIMUM_VERSION "0.8.0")
set(QT_MINIMUM_VERSION "5.12.0")

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)

set(PICOM_CONF_MAJOR_VERSION 0)
set(PICOM_CONF_MINOR_VERSION 16)
set(PICOM_CONF_PATCH_VERSION 0)
set(PICOM_CONF_VERSION ${PICOM_CONF_MAJOR_VERSION}.${PICOM_CONF_MINOR_VERSION}.${PICOM_CONF_PATCH_VERSION})
add_definitions(
    "-DPICOM_CONF_VERSION=\"${PICOM_CONF_VERSION}\""
)

include(LXQtPreventInSourceBuilds)
include(LXQtConfigVars)
include(LXQtTranslateTs)
include(LXQtTranslateDesktop)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5DBus ${QT_MINIMUM_VERSION} REQUIRED)
find_package(Qt5LinguistTools ${QT_MINIMUM_VERSION} REQUIRED)
message(STATUS "Building with Qt${Qt5Core_VERSION_STRING}")

# libconfig using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED
  libconfig
)

include(GNUInstallDirs REQUIRED)
include(LXQtCompilerSettings NO_POLICY_SCOPE) # must be after cmake_minimum_required()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS")

set(QTX_INCLUDE_DIRS "")
set(QTX_LIBRARIES Qt5::Widgets Qt5::DBus)

include_directories(
  ${QTX_INCLUDE_DIRS}
  ${LIBCONFIG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(picom-conf_HS
    picom-conf.h
    maindialog.h
)

set(picom-conf_SRCS
  maindialog.cpp
  picom-conf.cpp
)

set(picom-conf_UIS
  maindialog.ui
)

set(picom-conf_DESKTOP_FILES_IN
    picom-conf.desktop.in
)

lxqt_translate_ts(picom-conf_QM_FILES
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${picom-conf_HS}
        ${picom-conf_SRCS}
        ${picom-conf_UIS}
    INSTALL_DIR
        ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations
)

lxqt_translate_desktop(picom-conf_DESKTOP_FILES SOURCES ${picom-conf_DESKTOP_FILES_IN})

qt5_wrap_ui(picom-conf_UI_H ${picom-conf_UIS})

set_property(SOURCE ${picom-conf_UI_H} PROPERTY SKIP_AUTOMOC ON)

add_definitions(
  -DPICOM_CONF_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/picom-conf"
)

add_executable(picom-conf
  ${picom-conf_SRCS}
  ${picom-conf_UI_H}
  ${picom-conf_QM_FILES}
  ${picom-conf_QM_LOADER}
  ${picom-conf_DESKTOP_FILES}
)

target_link_libraries(picom-conf
  ${QTX_LIBRARIES}
  ${LIBCONFIG_LDFLAGS}
)

install(TARGETS picom-conf RUNTIME DESTINATION bin)
install(FILES picom.conf.example DESTINATION ${CMAKE_INSTALL_DATADIR}/picom-conf)
install(FILES ${picom-conf_DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/)

# merged from lxqt-common
add_subdirectory(autostart)

