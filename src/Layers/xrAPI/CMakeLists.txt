if (ANDROID)
    add_compile_options(-include ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)
endif ()

add_library(xrAPI)

target_sources(xrAPI PRIVATE
    xrAPI.cpp
)

if (NOT ANDROID)
target_include_directories(xrAPI
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)
else ()
    target_include_directories(xrAPI
            PRIVATE
            "${XRAY16_SOURCE_DIR}/src"
    )
endif ()

target_compile_definitions(xrAPI
    PRIVATE
    XRAPI_EXPORTS
)
if (NOT ANDROID)
set_target_properties(xrAPI PROPERTIES
    PREFIX ""
)
target_precompile_headers(xrAPI
        PRIVATE
        stdafx.h
)
if (BUILD_SHARED_LIBS)
    install(TARGETS xrAPI LIBRARY
            DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
endif ()


