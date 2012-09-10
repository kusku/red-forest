#include "RabbitIdleState.h"
#include "Utils\BoostRandomHelper.h"

// --- Per pintar l'estat enemic ---
#include "DebugGUIManager.h"
#include "DebugInfo\DebugRender.h"
#include "LogRender\LogRender.h"
#include "Core.h"
// ---------------------------------

#include "Characters\StatesDefs.h"
#include "Characters\Enemies\Rabbit\Rabbit.h"

#include "RabbitPursuitState.h"
#include "RabbitHitState.h"

#include "RabbitIdleAnimationState.h"
#include "RabbitIdle2AnimationState.h"
#include "RabbitRunAnimationState.h"
#include "RabbitHitAnimationState.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CRabbitIdleState::CRabbitIdleState( void )
	: CState		("CRabbitIdleState")
	, m_ActionTime  ( CActionStateCallback( 1.f, 2.f ) )
	, m_pRabbit		( NULL )
{
}

CRabbitIdleState::CRabbitIdleState( const std::string &_Name )
	: CState		(_Name)
	, m_ActionTime	( CActionStateCallback( 1.f, 2.f ) )
	, m_pRabbit		( NULL )
{
}


CRabbitIdleState::~CRabbitIdleState(void)
{
	m_pRabbit = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CRabbitIdleState::OnEnter( CCharacter* _Character )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}
	
	m_ActionTime.StartAction();
	#if defined _DEBUG
		if( CORE->IsDebugMode() )
		{
			CORE->GetDebugGUIManager()->GetDebugRender()->SetEnemyStateName("Idle state");
		}
	#endif
}

void CRabbitIdleState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}

	if ( m_pRabbit->IsPlayerDetected() ) 
	{
		m_pRabbit->GetLogicFSM()->ChangeState( m_pRabbit->GetPursuitState());
		m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetRunAnimationState());
	}
	else
	{
		if ( m_ActionTime.IsActionFinished() )
		{
			// pillamos la animaci�n idle que deseemos
			if ( BoostRandomHelper::GetInt(1, 4) == 1 ) 
			{
				//CState<CCharacter> * l_State = dynamic_cast<CState<CCharacter>*> (m_pRabbit->GetIdleAnimationState());
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());
				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						CORE->GetDebugGUIManager()->GetDebugRender()->SetEnemyStateName(RABBIT_IDLE_STATE);
					}
				#endif
			}
			else
			{
				//CState<CCharacter> * l_State = dynamic_cast<CState<CCharacter>*> (m_pRabbit->GetIdle2AnimationState());
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdle2AnimationState());
				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						CORE->GetDebugGUIManager()->GetDebugRender()->SetEnemyStateName(RABBIT_IDLE2_STATE);
					}
				#endif
			}

			// pillamos un tiempo de ejecuci�n aleatorio
			m_ActionTime.StartAction();
		}
		else
		{
			m_ActionTime.Update(_ElapsedTime);
		}
	}

	// Reseteamos la velocidad del enemigo
	m_pRabbit->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0));
	m_pRabbit->MoveTo2(_Character->GetSteeringEntity()->GetVelocity(), _ElapsedTime);
}


void CRabbitIdleState::OnExit( CCharacter* _Character )
{
}

bool CRabbitIdleState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
{
	if ( _Telegram.Msg == Msg_Attack ) 
	{
		if (!m_pRabbit) 
		{
			m_pRabbit = dynamic_cast<CRabbit*> (_Character);
		}

		m_pRabbit->RestLife(1000); 
		m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetHitState());
		return true;
	}

	return false;
}