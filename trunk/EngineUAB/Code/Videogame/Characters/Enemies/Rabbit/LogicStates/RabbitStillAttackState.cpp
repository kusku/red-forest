#include "RabbitStillAttackState.h"
#include "GameProcess.h"
#include "SoundManager.h"

// --- Per pintar l'estat enemic ---
#include "DebugGUIManager.h"
#include "DebugInfo\DebugRender.h"
#include "LogRender\LogRender.h"
#include "Core.h"
// ---------------------------------

#include "StatesMachine\MessageDispatcher.h"
#include "StatesMachine\Telegram.h"

#include "Characters\StatesDefs.h"
#include "Characters\Enemies\Rabbit\Rabbit.h"

#include "RabbitPursuitState.h"
#include "RabbitPreparedToAttackState.h"
#include "RabbitHitState.h"
#include "RabbitAttackState.h"
#include "RabbitIdleState.h"

#include "Characters\Enemies\Rabbit\AnimationStates\RabbitHitAnimationState.h"
#include "Characters\Enemies\Rabbit\AnimationStates\RabbitStillAttackAnimationState.h"
#include "Characters\Enemies\Rabbit\AnimationStates\RabbitRunAnimationState.h"
#include "Characters\Enemies\Rabbit\AnimationStates\RabbitIdleAnimationState.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"
#include "Steering Behaviors\Seek.h"

#include "Callbacks\Animation\AnimationCallback.h"
#include "Callbacks\Animation\AnimationCallbackManager.h"

#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CRabbitStillAttackState::CRabbitStillAttackState( void )
	: CState				("CRabbitStillAttackState")
	, m_pRabbit				( NULL )
	, m_pAnimationCallback	( NULL )
	, m_pActionStateCallback( 0, 1 )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(RABBIT_STILL_ATTACK_STATE);
}

CRabbitStillAttackState::CRabbitStillAttackState( const std::string &_Name )
	: CState				(_Name)
	, m_pRabbit				( NULL )
	, m_pAnimationCallback	( NULL )
	, m_pActionStateCallback( 0, 1 )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(RABBIT_STILL_ATTACK_STATE);
}


CRabbitStillAttackState::~CRabbitStillAttackState(void)
{
	m_pRabbit = NULL;
	m_pAnimationCallback = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CRabbitStillAttackState::OnEnter( CCharacter* _Character )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}

#if defined _DEBUG
	if( CORE->IsDebugMode() )
	{
		std::string l_State = RABBIT_STILL_ATTACK_STATE;
		CORE->GetDebugGUIManager()->GetDebugRender()->AddEnemyStateName(m_pRabbit->GetName().c_str(), l_State );
	}
#endif

	m_SoundPlayed1	= false;
	m_SoundPlayed2	= false;
	m_pAnimationCallback->Init();

	m_pRabbit->GetBehaviors()->CollisionAvoidanceOn();
	m_pRabbit->GetBehaviors()->ObstacleWallAvoidanceOn();

	//CORE->GetSoundManager()->PlayEvent("Play_EFX_DeerExclaim"); 
	m_pActionStateCallback.InitAction(0, m_pAnimationCallback->GetAnimatedModel()->GetCurrentAnimationDuration(RABBIT_STILL_ATTACK_STATE) );
}

