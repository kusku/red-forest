#include "DeerHitState.h"
#include "Utils\BoostRandomHelper.h"
#include "GameProcess.h"
#include "Callbacks\Animation\AnimationCallback.h"
#include "Callbacks\Animation\AnimationCallbackManager.h"
#include "Callbacks\State\ActionStateCallback.h"

// --- Per pintar l'estat enemic ---
#include "DebugGUIManager.h"
#include "DebugInfo\DebugRender.h"
#include "LogRender\LogRender.h"
#include "Core.h"
// ---------------------------------

#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"

#include "Characters\Enemies\Deer\Deer.h"
#include "Characters\StatesDefs.h"

#include "DeerPursuitState.h"
#include "DeerPreparedToAttackState.h"
#include "DeerTiredState.h"
#include "DeerIdleState.h"

#include "Characters\Enemies\Deer\AnimationStates\DeerHitAnimationState.h"
#include "Characters\Enemies\Deer\AnimationStates\DeeridleAnimationState.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"


#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CDeerHitState::CDeerHitState( void )
	: CState				("CDeerHitState")
	, m_pDeer				( NULL )
	, m_pActionState		( NULL )
	, m_pAnimationCallback	( NULL )
	, m_IsCommingFromTired	( false )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(DEER_HIT_STATE);

	m_pActionState = new CActionStateCallback(0,1);
}

CDeerHitState::CDeerHitState( const std::string &_Name )
	: CState				(_Name)
	, m_pDeer				( NULL )
	, m_pActionState		( NULL )
	, m_pAnimationCallback	( NULL )
	, m_IsCommingFromTired	( false )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(DEER_HIT_STATE);

	m_pActionState = new CActionStateCallback(0,1);
}


CDeerHitState::~CDeerHitState(void)
{
	m_pDeer = NULL;
	m_pAnimationCallback = NULL;
	CHECKED_DELETE ( m_pActionState );
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CDeerHitState::OnEnter( CCharacter* _Character )
{
	if (!m_pDeer) 
	{
		m_pDeer = dynamic_cast<CDeer*> (_Character);
	}

	// Si volvemos de haber recibido y despu�s de estar cansados nos salimos.
	if ( m_IsCommingFromTired ) 
	{
		m_pDeer->GetTiredState()->SetTiredTime(m_RecoverMinTiredTime, m_RecoverMaxTiredTime);	// Recuperamos el tiempo que teneniamos por defecto asignado al estado TIRED
		m_pDeer->GetLogicFSM()->ChangeState(m_pDeer->GetIdleState());
		m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetIdleAnimationState());
		m_IsCommingFromTired = false;
	}
	// Si entramos por primera vez ejecutaremos el hit normal
	else 
	{
		m_pAnimationCallback->Init();
		m_pAnimationCallback->StartAnimation();
	
		m_pActionState->InitAction(0.f, m_pDeer->GetAnimatedModel()->GetCurrentAnimationDuration(DEER_HIT_STATE));
		m_pActionState->StartAction();
	}

	
	#if defined _DEBUG
		if( CORE->IsDebugMode() )
		{
			CORE->GetDebugGUIManager()->GetDebugRender()->SetEnemyStateName("Hit state");
		}
	#endif
}

void CDeerHitState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pDeer) 
	{
		m_pDeer = dynamic_cast<CDeer*> (_Character);
	}

	/*if ( m_pAnimationCallback->IsAnimationStarted() ) 
	{
		if ( m_pAnimationCallback->IsAnimationFinished() ) 
		{
			m_pDeer->GetLogicFSM()->RevertToPreviousState();
			m_pDeer->GetGraphicFSM()->RevertToPreviousState();
		}
	}*/
	/*else
	{
		m_pAnimationCallback->StartAnimation();
	}*/

	if ( m_pActionState->IsActionFinished() )
	{
		/*m_pDeer->GetLogicFSM()->ChangeState(m_pDeer->GetIdleState());
		m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetIdleAnimationState());*/

		if ( m_pDeer->IsAlive() ) 
		{
			// Obligo a descansar entre unos segundosw
			float l_MaxTimeInTired = BoostRandomHelper::GetFloat(3, 5);
			m_RecoverMinTiredTime = m_pDeer->GetTiredState()->GetMinTiredTime();
			m_RecoverMaxTiredTime = m_pDeer->GetTiredState()->GetMaxTiredTime();
			m_pDeer->GetTiredState()->SetTiredTime(0.f, l_MaxTimeInTired);
			m_pDeer->GetLogicFSM()->ChangeState(m_pDeer->GetTiredState());
			m_IsCommingFromTired = true;
		}
	}
	else
	{
		m_pActionState->Update(_ElapsedTime);
	}
}

void CDeerHitState::OnExit( CCharacter* _Character )
{
	//if (!m_pDeer) 
	//{
	//	m_pDeer = dynamic_cast<CDeer*> (_Character);
	//}
}

bool CDeerHitState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
{
	return false;
}


