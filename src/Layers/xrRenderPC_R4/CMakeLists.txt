add_library(xrRenderPC_R4)

target_sources(xrRenderPC_R4 PRIVATE
    packages.config
    r2_test_hw.cpp
    r4_rendertarget.h
    r4_rendertarget_accum_direct.cpp
    r4_rendertarget_build_textures.cpp
    r4_rendertarget_phase_combine.cpp
    r4_rendertarget_phase_hdao.cpp
    r4_rendertarget_u_set_rt.cpp
    r4_shaders.cpp
    R_Backend_LOD.cpp
    R_Backend_LOD.h
    xrRender_R4.cpp
)

target_include_directories(xrRenderPC_R4
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/sdk/include"
)

target_link_libraries(xrRenderPC_R4
    PRIVATE
    xrCore
    xrCDB
    xrEngine
    xrParticles
    xrScriptEngine
    xrAPI
    xrMiscMath
)

target_compile_definitions(xrRenderPC_R4
    PRIVATE
    XRRENDER_R4_EXPORTS
    USE_DX11
)

if (NOT ANDROID)
set_target_properties(xrRenderPC_R4 PROPERTIES
    PREFIX ""
)
target_precompile_headers(xrRenderPC_R4
    PRIVATE
    stdafx.h
)
else ()
    target_compile_options(xrRenderPC_R4 PRIVATE
            -include ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
    )
endif ()

if (BUILD_SHARED_LIBS AND NOT ANDROID)
    install(TARGETS xrRenderPC_R4 LIBRARY
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
