cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(filesharing)

set(QT_MIN_VERSION "5.0.0")
set(KF5_MIN_VERSION "5.7.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
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
    Completion
    CoreAddons
    I18n
    KIO
    WidgetsAddons
)

find_package(PackageKitQt5)
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
include(CheckTypeSize)
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
endif()

if(WIN32)
    set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES})
    set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES})
endif(WIN32)

option(SAMBA_INSTALL "Offer to install Samba for file sharing with PackageKit if it is not already installed, use -DSAMBA_INSTALL=off to disable, use -DSAMBA_PACKAGE_NAME= to set package name." ON)
set(SAMBA_PACKAGE_NAME \"samba\" CACHE STRING "Single package or comma-separated list of packages needed for a functional Samba stack on this distribution.")

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

install(FILES org.kde.kdenetwork-filesharing.metainfo.xml
        DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
