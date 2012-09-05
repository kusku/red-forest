#include "WolfStillAttackState.h"
#include "GameProcess.h"
#include "Logger\Logger.h"
#include "StatesMachine\MessageDispatcher.h"

#include "Characters\StatesDefs.h"
#include "Characters\Enemies\Wolf\Wolf.h"

#include "WolfPursuitState.h"
#include "WolfPreparedToAttackState.h"
#include "WolfHitState.h"
#include "WolfAttackState.h"

#include "Characters\Enemies\Wolf\AnimationStates\WolfHitAnimationState.h"
#include "Characters\Enemies\Wolf\AnimationStates\WolfStillAttackAnimationState.h"
#include "Characters\Enemies\Wolf\AnimationStates\WolfRunAnimationState.h"
#include "Characters\Enemies\Wolf\AnimationStates\WolfIdleAnimationState.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"
#include "Steering Behaviors\Seek.h"

#include "Callbacks\Animation\AnimationCallback.h"
#include "Callbacks\Animation\AnimationCallbackManager.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CWolfStillAttackState::CWolfStillAttackState( void )
	: CState				("CWolfStillAttackState")
	, m_pWolf				( NULL )
	, m_pAnimationCallback	( NULL )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(STILL_ATTACK_STATE);
}

CWolfStillAttackState::CWolfStillAttackState( const std::string &_Name )
	: CState				(_Name)
	, m_pWolf				( NULL )
	, m_pAnimationCallback	( NULL )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(STILL_ATTACK_STATE);
}


CWolfStillAttackState::~CWolfStillAttackState(void)
{
	m_pWolf = NULL;
	m_pAnimationCallback = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CWolfStillAttackState::OnEnter( CCharacter* _Character )
{
	if (!m_pWolf) 
	{
		m_pWolf = dynamic_cast<CWolf*> (_Character);
	}
	m_pAnimationCallback->Init();
}

void CWolfStillAttackState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pWolf) 
	{
		m_pWolf = dynamic_cast<CWolf*> (_Character);
	}
	
	// Si �s atacable miro si llegu� al m�ximo de lo que permito que me golpeen y bloqueo
	if ( m_pWolf->IsPlayerAtacable() )
	{
		if ( m_pAnimationCallback->IsAnimationStarted() ) 
		{
			// Compruebo si la animaci�n a finalizado
			if ( m_pAnimationCallback->IsAnimationFinished() ) 
			{
				if ( DISPATCH != NULL ) 
				{
					DISPATCH->DispatchStateMessage(SEND_MSG_IMMEDIATELY, m_pWolf->GetID(), m_pWolf->GetPlayer()->GetID(), Msg_Attack, NO_ADDITIONAL_INFO );
				}
				else
				{
					LOGGER->AddNewLog(ELL_ERROR, "CWolfStillAttackState:Execute->El Dispatch es NULL" );
				}
					
				// Incrementamos el n� de ataques hechos --> si llega a un total estar� cansado
				m_pWolf->SetHitsDone(m_pWolf->GetHitsDone() + 1);

				// Volvemos al estado anterior
				m_pWolf->GetLogicFSM()->RevertToPreviousState();
			}
			else 
			{
				//m_pAnimationCallback->AnimationUpdate(_ElapsedTime);
			}
		}
		else
		{
			if ( m_pWolf->IsPlayerInsideImpactDistance() ) 
			{
				// print_logger( 0, "CWolfStillAttackState:Execute-> Impactamos!" )
				m_pWolf->FaceTo( m_pWolf->GetSteeringEntity()->GetPosition(), _ElapsedTime );
				m_pWolf->MoveTo2( Vect3f(0,0,0), _ElapsedTime );
				
				/*self.active_animation_id = _CCharacter:get_animation_id("run")
				_CCharacter:get_animation_model():clear_cycle( self.active_animation_id, 0.3 )
					
				self.active_animation_id = _CCharacter:get_animation_id("attack_1")
				_CCharacter:get_animation_model():execute_action( self.active_animation_id, 0.3 )*/
				
				m_pWolf->GetGraphicFSM()->ChangeState(m_pWolf->GetStillAttackAnimationState());
				m_pAnimationCallback->StartAnimation();
			}
			else 
			{
				// Nos acercamos 
				// _CCharacter.behaviors:pursuit_on()
				m_pWolf->GetBehaviors()->SeekOn();
				m_pWolf->GetBehaviors()->GetSeek()->SetTarget(m_pWolf->GetPlayer()->GetPosition());
				if ( m_pWolf != NULL ) 
				{
					// self.active_animation_name = _CCharacter:get_animation_id("attack_1")
					// _CCharacter:get_animation_model():clear_cycle( self.active_animation_name, 0.3 )
						
					/*self.active_animation_name = _CCharacter:get_animation_id("run")
					_CCharacter:get_animation_model():blend_cycle( self.active_animation_name, 0.3 )*/
					m_pWolf->GetGraphicFSM()->ChangeState(m_pWolf->GetRunAnimationState());
				}
				else 
				{
					LOGGER->AddNewLog(ELL_ERROR, "CWolfStillAttackState:Execute->El Character Wolf es NULL" );
				}
				
				// Rotamos al objetivo y movemos
				m_pWolf->FaceTo( m_pWolf->GetSteeringEntity()->GetPosition(), _ElapsedTime );
				m_pWolf->MoveTo2( m_pWolf->GetSteeringEntity()->GetVelocity(), _ElapsedTime );
			}
		}
	}
	else
	{
		// nos volvemos
		/*m_pWolf->GetLogicFSM()->RevertToPreviousState();
		m_pWolf->GetGraphicFSM()->RevertToPreviousState();*/
		m_pWolf->GetLogicFSM()->ChangeState(m_pWolf->GetAttackState());
		m_pWolf->GetGraphicFSM()->ChangeState(m_pWolf->GetIdleAnimationState());
	} 
}


void CWolfStillAttackState::OnExit( CCharacter* _Character )
{
	// nos volvemos
	/*m_pWolf->GetLogicFSM()->ChangeState(m_pWolf->GetAttackState());
	m_pWolf->GetGraphicFSM()->ChangeState(m_pWolf->GetIdleAnimationState());*/
}

bool CWolfStillAttackState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
{
	if ( _Telegram.Msg == Msg_Attack ) 
	{
		m_pWolf->GetLogicFSM()->ChangeState(m_pWolf->GetHitState());
		m_pWolf->GetGraphicFSM()->ChangeState(m_pWolf->GetHitAnimationState());
		return true;
	}

	return false;
}

