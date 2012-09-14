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

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CDeerPreparedToAttackState::CDeerPreparedToAttackState( void )
	: CState	("CDeerPreparedToAttackState")
	, m_pDeer	( NULL )
{
}

CDeerPreparedToAttackState::CDeerPreparedToAttackState( const std::string &_Name )
	: CState		(_Name)
	, m_pDeer		( NULL )
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
			CORE->GetDebugGUIManager()->GetDebugRender()->SetEnemyStateName("Prepared to attack");
		}
	#endif
}

void CDeerPreparedToAttackState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pDeer) 
	{
		m_pDeer = dynamic_cast<CDeer*> (_Character);
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
			// Este enemigo podria atacar pero no es el seleccionado. Ahora miro si est� dentro del angulo de vision y si no lo est� lo metemos
			float l_Angle = 60.f;			//math.pi/15		// 12 graus de fustrum
			if ( !m_pDeer->HaveToGoIntoFrustum(l_Angle, _ElapsedTime) )
			{
				return;
			}
			// Lo tengo en frente
			else
			{

			}

			//m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetWalkAnimationState());		// dudo de si uno u otro. Faltan pasos laterales...
			m_pDeer->GetGraphicFSM()->ChangeState(m_pDeer->GetRunAnimationState());
			#if defined _DEBUG
				if( CORE->IsDebugMode() )
				{
					LOGGER->AddNewLog(ELL_INFORMATION,"CDeerPreparedToAttackState::Execute->Prepared-Walk");
				}
			#endif
		}
		// Si el enemigo no est� listo para atacar ya que est� m�s lejos que los que deben atacar. Reseteamos velocidad y encaramos al player
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
		return true;
	} 

	return false;
}