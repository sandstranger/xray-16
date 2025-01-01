if (ANDROID)
    add_library(xr_3da SHARED)
else ()
    add_executable(xr_3da)
endif ()

target_sources(xr_3da PRIVATE
    entry_point.cpp
    stdafx.h
)

set_property(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTY
    VS_STARTUP_PROJECT xr_3da
)

target_include_directories(xr_3da
    PRIVATE
    "${XRAY16_SOURCE_DIR}/src"
)

target_link_libraries(xr_3da
    PRIVATE
    xrCore
    xrAPI
    xrEngine
    xrGame
)

if (NOT ANDROID)
set_target_properties(xr_3da PROPERTIES
    PREFIX ""
)
endif ()

install(TARGETS xr_3da RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
