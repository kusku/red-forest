#include "BoxTrigger.h"
#include "Math/Vector3.h"
#include "Math\Matrix44.h"
#include "RenderManager.h"
#include "Triggers\Trigger.h"
#include "PhysicUserData.h"
#include "PhysicActor.h"
#include "PhysicsDefs.h"
#include "PhysicsManager.h"
#include "Core.h"
#include "Base.h"
#include "XML\XMLTreeNode.h"
#include "Math\Color.h"

#if defined (_DEBUG)
#include "Memory/MemLeaks.h"
#endif

//----------------------------------------------
CBoxTrigger::CBoxTrigger()
{
}

//----------------------------------------------
CBoxTrigger::~CBoxTrigger()
{
}

//----------------------------------------------
void CBoxTrigger::ReadData( CXMLTreeNode &_Node )
{
	m_Name			= _Node.GetPszProperty("name", "");
	m_Position		= _Node.GetVect3fProperty("position", v3fZERO);
	m_Size			= _Node.GetVect3fProperty("size", v3fZERO);
	m_RenderColor	= CColor( _Node.GetVect4fProperty("color", Vect4f(1.f, 1.f, 1.f, 1.f)) );
}

//----------------------------------------------
void CBoxTrigger::Init()
{
	CPhysicUserData * l_UserData = new CPhysicUserData ( GetName(), UD_IS_TRIGGER );
	l_UserData->SetColor ( m_RenderColor );
	l_UserData->SetPaint ( m_bRenderDebugTrigger );

	m_pTriggerActor = new CPhysicActor( l_UserData );
	m_pTriggerActor->CreateBoxTrigger( m_Position, m_Size, ECG_TRIGGERS );

	CORE->GetPhysicsManager()->AddPhysicActor(m_pTriggerActor);
}

//----------------------------------------------
void CBoxTrigger::Update( float _fElapsedTime )
{

}

//----------------------------------------------
void CBoxTrigger::Render( CRenderManager *_RM )
{
	Mat44f l_Translation;

	if( m_bRenderDebugTrigger )
	{
		l_Translation.SetIdentity();
		l_Translation.Translate(m_Position);
		_RM->SetTransform(l_Translation);

		_RM->DrawCube(m_Size, m_RenderColor);
	}
}

//----------------------------------------------
void CBoxTrigger::OnEnter()
{

}

//----------------------------------------------
void CBoxTrigger::OnExecute()
{

}

//----------------------------------------------
void CBoxTrigger::OnExit()
{

}