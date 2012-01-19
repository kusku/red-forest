#pragma once

#ifndef _EFFECTDEFS_H
#define _EFFECTDEFS_H

#define EFFECT_MAX_NUM_PARAMETERS 30
#define EFFECT_MAX_NUM_TECHNIQUES 30

enum MATERIALS_ID
{
	NO_ID = 0,
	AMBIENT_LIGHT,
	OMNI_LIGHT,
	SPOT_LIGHT,
	END_MATERIALS
};

enum EFFECT_FLAGS
{
	EFFECT_DONOTSAVESTATE = D3DXFX_DONOTSAVESTATE,
	EFFECT_DONOTSAVESAMPLERSTATE = D3DXFX_DONOTSAVESAMPLERSTATE,
	EFFECT_DONOTSAVESHADERSTATE = D3DXFX_DONOTSAVESHADERSTATE
};

#endif