add_library(xrCDB SHARED)

target_sources_grouped(
    TARGET xrCDB
    NAME "engine"
    FILES
    ISpatial.cpp
    ISpatial.h
    ISpatial_q_box.cpp
    ISpatial_q_frustum.cpp
    ISpatial_q_ray.cpp
    ISpatial_verify.cpp
    xr_area.cpp
    xr_area.h
    xr_area_query.cpp
    xr_area_raypick.cpp
    xr_collide_defs.h
    xrXRC.cpp
    xrXRC.h
)

target_sources_grouped(
    TARGET xrCDB
    NAME "Kernel"
    FILES
    Frustum.cpp
    Frustum.h
    Intersect.hpp
    stdafx.h
    StdAfx.cpp
    xrCDB.cpp
    xrCDB.h
    xrCDB_box.cpp
    xrCDB_Collector.cpp
    xrCDB_frustum.cpp
    xrCDB_ray.cpp
)

target_include_directories(xrCDB
    PRIVATE
    "${XRAY16_SOURCE_DIR}/src"
    "${XRAY16_SOURCE_DIR}/Externals"
    "${XRAY16_SOURCE_DIR}/Externals/imgui"
)

target_link_libraries(xrCDB
    PRIVATE
    xrCore
    xrMiscMath
    xrOPCODE
    xrAPI
)

target_compile_definitions(xrCDB
    PRIVATE
    XRCDB_EXPORTS
    $<$<BOOL:${STATIC_BUILD}>:OPCODE_STATIC>
)

if (NOT ANDROID)
set_target_properties(xrCDB PROPERTIES
    PREFIX ""
)
endif ()
target_precompile_headers(xrCDB
    PRIVATE
    stdafx.h
)

if (NOT ANDROID)
install(TARGETS xrCDB LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
endif ()