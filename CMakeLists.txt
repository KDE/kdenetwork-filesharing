cmake_minimum_required(VERSION 3.16)
project(filesharing)

set(QT_MIN_VERSION "6.6.0")
set(KF_MIN_VERSION "5.240.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${ECM_MODULE_PATH})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMDeprecationSettings)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    Network
    DBus
    Qml
    QuickWidgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Auth
    Completion
    CoreAddons
    I18n
    KIO
    WidgetsAddons
)

find_package(PackageKitQt6)
set_package_properties(PackageKitQt6
    PROPERTIES DESCRIPTION "Software Manager integration"
    TYPE OPTIONAL
    PURPOSE "Needed to automatically install the samba package."
)

find_package(QCoro6 REQUIRED COMPONENTS Core)

find_package(net)
set_package_properties(net PROPERTIES
    TYPE RUNTIME
    DESCRIPTION "Samba CLI tool"
    PURPOSE "Runtime dependency to do anything"
)

find_package(testparm)
set_package_properties(testparm PROPERTIES
    TYPE RUNTIME
    DESCRIPTION "Samba CLI tool"
    PURPOSE "Runtime dependency to do anything"
)

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckTypeSize)

if(WIN32)
    set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES})
    set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES})
endif(WIN32)

option(SAMBA_INSTALL "Offer to install Samba for file sharing with PackageKit if it is not already installed, use -DSAMBA_INSTALL=off to disable, use -DSAMBA_PACKAGE_NAME= to set package name." ON)
set(SAMBA_PACKAGE_NAME \"samba\" CACHE STRING "Single package or comma-separated list of packages needed for a functional Samba stack on this distribution.")

if(SAMBA_INSTALL AND PackageKitQt6_FOUND)
    add_definitions(-DSAMBA_INSTALL)
    add_definitions(-DSAMBA_PACKAGE_NAME=${SAMBA_PACKAGE_NAME})
else()
    set(SAMBA_INSTALL false)
endif()
add_feature_info("Samba Installation" SAMBA_INSTALL "Automatic installation of '${SAMBA_PACKAGE_NAME}' using PackageKit.")

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kfileshare\")

add_definitions(
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_NO_FOREACH
    -DQT_NO_KEYWORDS
)

ecm_set_disabled_deprecation_versions(QT 5.15.2
     KF 5.101.0
    )

qcoro_enable_coroutines()

add_subdirectory(samba)

install(FILES org.kde.kdenetwork-filesharing.metainfo.xml
        DESTINATION ${KDE_INSTALL_METAINFODIR})

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
