#include "RenderDebugInfoSceneRendererCommand.h"
#include "XML\XMLTreeNode.h"
#include "RenderManager.h"
#include "Core.h"
#include "Base.h"
#include "Fonts\FontManager.h"
#include "Utils\Timer.h"
#include "DebugGUIManager.h"
#include "Modifiers\ModifierManager.h"
#include "DebugOptions\DebugOptions.h"
#include "Math\Matrix44.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CRenderDebugInfoSceneRendererCommand::CRenderDebugInfoSceneRendererCommand(CXMLTreeNode &Node)
{
	m_Grid = Node.GetBoolProperty("grid", false);
	m_Axis = Node.GetBoolProperty("axis", false);
}

void CRenderDebugInfoSceneRendererCommand ::Execute(CRenderManager &RM)
{	
	CFontManager *fontManager = CORE->GetFontManager();
	CORE->GetDebugGUIManager()->Render(RM, *fontManager, CORE->GetTimer() );

	CORE->GetModifierManager()->Render( RM, *fontManager );	

	Mat44f mat;
	mat.SetIdentity();
	RM.SetTransform(mat);

	if( m_Grid )
		RM.DrawGrid(100.f, 100.f, 30, colBLACK);

	if( m_Axis )
		RM.DrawAxis(1.0f);

	CORE->GetDebugOptions()->Render( RM, *fontManager );
}
