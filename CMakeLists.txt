project(filesharing)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(QT_MIN_VERSION "5.0.0")
set(KF5_MIN_VERSION "5.7.0")
set(ECM_MIN_VERSION "1.6.0")

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    DocTools
    CoreAddons
    I18n
    KIO
    WidgetsAddons
)
find_package(KF5WidgetsAddons)

find_package(PackageKitQt5 0.9.5 REQUIRED)
set_package_properties(PackageKitQt5
    PROPERTIES DESCRIPTION "Software Manager integration"
    TYPE OPTIONAL
    PURPOSE "Provides package management integration to the application launcher."
)

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
#include(CheckPrototypeExists)
include(CheckTypeSize)

set(CMAKE_REQUIRED_DEFINITIONS ${_KDE5_PLATFORM_DEFINITIONS})
if(WIN32)
set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES})
set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES})
endif(WIN32)
add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE5_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE5_INCLUDES})

option(SAMBA_INSTALL "Offer to install Samba for file sharing with PackageKit if it is not already installed, use -DSAMBA_INSTALL=off to disable, use -DSAMBA_PACKAGE_NAME= to set package name." ON)
if(NOT SAMBA_PACKAGE_NAME)
    set(SAMBA_PACKAGE_NAME \"samba\")
endif(NOT SAMBA_PACKAGE_NAME)
if(SAMBA_INSTALL)
    message(STATUS "Samba install feature will be enabled.")
    add_definitions(-DSAMBA_INSTALL)
    add_definitions(-DSAMBA_PACKAGE_NAME=${SAMBA_PACKAGE_NAME})
else(SAMBA_INSTALL)
    message(STATUS "Samba install feature will NOT be enabled.")
endif(SAMBA_INSTALL)

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kfileshare\")

add_subdirectory(samba)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
