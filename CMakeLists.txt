cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(picom-conf
    VERSION 0.17.0
    LANGUAGES CXX
)
include(GNUInstallDirs)
set(PROJECT_AUTOSTART_DIR "${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart" CACHE STRING "XDG autostart directory")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#===============================================================================
# Qt
#===============================================================================
set(PROJECT_QT_VERSION 5 CACHE STRING "Qt version to use: 5 (default) or 6")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(QT NAMES Qt${PROJECT_QT_VERSION})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED DBus LinguistTools Widgets)
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
    src/maindialog.h
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
source_group("Other Files" FILES ${PROJECT_OTHER_FILES})
source_group("UI Files" FILES ${PROJECT_UI_FILES})
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
    ${PROJECT_SOURCES}
    ${PROJECT_OTHER_FILES}
    ${PROJECT_RESOURCES}
    ${PROJECT_QM_FILES}
    ${PROJECT_UI_FILES}
    ${PROJECT_TRANSLATION_SOURCES}
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
)
#===============================================================================
# Install application
#===============================================================================
install(TARGETS ${PROJECT_ID}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(FILES "resources/picom.conf.example"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_ID}"
)
install(FILES ${DESKTOP_FILES}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications/"
)
#===============================================================================
# XDG Autostart
#===============================================================================
qtls_translate_desktop(DESKTOP_FILES
    SOURCES         "resources/autostart/picom.desktop.in"
    TRANSLATION_DIR "resources/autostart/translations"
)
add_custom_target(autostart_desktop_files ALL DEPENDS ${DESKTOP_FILES})
install(FILES ${DESKTOP_FILES}
    DESTINATION "${PROJECT_AUTOSTART_DIR}"
    COMPONENT   "Runtime"
)
#===============================================================================
# Project information
#===============================================================================
message(STATUS "
Project name:                ${PROJECT_NAME}
Version:                     ${PROJECT_VERSION}
Qt version:                  ${QT_VERSION}
Build type:                  ${CMAKE_BUILD_TYPE}
Install prefix:              ${CMAKE_INSTALL_PREFIX}
Configuration directory:     ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_ID}
Autostart directory:         ${PROJECT_AUTOSTART_DIR}

CXX Debug flags:             ${CMAKE_CXX_FLAGS_DEBUG}
CXX Release flags:           ${CMAKE_CXX_FLAGS_RELEASE}
CXX MinSize flags:           ${CMAKE_CXX_FLAGS_MINSIZEREL}
CXX RelWithDebInfo flags:    ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
")

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
