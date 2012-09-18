#include "RabbitRunAttackState.h"
#include "Utils\BoostRandomHelper.h"
#include "GameProcess.h"
#include "Logger\Logger.h"
#include "Math\Vector3.h"
#include "SoundManager.h"
#include "Core.h"

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

#include "RabbitAttackState.h"
#include "RabbitPursuitState.h"
#include "RabbitPreparedToAttackState.h"
#include "RabbitHitState.h"
#include "RabbitIdleState.h"

#include "Characters\Enemies\Rabbit\AnimationStates\RabbitHitAnimationState.h"
#include "Characters\Enemies\Rabbit\AnimationStates\RabbitIdleAnimationState.h"
#include "Characters\Enemies\Rabbit\AnimationStates\RabbitRunAttackAnimationState.h"
#include "Characters\Enemies\Rabbit\AnimationStates\RabbitRunAnimationState.h"

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
CRabbitRunAttackState::CRabbitRunAttackState( CCharacter* _pCharacter )
	: CState				(_pCharacter, "CRabbitRunAttackState")
	, m_pRabbit				( NULL )
	, m_pAnimationCallback	( NULL )
	, m_ActionStateCallback	( 0, 1 )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(_pCharacter->GetName(),RABBIT_RUN_ATTACK_STATE);
}

CRabbitRunAttackState::CRabbitRunAttackState( CCharacter* _pCharacter, const std::string &_Name )
	: CState				(_pCharacter, _Name)
	, m_pRabbit				( NULL )
	, m_pAnimationCallback	( NULL )
	, m_ActionStateCallback ( 0, 1 )
{
	CGameProcess * l_Process = dynamic_cast<CGameProcess*> (CORE->GetProcess());
	m_pAnimationCallback = l_Process->GetAnimationCallbackManager()->GetCallback(_pCharacter->GetName(),RABBIT_RUN_ATTACK_STATE);
}


CRabbitRunAttackState::~CRabbitRunAttackState(void)
{
	m_pRabbit = NULL;
	m_pAnimationCallback = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CRabbitRunAttackState::OnEnter( CCharacter* _pCharacter )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_pCharacter);
	}

	PlayRandomSound();
	
	m_pRabbit->SetPlayerHasBeenReached( false );
	m_playerPushed = false;

	// Metemos m�s velocidad al ataque i menos massa para acelerar m�s 
	m_OldMaxSpeed	= m_pRabbit->GetSteeringEntity()->GetMaxSpeed();
	m_OldMass		= m_pRabbit->GetSteeringEntity()->GetMass();
	m_pRabbit->GetSteeringEntity()->SetMaxSpeed(0.4f);

	// Almacenamos la distancia actual para saber si luego nos hemos pasado
	m_PlayerInitialPosition		= m_pRabbit->GetPlayer()->GetSteeringEntity()->GetPosition();
	Vect3f l_Position			= m_pRabbit->GetPosition();
	m_InitialDistance			= m_pRabbit->GetDistanceToPlayer();
	Vect3f l_RelativePosition	= m_PlayerInitialPosition - m_pRabbit->GetPosition();
	Vect3f l_RelativePositionN	= l_RelativePosition.GetNormalized();
	m_FinalAttackPosition = l_Position + (l_RelativePositionN * ( m_InitialDistance + 0.f) );

	// Activo el seek a saco a una posici� en el momento de inicio de ataque
	m_pRabbit->GetBehaviors()->SeekOff();
	m_pRabbit->GetBehaviors()->GetSeek()->SetTarget(m_FinalAttackPosition);
	
	m_AnimationDuration = m_pRabbit->GetAnimatedModel()->GetCurrentAnimationDuration(DEER_RUN_ATTACK_STATE);

	m_pAnimationCallback->Init();
	m_ActionStateCallback.InitAction(0,m_AnimationDuration);
	m_ActionStateCallback.StartAction();

	// Almacenamos la distancia actual para saber si luego nos hemos pasado
	//m_CurrentDistance = m_pRabbit->GetDistanceToPlayer();

	#if defined _DEBUG
		if( CORE->IsDebugMode() )
		{
			std::string l_State = "Enter Run Attack";
			CORE->GetDebugGUIManager()->GetDebugRender()->AddEnemyStateName(m_pRabbit->GetName().c_str(), l_State );
		}
	#endif

}

