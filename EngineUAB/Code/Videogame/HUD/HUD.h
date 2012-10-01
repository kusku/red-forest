#pragma once

#ifndef _HUD_H_
#define _HUD_H_

#include "Utils\TemplatedVectorMapManager.h"
#include "Math\Vector2.h"
#include <string>

class CRenderManager;
class CTexture;

//------------------------------------------------
struct STexture
{
	CTexture	*pTexture;
	bool		bActive;
	Vect2i		position;
	Vect2i		size;
};

//------------------------------------------------
class CHud : public CTemplatedVectorMapManager<STexture>
{
public:
	CHud();
	~CHud();

	bool	Load			( const std::string &_Filename );
	bool	Reload			();
	void	CleanUp			();

	void	Init			( int _iPlayerLife );
	void	Update			( float _fElapsedTime, int _iPlayerLife );
	void	Render			( CRenderManager &RM );

	bool	IsTextureActive	( const std::string &_name );
	void	ActiveTexture	( const std::string &_name, bool _bActive );

private:
	bool	LoadFile		();
	void	CalculatePositions();

private:
	std::string		m_Filename;

	float			m_fThresholdDyingEffect;

	//Player info
	int				m_iPlayerLife;
	int				m_iPlayerPreviousLife;

	//Texture properties
	Vect2i			m_MaskPosition;
	Vect2i			m_MaskSize;

	Vect2i			m_BarPosition;
	Vect2i			m_BarSize;
	Vect2i			m_BarRealSize;
	float			m_PerCentSize;

	Vect2i			m_BackgroundPosition;
	Vect2i			m_BackgroundSize;

	bool			m_bBackgroundActive;
	bool			m_bBarActive;
	bool			m_bMaskActive;

	CTexture*		m_pBar;
	CTexture*		m_pMask;
	CTexture*		m_pBackground;
};
//------------------------------------------------

#endif //_HUD_H_