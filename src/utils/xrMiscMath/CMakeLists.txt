if (ANDROID)
    add_compile_options(-include "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp")
endif ()

add_library(xrMiscMath STATIC)

target_sources(xrMiscMath PRIVATE
    matrix.cpp
    quaternion.cpp
    vector.cpp
    vector3d_ext.cpp
    xrMiscMath.cpp
)

target_include_directories(xrMiscMath
        PRIVATE
        "${XRAY16_SOURCE_DIR}/src"
)

set_target_properties(xrMiscMath PROPERTIES
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

if (NOT ANDROID)
target_precompile_headers(xrMiscMath
    PRIVATE
    pch.hpp
)
endif ()