void CRabbitRunAttackState::Execute( CCharacter* _pCharacter, float _ElapsedTime )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_pCharacter);
	}

	m_ActionStateCallback.Update(_ElapsedTime);

	if ( m_pAnimationCallback->IsAnimationStarted() ) 
	{
		// Si el player no estaba ya alcanzado pero ahora se alcanza
		if ( !m_pRabbit->GetPlayerHasBeenReached() && m_pRabbit->IsPlayerReached() )
		{
			m_pRabbit->SetPlayerHasBeenReached(true);
			CORE->GetSoundManager()->PlayEvent("Play_EFX_DeerRunAttackCharged"); 
		}

		// Compruebo si la animaci�n ha finalizado
		if ( m_pAnimationCallback->IsAnimationFinished() ) 
		{
			// Si encontr� el player por delante finalizo golpeando
			if ( m_pRabbit->GetPlayerHasBeenReached() )
			{
				if ( DISPATCH != NULL ) 
				{
					DISPATCH->DispatchStateMessage(SEND_MSG_IMMEDIATELY, m_pRabbit->GetID(), m_pRabbit->GetPlayer()->GetID(), Msg_Attack, NO_ADDITIONAL_INFO );
					LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitRunAttackState::Execute->Envio mensaje de tocado");
					#if defined _DEBUG
						if( CORE->IsDebugMode() )
						{
							LOGGER->AddNewLog(ELL_INFORMATION, "CRabbitRunAttackState:Execute->Dispatch" );
						}
					#endif
				}
				else
				{
					LOGGER->AddNewLog(ELL_ERROR, "CRabbitRunAttackState:Execute->El Dispatch es NULL" );
				}
			}
			else
			{
				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog(ELL_INFORMATION,"CRabbitStillAttackState::Execute->Golpeo erratico");
					}
				#endif
			}

			// Incrementamos el n� de ataques hechos --> si llega a un total estar� cansado
			m_pRabbit->SetHitsDone(m_pRabbit->GetHitsDone() + 1);

			// Volvemos al estado anterior
			m_pRabbit->GetBehaviors()->SeekOff();
			m_pRabbit->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0));
			m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime );
			m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );
				
			// Volvemos a idle
			m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetIdleState());
			m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());

			return;
		}
		// La animaci�n aun no ha acabado pero est� iniciada
		else 
		{
			// Si encuentro el player por delante me lo llevo
			if ( m_pRabbit->GetPlayerHasBeenReached() )
			{
				if ( !m_playerPushed )
				{
					m_AditionalInfo.Direccion	= m_pRabbit->GetSteeringEntity()->GetVelocity()/2;
					m_AditionalInfo.ElapsedTime = _ElapsedTime;
				}
					
				if ( DISPATCH != NULL ) 
				{
					DISPATCH->DispatchStateMessage(SEND_MSG_IMMEDIATELY, m_pRabbit->GetID(), m_pRabbit->GetPlayer()->GetID(), Msg_Push, &m_AditionalInfo );
					m_playerPushed = true;
				}
				else
				{
					LOGGER->AddNewLog(ELL_ERROR, "CDeerRunAttackState:Execute->El Dispatch es NULL" );
				}
			}
			else
			{
				#if defined _DEBUG
					if( CORE->IsDebugMode() )
					{
						LOGGER->AddNewLog(ELL_INFORMATION, "CDeerRunAttackState:Execute->Animation Not finished yet" );
					}
				#endif
			}
		}
			
		if ( m_ActionStateCallback.IsActionInTime(0.4f) )
		{
			m_pRabbit->GetBehaviors()->SeekOn();
			m_pRabbit->GetBehaviors()->GetSeek()->SetTarget(m_PlayerInitialPosition);
		}

		// No Rotamos al objetivo y pero si movemos. Esto dar� sensaci�n de golpear all� donde estava el target cuando inicie el ataque
		//_CCharacter:face_to( self.target_position, _elapsed_time )
		m_pRabbit->FaceTo( m_FinalAttackPosition, _ElapsedTime );
		m_pRabbit->MoveTo2( m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );
	}
	// Si l'animaci�n no se ha iniciado
	// Primer estado que se ejecutar�. Si est� lejos nos acercamos con gran velocidad Corremos r�pido hac�a el player. 
	// Para ello hay que saber el tiempo de la animaci�n y cuando hay que activarla. 
	else
	{
		m_pRabbit->GetBehaviors()->SeekOff();
		m_pRabbit->GetBehaviors()->GetSeek()->SetTarget(m_PlayerInitialPosition);
		#if defined _DEBUG
			if( CORE->IsDebugMode() )
			{
				LOGGER->AddNewLog(ELL_INFORMATION, "CDeerRunAttackState:Execute->Correm al objectiu" );
			}
		#endif
			
		// Ahora azitamos y empezamos la animaci�n de ataque
		m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetRunAttackAnimationState());
		m_pAnimationCallback->StartAnimation();

		m_pRabbit->FaceTo( m_FinalAttackPosition, _ElapsedTime );
		m_pRabbit->MoveTo2(  m_pRabbit->GetSteeringEntity()->GetVelocity(), _ElapsedTime );

		if ( m_pRabbit->GetPlayerHasBeenReached() == false && m_pRabbit->IsPlayerReached() )
		{
			m_pRabbit->SetPlayerHasBeenReached(true);
			CORE->GetSoundManager()->PlayEvent("Play_EFX_DeerRunAttackCharged"); 
		}
	}
}


