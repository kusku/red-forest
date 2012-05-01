#include "RenderSceneSceneRendererCommand.h"
#include "XML\XMLTreeNode.h"
#include "RenderManager.h"
#include "Core.h"
#include "Base.h"
#include "RenderableObjects\RenderableObjectsManager.h"
#include "RenderableObjects\RenderableObjectsLayersManager.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CRenderSceneSceneRendererCommand::CRenderSceneSceneRendererCommand(CXMLTreeNode &Node)
	: CSceneRendererCommand ( Node )

{
	//<render_scene layer="solid" active="true"/>
	m_Layer = Node.GetPszProperty("layer", "");

	bool active = Node.GetBoolProperty("active", false);
	SetActive(active);

	m_pROLManager = CORE->GetRenderableObjectsLayersManager();
}

CRenderSceneSceneRendererCommand::~CRenderSceneSceneRendererCommand()
{
}

void CRenderSceneSceneRendererCommand::Execute(CRenderManager &RM)
{
	if( GetActive() )
	{
		m_pROLManager->GetResource(m_Layer)->Render(&RM);
	}
}