void CRabbitStillAttackState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}
	
	// Si �s atacable miro si llegu� al m�ximo de lo que permito que me golpeen y bloqueo
	if ( m_pRabbit->IsPlayerAtacable() )
	{
		if ( m_pAnimationCallback->IsAnimationStarted() ) 
		{
			if ( !m_pRabbit->GetPlayerHasBeenReached()  && m_pRabbit->IsPlayerReached() )
			{
				m_pRabbit->SetPlayerHasBeenReached(true);
				//m_pRabbit->GetPreparedToAttack()->set
			}

			// Compruebo si la animaci�n a finalizado
			if ( m_pAnimationCallback->IsAnimationFinished() && m_pRabbit->GetPlayerHasBeenReached() )
			{
				if ( DISPATCH != NULL ) 
				{
					DISPATCH->DispatchStateMessage(SEND_MSG_IMMEDIATELY, m_pRabbit->GetID(), m_pRabbit->GetPlayer()->GetID(), Msg_Attack, NO_ADDITIONAL_INFO );
					LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitStillAttackState::Execute->Envio mensaje de tocado");
				}
				else
				{
					LOGGER->AddNewLog(ELL_ERROR, "CRabbitStillAttackState:Execute->El Dispatch es NULL" );
				}
					
				// Incrementamos el n� de ataques hechos --> si llega a un total estar� cansado
				m_pRabbit->SetHitsDone(m_pRabbit->GetHitsDone() + 1);

				// Volvemos a idle
				m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetIdleState());
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());
				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog( ELL_INFORMATION, "CRabbitStillAttackState:Execute->Dispatch" );
					}
				#endif
				
				m_pRabbit->GetBehaviors()->SeekOff();
				m_pRabbit->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0) );
				m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime );
				m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );
			}
			// Si acaba la animacion pero no estamos en una distancia de poder impactar solo hacemos que se canse
			else if ( m_pAnimationCallback->IsAnimationFinished() && !m_pRabbit->GetPlayerHasBeenReached() )
			{
				// Incrementamos el n� de ataques hechos --> si llega a un total estar� cansado
				m_pRabbit->SetHitsDone(m_pRabbit->GetHitsDone() + 1);

				// Volvemos al estado anterior
				m_pRabbit->GetLogicFSM()->RevertToPreviousState();
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());

				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitStillAttackState::Execute->Golpeo erratico");
					}
				#endif

				m_pRabbit->GetBehaviors()->SeekOff();
				m_pRabbit->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0) );
				m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime );
				m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );

				return;
			}
			// NO!! Esto hace que luego corra mientras ataca y queda de pena. Mantengo el comentario para recordarlo.
			// En caso que no estemos en una distancia de ataque nos salimos. Este se comenta pq se prevee que falle y corte la animacion
			/*else if ( !m_pRabbit->IsPlayerInsideImpactDistance() )
			{
				m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetAttackState());
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());

				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						CORE->GetDebugGUIManager()->GetDebugRender()->AddEnemyStateName(m_pWolf->GetName().c_str(), "Allunyat!");
					}
				#endif

				return;
			}*/
			// En otro caso actualizamos el tiempo de animacion
			else
			{
				m_pRabbit->GetBehaviors()->SeekOff();
				m_pRabbit->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0) );
				m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime );
				m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );

				//float t = m_pAnimationCallback->GetAnimatedModel()->GetCurrentAnimationDuration(DEER_STILL_ATTACK_STATE);

				// Sonido de bofetada 
				if ( m_pRabbit->GetPlayerHasBeenReached() && !m_SoundPlayed1 &&  m_pActionStateCallback.IsActionInTime( 0.7f ) )
				{
					m_SoundPlayed1 = true;
					CORE->GetSoundManager()->PlayEvent("Play_EFX_Punch2"); 
				}
				// Sonido de bofetada fallida
				else if ( !m_pRabbit->GetPlayerHasBeenReached() && !m_SoundPlayed1 &&  m_pActionStateCallback.IsActionInTime( 0.7f ) )
				{
					CORE->GetSoundManager()->PlayEvent("Play_EFX_Slap1"); 
				}

				// Sonido de bofetada 
				if ( m_pRabbit->GetPlayerHasBeenReached() && !m_SoundPlayed2 &&  m_pActionStateCallback.IsActionInTime( 0.2f ) )
				{
					m_SoundPlayed2 = true;
					CORE->GetSoundManager()->PlayEvent("Play_EFX_Punch3"); 
				}
				// Sonido de bofetada fallida
				else if ( !m_pRabbit->GetPlayerHasBeenReached() && !m_SoundPlayed2 &&  m_pActionStateCallback.IsActionInTime( 0.2f ) )
				{
					CORE->GetSoundManager()->PlayEvent("Play_EFX_Slap1"); 
				}

				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitStillAttackState::Execute->Animacion en curso...");
					}
				#endif

				// hay que cancelar la animaci�n
				/*if ( !m_pRabbit->IsPlayerInsideImpactDistance() )
				{
					m_pRabbit->GetLogicFSM()->RevertToPreviousState();
					m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());
				}*/

				m_pActionStateCallback.Update(_ElapsedTime);
			}
		}
		else
		{
			if ( m_pRabbit->IsPlayerInsideImpactDistance() ) 
			{
				m_pRabbit->GetBehaviors()->SeekOff();
				m_pRabbit->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0) );
				m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime );
				m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );
				
				/*self.active_animation_id = _CCharacter:get_animation_id("run")
				_CCharacter:get_animation_model():clear_cycle( self.active_animation_id, 0.3 )
					
				self.active_animation_id = _CCharacter:get_animation_id("attack_1")
				_CCharacter:get_animation_model():execute_action( self.active_animation_id, 0.3 )*/
				
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetStillAttackAnimationState());
				m_pAnimationCallback->StartAnimation();
				m_pActionStateCallback.StartAction();

				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitStillAttackState::Execute->Inicio Animacion");
					}
				#endif
			}
			else 
			{
				// Nos acercamos 
				// _CCharacter.behaviors:pursuit_on()
				m_pRabbit->GetBehaviors()->SeekOn();
				m_pRabbit->GetBehaviors()->GetSeek()->SetTarget(m_pRabbit->GetPlayer()->GetPosition());
				m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetRunAnimationState());
				
				// Rotamos al objetivo y movemos
				m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime );
				m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );
				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitStillAttackState::Execute->Nos acercamos primero");
					}
				#endif
			}
		}
	}
	else
	{
		// nos volvemos
		/*m_pRabbit->GetLogicFSM()->RevertToPreviousState();
		m_pRabbit->GetGraphicFSM()->RevertToPreviousState();*/
		m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetAttackState());
		m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());
	} 
}


void CRabbitStillAttackState::OnExit( CCharacter* _Character )
{
	m_pRabbit->GetBehaviors()->CollisionAvoidanceOff();
	m_pRabbit->GetBehaviors()->ObstacleWallAvoidanceOff();

	CORE->GetSoundManager()->PlayEvent("Stop_EFX_DeerExclaim"); 
}

bool CRabbitStillAttackState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
{
	if ( _Telegram.Msg == Msg_Attack ) 
	{
		if (!m_pRabbit) 
		{
			m_pRabbit = dynamic_cast<CRabbit*> (_Character);
		}

		m_pRabbit->RestLife(10); 
		m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetHitState());
		return true;
	}

	return false;
}

