
out gl_PerVertex
{
	vec4 gl_Position;
};

struct v2p
{
	float3 	lightToPos	; // TEXCOORD0;		// light center to plane vector
	float3 	vPos		; // TEXCOORD1;		// position in camera space
	float 	fDensity	; // TEXCOORD2;		// plane density alon Z axis
//	float2	tNoise 		; // TEXCOORD3;		// projective noise
	float3	clip0		; // SV_ClipDistance0;
	float3	clip1		; // SV_ClipDistance1;
	float4 	hpos		; // SV_Position;
};

layout(location = POSITION)		in float3	v_volumetric_P;

layout(location = TEXCOORD0)	out float3 	v2p_lightToPos	; // TEXCOORD0;		// light center to plane vector
layout(location = TEXCOORD1)	out float3 	v2p_vPos		; // TEXCOORD1;		// position in camera space
layout(location = TEXCOORD2)	out float 	v2p_fDensity	; // TEXCOORD2;		// plane density alon Z axis
//layout(location = TEXCOORD3)	out float2	v2p_tNoise 		; // TEXCOORD3;		// projective noise
layout(location = TEXCOORD3) 	out float3 v2p_clip0		;  
layout(location = TEXCOORD4) 	out float3 v2p_clip1		;

v2p _main ( float3 P );

void main()
{
	v2p O	= _main ( v_volumetric_P );
	v2p_lightToPos	= O.lightToPos;
	v2p_vPos		= O.vPos;
	v2p_fDensity	= O.fDensity;
//	v2p_tNoise		= O.tNoise;
    v2p_clip0       = O.clip0;
    v2p_clip1       = O.clip1;
	gl_Position		= O.hpos;
}
