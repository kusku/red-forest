#include "DebugRender.h"
#include "Fonts\FontManager.h"
#include "RenderManager.h"
#include "Utils\Timer.h"
#include "ActionToInput.h"
#include "Utils\BaseUtils.h"
#include "Base.h"
#include "Core.h"
#include <string>

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CDebugRender::CDebugRender(void)
	: m_bIsVisible(true)
	, m_SizeRectangle( Vect2i(0,0) )
	, m_uNumOfElements(5)
{
}


CDebugRender::~CDebugRender(void)
{
}

void CDebugRender::Init(CTimer *timer)
{
	AnalizeSizeInfo(timer);
}

void CDebugRender::Render(CRenderManager *_RM, CFontManager *fm, CTimer *timer, CColor color )
{
	uint32 dy = 6;

	if( m_bIsVisible )
	{
		uint32 dx = m_SizeRectangle.x + 20;
		Vect2i screen = _RM->GetScreenSize();

		CColor backgroundColor = colGREEN;
		backgroundColor.SetAlpha(0.7f);
		CColor edgesColor = colBLACK;
		edgesColor.SetAlpha(0.7f);
		
		_RM->DrawRectangle2D(Vect2i(screen.x - dx, dy), m_SizeRectangle.x, m_SizeRectangle.y, backgroundColor, 2, 2, edgesColor);

		std::string gamepad, hideInfo;
		gamepad = CORE->GetActionToInput()->GetGamepadState() ? "true" : "false";
		CORE->GetActionToInput()->GetActionInfo("DebugInfo", hideInfo);

		dy += fm->DrawDefaultText(screen.x - dx, dy, color, "FPS: %f", timer->GetFPS() );
		dy += fm->DrawDefaultText(screen.x - dx, dy, color, "Delta Time (ms): %f",  timer->GetElapsedTime() * 1000.f);
		dy += fm->DrawDefaultText(screen.x - dx, dy, color, "Gamepad: %s", gamepad.c_str());
		
		//Info para ocultar la información
		dy += fm->DrawDefaultText(screen.x - dx, dy, color, "_________________________");
		dy += fm->DrawDefaultText(screen.x - dx, dy, color, "%s to hide", hideInfo.c_str());
	}
	else
	{
		Vect2i l_SizeRectangle;
		Vect2i l_Screen = _RM->GetScreenSize();
		CFontManager* fm = CORE->GetFontManager();

		CColor backgroundColor = colGREEN;
		backgroundColor.SetAlpha(0.7f);
		CColor edgesColor = colBLACK;
		edgesColor.SetAlpha(0.7f);

		std::string l_sInfo, l_Action;
		CORE->GetActionToInput()->GetActionInfo("DebugInfo", l_Action);
		baseUtils::FormatSrting (l_sInfo, "Press %s to view the Debug Info", l_Action.c_str() );
		l_SizeRectangle.x = fm->SizeX(l_sInfo.c_str());
		l_SizeRectangle.y = fm->SizeY(l_sInfo.c_str());

		uint32 dx = l_SizeRectangle.x + 20;
		_RM->DrawRectangle2D(Vect2i(l_Screen.x - dx, dy), l_SizeRectangle.x, l_SizeRectangle.y, backgroundColor, 2, 2, edgesColor);

		fm->DrawDefaultText(l_Screen.x - dx, dy, colWHITE, "Press %s to view the Debug Info", l_Action.c_str());
	}

}
void CDebugRender::AnalizeSizeInfo(CTimer *timer)
{
	std::string l_sInfo;
	CFontManager* fm = CORE->GetFontManager();

	baseUtils::FormatSrting(l_sInfo,"_________________________");
	m_SizeRectangle.x = fm->SizeX(l_sInfo.c_str());
	m_SizeRectangle.y = fm->SizeY(l_sInfo.c_str()) * m_uNumOfElements;
}