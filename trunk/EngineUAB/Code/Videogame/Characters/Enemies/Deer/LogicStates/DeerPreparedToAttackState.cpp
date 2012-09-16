#include "DeerPreparedToAttackState.h"
#include "Utils\BoostRandomHelper.h"

// --- Per pintar l'estat enemic ---
#include "DebugGUIManager.h"
#include "DebugInfo\DebugRender.h"
#include "LogRender\LogRender.h"
#include "Core.h"
// ---------------------------------

#include "Characters\Enemies\Deer\Deer.h"

#include "DeerPursuitState.h"
#include "DeerAttackState.h"
#include "DeerHitState.h"

#include "Characters\Enemies\Deer\AnimationStates\DeerRunAnimationState.h"
#include "Characters\Enemies\Deer\AnimationStates\DeerHitAnimationState.h"
#include "Characters\Enemies\Deer\AnimationStates\DeerWalkAnimationState.h"
#include "Characters\Enemies\Deer\AnimationStates\DeerIdleAnimationState.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"
#include "Steering Behaviors\Seek.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CDeerPreparedToAttackState::CDeerPreparedToAttackState( void )
	: CState								("CDeerPreparedToAttackState")
	, m_pDeer								( NULL )
	, m_IsPositionAfterHitPlayerAssigned	( false )
{
}

CDeerPreparedToAttackState::CDeerPreparedToAttackState( const std::string &_Name )
	: CState								(_Name)
	, m_pDeer								( NULL )
	, m_IsPositionAfterHitPlayerAssigned	( false )
{
}


