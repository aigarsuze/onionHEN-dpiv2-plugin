cmake_minimum_required(VERSION 3.20)

project(OnionHENDpiV2Plugin VERSION 1.0.0 LANGUAGES C)

set(ONION_PLUGIN_TARGET dpiv2)
set(ONION_PLUGIN_ID DPIV00001)
set(ONION_PLUGIN_VERSION 1.00)
set(ONION_PLUGIN_NAME "DPI v2")

set(ONIONHEN_PLUGIN_SDK_SOURCE "" CACHE PATH
    "Use a local onionHEN-plugin-sdk checkout instead of downloading it")
set(ONIONHEN_PLUGIN_SDK_GIT_REPOSITORY
    "https://github.com/OnionBuddies/onionHEN-plugin-sdk.git" CACHE STRING
    "OnionHEN Plugin SDK repository")
set(ONIONHEN_PLUGIN_SDK_GIT_TAG
    "0cf4346536705fb271ba0eb2226d97ebcb23559c" CACHE STRING
    "Pinned OnionHEN Plugin SDK revision")

if(NOT ONIONHEN_PLUGIN_SDK_SOURCE AND
   DEFINED ENV{ONIONHEN_PLUGIN_SDK_SOURCE})
    set(ONIONHEN_PLUGIN_SDK_SOURCE "$ENV{ONIONHEN_PLUGIN_SDK_SOURCE}")
endif()

if(ONIONHEN_PLUGIN_SDK_SOURCE)
    if(NOT EXISTS "${ONIONHEN_PLUGIN_SDK_SOURCE}/CMakeLists.txt")
        message(FATAL_ERROR
            "ONIONHEN_PLUGIN_SDK_SOURCE is not an SDK checkout: "
            "${ONIONHEN_PLUGIN_SDK_SOURCE}")
    endif()
    add_subdirectory(
        "${ONIONHEN_PLUGIN_SDK_SOURCE}"
        "${CMAKE_BINARY_DIR}/_deps/onionhen-plugin-sdk-build"
        EXCLUDE_FROM_ALL)
else()
    include(FetchContent)
    FetchContent_Declare(onionhen_plugin_sdk
        GIT_REPOSITORY "${ONIONHEN_PLUGIN_SDK_GIT_REPOSITORY}"
        GIT_TAG "${ONIONHEN_PLUGIN_SDK_GIT_TAG}"
        GIT_SHALLOW FALSE)
    FetchContent_MakeAvailable(onionhen_plugin_sdk)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/plugin_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/plugin_config.h"
    @ONLY)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
file(GLOB DPI_LOCALE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/i18n/*.json")
set(DPI_I18N_CATALOG
    "${CMAKE_CURRENT_BINARY_DIR}/generated/dpi_i18n_catalog.inc")
add_custom_command(
    OUTPUT "${DPI_I18N_CATALOG}"
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_notification_catalog.py"
        --locales-dir "${CMAKE_CURRENT_SOURCE_DIR}/i18n"
        --output "${DPI_I18N_CATALOG}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_notification_catalog.py"
        ${DPI_LOCALE_FILES}
    VERBATIM)

set(PKGSERVER_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pkgserver")
set(PKGSERVER_SOURCES
    "${PKGSERVER_SOURCE_DIR}/pkg_server.c"
    "${PKGSERVER_SOURCE_DIR}/multipart.c")
set(WEBUI_INDEX "${CMAKE_CURRENT_SOURCE_DIR}/webui/dist/index.html")
if(NOT EXISTS "${WEBUI_INDEX}")
    message(FATAL_ERROR
        "DPI WebUI bundle missing: ${WEBUI_INDEX}. Run npm run build in webui/.")
endif()

onion_add_plugin(
    NAME ${ONION_PLUGIN_TARGET}
    TITLE_ID ${ONION_PLUGIN_ID}
    VERSION ${ONION_PLUGIN_VERSION}
    SOURCES
        source/main.c
        source/dpi_i18n.c
        source/dpi_service.c
        source/plugin_descriptor.c
        source/plugin_settings.c
        source/plugin_ui.c
        ${PKGSERVER_SOURCES}
        "${DPI_I18N_CATALOG}"
    INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_BINARY_DIR}/generated"
        "${PKGSERVER_SOURCE_DIR}"
        "${PKGSERVER_SOURCE_DIR}/include"
    LIBRARIES
        SceLibcInternal
        SceSystemService
        SceAppInstUtil
        SceNotification
        SceNetCtl
        SceNet
        kernel_sys)

set_source_files_properties("${PKGSERVER_SOURCE_DIR}/pkg_server.c" PROPERTIES
    COMPILE_DEFINITIONS "ONIONHEN_WEBUI_INDEX=\"${WEBUI_INDEX}\""
    OBJECT_DEPENDS "${WEBUI_INDEX}")
set_source_files_properties(${PKGSERVER_SOURCES} PROPERTIES
    COMPILE_FLAGS
    "-Wno-error -Wno-unused-function -Wno-unused-parameter -Wno-sign-compare")
