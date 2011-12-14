#pragma once

#ifndef _CORE_H
#define _CORE_H

class CInputManager;
class CFontManager;
class CLanguageManager;
class CActionToInput;
class CRenderManager;
class CTextureManager;
class CStaticMeshManager;

#include <Windows.h>
#include "Utils\Singleton.h"
#include "CoreDefs.h"

class CCore : public CSingleton<CCore>
{
public:
	CCore();
	virtual ~CCore();

	bool Init		( HWND hWnd, const SConfig &config );
	void Done		();
	bool IsOk		() const { return m_bIsOk; }

	void Update		( float ElapsedTime );
	void Render		();

	void	Reload				();
	void	ReloadTTFs			();	
	void	ReloadLanguages		();	
	void	ReloadInputs		();

	CRenderManager*		GetRenderManager() const	{ return m_pRenderManager; }
	CFontManager*		GetFontManager() const		{ return m_pFontManager; }
	CLanguageManager*	GetLanguageManager() const	{ return m_pLanguageManager; }
	CInputManager*		GetInputManager() const		{ return m_pInputManager; }
	CActionToInput*		GetActionToInput() const	{ return m_pActionToInput; }
	CTextureManager*	GetTextureManager() const	{ return m_pTextureManager; }
	CStaticMeshManager*	GetStaticMeshManager() const{ return m_pStaticMeshManager; }

private:
	void Release();

	bool	m_bIsOk;

	CRenderManager		*m_pRenderManager;
	CFontManager		*m_pFontManager;
	CLanguageManager	*m_pLanguageManager;
	CInputManager		*m_pInputManager;
	CActionToInput		*m_pActionToInput;
	CTextureManager		*m_pTextureManager;
	CStaticMeshManager*	m_pStaticMeshManager;

};

#endif