CDeerPreparedToAttackState::~CDeerPreparedToAttackState(void)
{
	m_pDeer = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CDeerPreparedToAttackState::OnEnter( CCharacter* _Character )
{
	if (!m_pDeer) 
	{
		m_pDeer = dynamic_cast<CDeer*> (_Character);
	}

	#if defined _DEBUG
		if( CORE->IsDebugMode() )
		{
			std::string l_State = "Prepared to attack";
			CORE->GetDebugGUIManager()->GetDebugRender()->AddEnemyStateName(m_pDeer->GetName().c_str(), l_State );
		}
	#endif
}

void CDeerPreparedToAttackState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pDeer) 
	{
		m_pDeer = dynamic_cast<CDeer*> (_Character);
	}

	// 0) Caso en que alcanz� al player y por tanto vamos a un punto de inicio de ataque. As� dejo que el player se reponga
	if ( m_pDeer->GetPlayerHasBeenReached() )
	{
		// Si no ser donde tengo que ir...
		if ( !m_IsPositionAfterHitPlayerAssigned )
		{
			m_PositionReachedAfterHitPlayer = m_pDeer->GetPointInsideCameraFrustum();
			m_IsPositionAfterHitPlayerAssigned	= true;
		}

		// Mira si alcanzamos la posici�n. Reseteamos indicando que este enemigo ya ha realizado las tareas postimpacto 
		float l_Distance = m_pDeer->GetPosition().Distance(m_PositionReachedAfterHitPlayer);
		if ( l_Distance <= 3.01f )
		{
			m_IsPositionAfterHitPlayerAssigned = false;		// Reiniciamos el flag para la pr�xima vez
			m_pDeer->SetPlayerHasBeenReached(false);		// Reiniciamos el flag de player alcanzado
			m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetIdleAnimationState());
			return;
		}
		else
		{
			m_pDeer->GetBehaviors()->GetSeek()->SetTarget(m_PositionReachedAfterHitPlayer);
			m_pDeer->GetBehaviors()->SeekOn();
			m_pDeer->FaceTo( m_pDeer->GetPlayer()->GetPosition(), _ElapsedTime);
			m_pDeer->MoveTo2(m_pDeer->GetSteeringEntity()->GetVelocity(), _ElapsedTime);
			LOGGER->AddNewLog(ELL_INFORMATION, "CDeerPreparedToAttackState::Execute -> %s pegu� al player y ahora vuelve a una posici�n inicial de ataque", m_pDeer->GetName().c_str());
			return;
		}
	}

	// 1) Caso en que ataco al player. Si est� focalizado y suficientemente cerca de atacar lo hace independientemente del angulo de visi�n del player
	if ( m_pDeer->IsPlayerAtacable() ) 
	{
		m_pDeer->SetHitsDone(2);		// Esto permite hacer una pausa al entrar en el estado de ataque antes de atacar por obligar estar fatigado y permitir ver al player qu� va a hacer el enemigo
		m_pDeer->GetLogicFSM()->ChangeState( m_pDeer->GetAttackState() );

		// Reseteamos la velocidad del enemigo
		m_pDeer->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0));
		m_pDeer->FaceTo( m_pDeer->GetPlayer()->GetPosition(), _ElapsedTime );
		m_pDeer->MoveTo2( m_pDeer->GetSteeringEntity()->GetVelocity(), _ElapsedTime );

		#if defined _DEBUG
			if( CORE->IsDebugMode() )
			{
				LOGGER->AddNewLog(ELL_INFORMATION,"CDeerPreparedToAttackState::Execute->Change to Attack State");
			}
		#endif
	}
	
	// 2) Si el player NO es atacable pero casi nos aproximamos. Buscamos el hueco que no col�lisione con nada.
	else if ( m_pDeer->IsEnemyPreparedToAttack() ) 
	{
		// Si el player puede atacar porque �s uno de los m�s cercanos pero aun no �s el elegido (el que realmente ataca ya que solo ataca 1)
		if ( m_pDeer->GetAvalaibleToAttack() ) 
		{
			// Este enemigo podria atacar pero no es el seleccionado. Ahora miro si est� dentro del angulo de vision y si no lo est� lo metemos para que el player pueda verlo
			float l_Angle = 60.f;			//math.pi/15 == 12 graus de fustrum
			if ( !m_pDeer->IsEnemyIntoCameraFrustum( l_Angle, _ElapsedTime ) )
			{
				m_pDeer->GoIntoCameraFrustum(l_Angle, _ElapsedTime);
				return;
			}

			//Vect3f l_NewAttackPosition = GetPositionToAttack();
			m_pDeer->GetBehaviors()->GetSeek()->SetTarget(m_pDeer->GetPlayer()->GetPosition());
			m_pDeer->GetBehaviors()->SeekOn();
			m_pDeer->FaceTo( m_pDeer->GetPlayer()->GetPosition(), _ElapsedTime);
			m_pDeer->MoveTo2(m_pDeer->GetSteeringEntity()->GetVelocity(), _ElapsedTime);

			//m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetWalkAnimationState());		// dudo de si uno u otro. Faltan pasos laterales...
			m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetRunAnimationState());
			#if defined _DEBUG
				if( CORE->IsDebugMode() )
				{
					LOGGER->AddNewLog(ELL_INFORMATION,"CDeerPreparedToAttackState::Execute->Prepared-Walk");
				}
			#endif
		}
		// Si el enemigo no est� listo para atacar ya que est� m�s lejos que los que deben atacar. Reseteamos velocidad y encaramos al player. 
		// Exite un total de enemigos a atacar. El resto se quedan en idle
		else
		{
			//m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetWalkAnimationState());
			m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetIdleAnimationState());
			m_pDeer->FaceTo( m_pDeer->GetPlayer()->GetPosition(), _ElapsedTime);
			m_pDeer->MoveTo2(m_pDeer->GetSteeringEntity()->GetVelocity(), _ElapsedTime);
			#if defined _DEBUG
				if( CORE->IsDebugMode() )
				{
					LOGGER->AddNewLog(ELL_INFORMATION,"CDeerPreparedToAttackState::Execute->Not Ready-Too far");
				}
			#endif
		}
	}
	else
	{
		m_pDeer->GetLogicFSM()->ChangeState( m_pDeer->GetPursuitState());
		m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetRunAnimationState());
	}
}


void CDeerPreparedToAttackState::OnExit( CCharacter* _Character )
{
}

bool CDeerPreparedToAttackState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
{
	if ( _Telegram.Msg == Msg_Attack ) 
	{
		if (!m_pDeer) 
		{
			m_pDeer = dynamic_cast<CDeer*> (_Character);
		}

		m_pDeer->RestLife(50); 
		m_pDeer->GetLogicFSM()->ChangeState(m_pDeer->GetHitState());
		m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetHitAnimationState());
		return true;
	} 

	return false;
}