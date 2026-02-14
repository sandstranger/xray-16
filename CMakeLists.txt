cmake_minimum_required(VERSION 3.23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(PreProjectInit)

calculate_xray_build_id(XRAY_BUILD_ID)
project(OpenXRay
    DESCRIPTION "OpenXRay is an improved version of the X-Ray Engine, \
                the game engine used in the world-famous S.T.A.L.K.E.R. game series by GSC Game World."
    VERSION 1.6.02.${XRAY_BUILD_ID}
    HOMEPAGE_URL "https://github.com/OpenXRay/xray-16"
    LANGUAGES CXX C
)

if (NOT ANDROID)
    option(BUILD_SHARED_LIBS "Build shared libraries"     ${BUILD_SHARED_LIBS_DEFAULT_VALUE})
else ()
    option(BUILD_SHARED_LIBS "Build shared libraries"     OFF)
endif ()

option(XRAY_USE_LUAJIT   "Use LuaJIT"                 ON)
option(XRAY_USE_ASAN     "Use AddressSanitizer"       OFF)
option(XRAY_ENABLE_TRACY "Enable tracy profiler"      OFF)

include(XRay.Build)
include(XRay.Packaging)

if (ANDROID)
    set(XRAY16_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(Externals)
    include_directories(${OPENAL_INCLUDE_DIR}/AL
            ${LUA_INCLUDE_DIR}
            ${OGG_INCLUDE_DIR}
            ${THEORA_INCLUDE_DIR}
            ${SDL2_INCLUDE_DIR})
    include_directories(src)
    include_directories(res)
    include_directories(misc)
    add_compile_options(-fPIC)
endif ()

add_subdirectory(Externals)
add_subdirectory(src)
add_subdirectory(res)
add_subdirectory(misc)
