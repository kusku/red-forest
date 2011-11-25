#pragma once

#ifndef _ENGINE_H
#define _ENGINE_H

#include <Windows.h>
#include "Process.h"
#include "Math\Color.h"
#include "Utils\Timer.h"
#include "CoreDefs.h"
#include "LogRender\LogRender.h"
class CLogger;
class CCore;

class CEngine
{
public:
	CEngine();
	virtual ~CEngine();

	void Init( HWND hWnd );
	void Update();
	void Render();
	void RenderScene( CRenderManager* renderManager );
	uint32 RenderFPS	();

	void LoadConfigXML	( const std::string &configFile );
	void Reload			();

	void SetProcess( CProcess *process );

	Vect2i		GetResolution	() const { return m_Config.resolution; }
	Vect2i		GetPosition		() const { return m_Config.position; }

protected:
	CCore		*m_pCore;
	CProcess	*m_pProcess;
	CLogger		*m_pLogger;
	CTimer		m_Timer;
	CLogRender	m_LogRender;

	SConfig		m_Config;

	const CColor	string2Color	( const std::string &color );
	void			UpdateDebugInputs	();	

};

#endif