#ifndef SLOAD_H
#define SLOAD_H

#include "common.h"

#ifdef	MSAA_ALPHATEST_DX10_1
#if MSAA_SAMPLES == 2
const float2 MSAAOffsets[2] = { float2(4,4), float2(-4,-4) };
#endif
#if MSAA_SAMPLES == 4
const float2 MSAAOffsets[4] = { float2(-2,-6), float2(6,-2), float2(-6,2), float2(2,6) };
#endif
#if MSAA_SAMPLES == 8
const float2 MSAAOffsets[8] = { float2(1,-3), float2(-1,3), float2(5,1), float2(-3,-5), 
								               float2(-5,5), float2(-7,-1), float2(3,7), float2(7,-7) };
#endif
#endif	//	MSAA_ALPHATEST_DX10_1

//////////////////////////////////////////////////////////////////////////////////////////
// Bumped surface loader                //
//////////////////////////////////////////////////////////////////////////////////////////
struct	surface_bumped
{
	float4	base;
	float3	normal;
	float	gloss;
	float	height;

};

float4 tbase( float2 tc )
{
   return	tex2D( s_base, tc );
}


void UpdateTC( inout p_bumped I)
{
	;
}

surface_bumped sload_i( p_bumped I)
{
	surface_bumped	S;
   
	UpdateTC(I);	//	All kinds of parallax are applied here.

	float4 	Nu	= tex2D( s_bump, I.tcdh );		// IN:	normal.gloss
	float4 	NuE	= tex2D( s_bumpX, I.tcdh);		// IN:	normal_error.height

	S.base		= tbase(I.tcdh);				//	IN:  rgb.a
	S.normal	= Nu.wzy + (NuE.xyz - 1.0);	//	(Nu.wzyx - 0.5) + (E-0.5)
	S.gloss		= Nu.x*Nu.x;					//	S.gloss = Nu.x*Nu.x;
	S.height	= NuE.z;
	//S.height	= 0;

#ifdef        USE_TDETAIL
#ifdef        USE_TDETAIL_BUMP
	float4 NDetail		= tex2D( s_detailBump, I.tcdbump);
	float4 NDetailX		= tex2D( s_detailBumpX, I.tcdbump);
	S.gloss				= S.gloss * NDetail.x * 2.0;
	//S.normal			+= NDetail.wzy-0.5;
	S.normal			+= NDetail.wzy + NDetailX.xyz - 1.0; //	(Nu.wzyx - 0.5) + (E-0.5)

	float4 detail		= tex2D( s_detail, I.tcdbump);
	S.base.rgb			= S.base.rgb * detail.rgb * 2;

//	S.base.rgb			= float3(1,0,0);
#else        //	USE_TDETAIL_BUMP
	float4 detail		= tex2D( s_detail, I.tcdbump);
	S.base.rgb			= S.base.rgb * detail.rgb * 2.0;
	S.gloss				= S.gloss * detail.w * 2.0;
#endif        //	USE_TDETAIL_BUMP
#endif

	return S;
}

surface_bumped sload_i( p_bumped I, float2 pixeloffset )
{
	surface_bumped	S;
   
   // apply offset
#ifdef	MSAA_ALPHATEST_DX10_1
   I.tcdh.xy += pixeloffset.x * ddx(I.tcdh.xy) + pixeloffset.y * ddy(I.tcdh.xy);
#endif

	UpdateTC(I);	//	All kinds of parallax are applied here.

	float4 	Nu	= tex2D( s_bump, I.tcdh );		// IN:	normal.gloss
	float4 	NuE	= tex2D( s_bumpX, I.tcdh);	// IN:	normal_error.height

	S.base		= tbase(I.tcdh);				//	IN:  rgb.a
	S.normal	= Nu.wzy + (NuE.xyz - 1.0);	//	(Nu.wzyx - 0.5) + (E-0.5)
	S.gloss		= Nu.x*Nu.x;					//	S.gloss = Nu.x*Nu.x;
	S.height	= NuE.z;
	//S.height	= 0;

#ifdef        USE_TDETAIL
#ifdef        USE_TDETAIL_BUMP
#ifdef MSAA_ALPHATEST_DX10_1
#if ( (!defined(ALLOW_STEEPPARALLAX) ) && defined(USE_STEEPPARALLAX) )
   I.tcdbump.xy += pixeloffset.x * ddx(I.tcdbump.xy) + pixeloffset.y * ddy(I.tcdbump.xy);
#endif
#endif

	float4 NDetail		= tex2D( s_detailBump, I.tcdbump);
	float4 NDetailX		= tex2D( s_detailBumpX, I.tcdbump);
	S.gloss				= S.gloss * NDetail.x * 2.0;
	//S.normal			+= NDetail.wzy-0.5;
	S.normal			+= NDetail.wzy + NDetailX.xyz - 1.0; //	(Nu.wzyx - 0.5) + (E-0.5)

	float4 detail		= tex2D( s_detail, I.tcdbump);
	S.base.rgb			= S.base.rgb * detail.rgb * 2;

//	S.base.rgb			= float3(1,0,0);
#else        //	USE_TDETAIL_BUMP
#ifdef MSAA_ALPHATEST_DX10_1
	I.tcdbump.xy += pixeloffset.x * ddx(I.tcdbump.xy) + pixeloffset.y * ddy(I.tcdbump.xy);
#endif
	float4 detail		= tex2D( s_detail, I.tcdbump);
	S.base.rgb			= S.base.rgb * detail.rgb * 2.0;
	S.gloss				= S.gloss * detail.w * 2.0;
#endif        //	USE_TDETAIL_BUMP
#endif

	return S;
}

surface_bumped sload ( p_bumped I)
{
      surface_bumped      S   = sload_i	(I);
		S.normal.z			*=	0.5;		//. make bump twice as contrast (fake, remove me if possible)

#ifdef	GBUFFER_OPTIMIZATION
	   S.height = 0;
#endif	//	GBUFFER_OPTIMIZATION
      return              S;
}

surface_bumped sload ( p_bumped I, float2 pixeloffset )
{
      surface_bumped      S   = sload_i	(I, pixeloffset );
		S.normal.z			*=	0.5;		//. make bump twice as contrast (fake, remove me if possible)
#ifdef	GBUFFER_OPTIMIZATION
	   S.height = 0;
#endif	//	GBUFFER_OPTIMIZATION
      return              S;
}

#endif