void CRabbitRunAttackState::OnExit( CCharacter* _pCharacter )
{
	// Limpiamos animaciones
	/*self.active_animation_name = _CCharacter:get_animation_id("attack_2")
	_CCharacter:get_animation_model():clear_cycle( self.active_animation_name, 0.3 )
		
	self.active_animation_name = _CCharacter:get_animation_id("run")
	_CCharacter:get_animation_model():clear_cycle( self.active_animation_name, 0.3 )*/

	// Quitamos el behaviur
	m_pRabbit->GetBehaviors()->SeekOff();
	m_pRabbit->GetBehaviors()->CollisionAvoidanceOff();
	m_pRabbit->GetBehaviors()->ObstacleWallAvoidanceOff();
				
	// Restauramos la velocidad original
	m_pRabbit->GetSteeringEntity()->SetMaxSpeed(m_OldMaxSpeed);
	m_pRabbit->GetSteeringEntity()->SetMass(m_OldMass);

	CORE->GetSoundManager()->PlayEvent("Stop_EFX_RabbitsRunAttacks"); 

}

bool CRabbitRunAttackState::OnMessage( CCharacter* _pCharacter, const STelegram& _Telegram )
{
	if ( _Telegram.Msg == Msg_Attack ) 
	{
		if (!m_pRabbit) 
		{
			m_pRabbit = dynamic_cast<CRabbit*> (_pCharacter);
		}

		m_pRabbit->RestLife(1000); 
		m_pRabbit->GetLogicFSM()->ChangeState(m_pRabbit->GetHitState());
		return true;
	}

	return false;
}

// Devuelve el tiempo, la duraci�n
void CRabbitRunAttackState::PlayRandomSound( void )
{
	int l_Num = BoostRandomHelper::GetInt(1,2);
	if ( l_Num == 1 )
	{
		CORE->GetSoundManager()->PlayEvent("Play_EFX_RabbitsRunAttack1");
		m_SoundDuration = 0.858f;
	}
	else if ( l_Num == 2)
	{
		CORE->GetSoundManager()->PlayEvent("Play_EFX_RabbitsRunAttack2");
		m_SoundDuration = 0.817f;
	}
}
