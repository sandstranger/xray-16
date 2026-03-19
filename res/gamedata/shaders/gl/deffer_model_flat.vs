#include "common.h"
#include "skin.h"
#include "iostructs\v_model_flat.h"

v2p_flat _main( v_model I )
{
	// world-space  N
	float3 	N_w	= mul( m_W, I.N );

	// Eye-space pos/normal
	v2p_flat 		O;
	float3	Pe	= mul( m_WV, I.P );
	O.hpos 		= mul( m_WVP, I.P );
	O.N 		= mul( float3x3(m_WV), float3(I.N) );
#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
	O.tcdh 		= float4	(I.tc.xyyy			);
#else
	O.tcdh 		= float2	(I.tc.xyyy			);
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
	O.tcdh.w	= L_material.y;							// (,,,dir-occlusion)
#endif

#ifdef USE_TDETAIL
	O.tcdbump	= O.tcdh.xy*dt_params.xy;					// dt tc
#endif

	return	O;
}
