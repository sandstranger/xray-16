if (STATIC_BUILD)
    add_library(xrGameSpy STATIC)
else()
    add_library(xrGameSpy SHARED)
endif()


target_sources_grouped(
    TARGET xrGameSpy
    NAME "Kernel"
    FILES
    stdafx.cpp
    stdafx.h
    xrGameSpy.cpp
    xrGameSpy.h
    xrGameSpy_MainDefs.h
)

target_sources_grouped(
    TARGET xrGameSpy
    NAME "Wrapper"
    FILES
    GameSpy_ATLAS.cpp
    GameSpy_ATLAS.h
    GameSpy_Available.cpp
    GameSpy_Available.h
    GameSpy_Browser.cpp
    GameSpy_Browser.h
    GameSpy_BrowsersWrapper.cpp
    GameSpy_BrowsersWrapper.h
    GameSpy_Full.cpp
    GameSpy_Full.h
    GameSpy_GCD_Client.cpp
    GameSpy_GCD_Client.h
    GameSpy_GCD_Server.cpp
    GameSpy_GCD_Server.h
    GameSpy_GP.cpp
    GameSpy_GP.h
    GameSpy_HTTP.cpp
    GameSpy_HTTP.h
    GameSpy_Keys.h
    GameSpy_QR2.cpp
    GameSpy_QR2.h
)

target_include_directories(xrGameSpy
    PRIVATE
    "${XRAY16_SOURCE_DIR}/src"
    "${XRAY16_SOURCE_DIR}/src/xrServerEntities"
    "${XRAY16_SOURCE_DIR}/Externals/GameSpy/src"
    "${XRAY16_SOURCE_DIR}/Externals/ode/include"
    "${XRAY16_SOURCE_DIR}/Externals/OpenAutomate/inc"
)

target_link_libraries(xrGameSpy
    PRIVATE
    xrCore
    xrMiscMath
    GameSpy-oxr
)

target_compile_definitions(xrGameSpy
    PRIVATE
    XRGAMESPY_EXPORTS
    XRAY_DISABLE_GAMESPY_WARNINGS
)

set_target_properties(xrGameSpy PROPERTIES
    PREFIX ""
    POSITION_INDEPENDENT_CODE ON
)

target_precompile_headers(xrGameSpy
    PRIVATE
    stdafx.h
)
if (NOT ANDROID)
if (NOT STATIC_BUILD)
    install(TARGETS xrGameSpy LIBRARY
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
endif ()