add_library(xrAPI SHARED)

target_sources(xrAPI PRIVATE
    stdafx.h
    stdafx.cpp
    xrAPI.cpp
)

target_include_directories(xrAPI
    PRIVATE
    "${XRAY16_SOURCE_DIR}/src"
)

target_compile_definitions(xrAPI
    PRIVATE
    XRAPI_EXPORTS
)
if (NOT ANDROID)
set_target_properties(xrAPI PROPERTIES
    PREFIX ""
)
endif ()

target_precompile_headers(xrAPI
    PRIVATE
    stdafx.h
)
if (NOT ANDROID)
install(TARGETS xrAPI LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
endif ()