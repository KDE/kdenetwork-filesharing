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

find_package(PackageKitQt5 0.9.5)
set_package_properties(PackageKitQt5
    PROPERTIES DESCRIPTION "Software Manager integration"
    TYPE OPTIONAL
    PURPOSE "Needed to automatically install the samba package."
)

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
#include(CheckPrototypeExists)
include(CheckTypeSize)

if(WIN32)
    set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES})
    set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES})
endif(WIN32)

option(SAMBA_INSTALL "Offer to install Samba for file sharing with PackageKit if it is not already installed, use -DSAMBA_INSTALL=off to disable, use -DSAMBA_PACKAGE_NAME= to set package name." ON)
set(SAMBA_PACKAGE_NAME "samba" CACHE STRING "Name of distribution package to install to get a functional samba stack.")

if(SAMBA_INSTALL AND PackageKitQt5_FOUND)
    add_definitions(-DSAMBA_INSTALL)
    add_definitions(-DSAMBA_PACKAGE_NAME=${SAMBA_PACKAGE_NAME})
else()
    set(SAMBA_INSTALL false)
endif()
add_feature_info("Samba Installation" SAMBA_INSTALL "Automatic installation of '${SAMBA_PACKAGE_NAME}' using PackageKit.")

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kfileshare\")

add_subdirectory(samba)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
