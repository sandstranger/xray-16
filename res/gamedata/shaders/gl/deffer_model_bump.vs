#include	"common.h"
#include	"skin.h"
#include	"iostructs\v_model_bump.h"

#if defined(USE_PARALLAX) || defined(USE_STEEPPARALLAX)
	uniform float3x4	    m_invW;
#endif 	//	defined(USE_PARALLAX) || defined(USE_STEEPPARALLAX)

v2p_bumped _main( v_model I )
{
	float4	w_pos	= I.P;

	// Eye-space pos/normal
	v2p_bumped 	O;
	O.hpos 		= mul( m_WVP, w_pos	);
	float2 	tc 	= I.tc;
	float3	Pe	= mul( m_WV, w_pos );
#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	O.tcdh 		= float4( tc.xyyy );
#else
	O.tcdh 		= float2( tc.xyyy );
#endif

	//  Hemi cube lighting
    vec3 n = I.N;

    float n2 = dot(n, n);
    n = (n2 > 1e-8) ? n * inversesqrt(n2) : vec3(0.0, 0.0, 1.0);

    vec3 Nw = (mat3(m_W) * n);

    float nw2 = dot(Nw, Nw);
    Nw = (nw2 > 1e-8) ? Nw * inversesqrt(nw2) : vec3(0.0, 0.0, 1.0);

    vec3 hc_pos = vec3(hemi_cube_pos_faces);
    vec3 hc_neg = vec3(hemi_cube_neg_faces);

    vec3 hc_mixed = vec3(
            (Nw.x < 0.0) ? hc_neg.x : hc_pos.x,
            (Nw.y < 0.0) ? hc_neg.y : hc_pos.y,
            (Nw.z < 0.0) ? hc_neg.z : hc_pos.z
    );

    float hemi_val = dot(hc_mixed, abs(Nw));

    hemi_val = clamp(hemi_val, 0.0, 1.0);

    if (isnan(hemi_val))
        hemi_val = 1.0;

    O.position = vec4(Pe, hemi_val);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	O.tcdh.w	= L_material.y;					// (,,,dir-occlusion)
#endif

	// Calculate the 3x3 transform from tangent space to eye-space
	// TangentToEyeSpace = object2eye * tangent2object
	//		     = object2eye * transpose(object2tangent) (since the inverse of a rotation is its transpose)
	float3 	N 	= I.N;		// just scale (assume normal in the -0.5, 0.5)
	float3 	T 	= I.T;		// 
	float3 	B 	= I.B;		// 
	float3x3 xform	= mul	(float3x3(m_WV), float3x3(2.0*T,2.0*B,2.0*N));
	// The pixel shader operates on the bump-map in [0..1] range
	// Remap this range in the matrix, anyway we are pixel-shader limited :)
	// ...... [ 2  0  0  0]
	// ...... [ 0  2  0  0]
	// ...... [ 0  0  2  0]
	// ...... [-1 -1 -1  1]
	// issue: strange, but it's slower :(
	// issue: interpolators? dp4? VS limited? black magic? 

	// Feed this transform to pixel shader
	O.M1 			= xform	[0]; 
	O.M2 			= xform	[1]; 
	O.M3 			= xform	[2]; 

#ifdef 	USE_TDETAIL
	O.tcdbump		= O.tcdh.xy * dt_params.xy;		// dt tc
#endif

	return	O;
}
