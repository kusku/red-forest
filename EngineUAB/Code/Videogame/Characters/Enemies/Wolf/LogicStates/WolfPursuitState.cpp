#include "WolfPursuitState.h"
#include "WolfIdleState.h"
#include "WolfPreparedToAttackState.h"

#include "Characters\Enemies\Wolf\AnimationStates\WolfIdleAnimationState.h"
#include "Characters\Enemies\Wolf\AnimationStates\WolfHitAnimationState.h"

#include "Characters\Enemies\Wolf\Wolf.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"
#include "Steering Behaviors\Pursuit.h"
#include "Steering Behaviors\Seek.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CWolfPursuitState::CWolfPursuitState( void )
	: CState	("CWolfPursuitState")
	, m_pWolf	( NULL )
{
}

CWolfPursuitState::CWolfPursuitState( const std::string &_Name )
	: CState		(_Name)
	, m_pWolf		( NULL )
{
}

CWolfPursuitState::~CWolfPursuitState( void )
{
	m_pWolf = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------

void CWolfPursuitState::OnEnter( CCharacter* _Character )
{
	if (!m_pWolf) 
	{
		m_pWolf = dynamic_cast<CWolf*> (_Character);
	}

	m_pWolf->GetBehaviors()->GetSeek()->SetTarget(m_pWolf->GetPlayer()->GetPosition());
	m_pWolf->GetBehaviors()->SeekOn();
		
	m_pWolf->GetBehaviors()->GetPursuit()->SetTarget(m_pWolf->GetPlayer()->GetPosition());
	m_pWolf->GetBehaviors()->GetPursuit()->UpdateEvaderEntity( m_pWolf->GetPlayer()->GetSteeringEntity() );
	m_pWolf->GetBehaviors()->PursuitOn();
		
	// _Character->GetBehaviors()->separation_on()
	// _Character->GetBehaviors()->collision_avoidance_on()
	// _Character->GetBehaviors()->obstacle_wall_avoidance_on()
}

void CWolfPursuitState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pWolf) 
	{
		m_pWolf = dynamic_cast<CWolf*> (_Character);
	}
	
	m_pWolf->GetBehaviors()->SeekOff();
	m_pWolf->GetBehaviors()->PursuitOff();

	if ( m_pWolf->IsPlayerDetected() ) 
	{
		if ( m_pWolf->IsEnemyPreparedToAttack() ) 
		{
			// Reseteamos la velocidad del enemigo y cambiamos a un estado que prepara para el ataque
			m_pWolf->GetBehaviors()->PursuitOff();
			m_pWolf->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0));
			m_pWolf->GetLogicFSM()->ChangeState(m_pWolf->GetPreparedToAttack());
		}
		else
		{
			// Seguimos persiguiendo...
			/*m_pWolf->GetBehaviors()->GetPursuit()->SetTarget(m_pWolf->GetPlayer()->GetPosition());
			m_pWolf->GetBehaviors()->GetPursuit()->UpdateEvaderEntity( m_pWolf->GetPlayer()->GetSteeringEntity() );
			m_pWolf->GetBehaviors()->PursuitOn();*/

			m_pWolf->GetBehaviors()->GetSeek()->SetTarget(m_pWolf->GetPlayer()->GetPosition());
			m_pWolf->GetBehaviors()->SeekOn();

			m_pWolf->FaceTo(m_pWolf->GetSteeringEntity()->GetPosition(), _ElapsedTime);
			m_pWolf->MoveTo2(m_pWolf->GetSteeringEntity()->GetVelocity(), _ElapsedTime);
		}
	}
	else
	{
		// Volvemos al reposo
		m_pWolf->GetLogicFSM()->ChangeState( m_pWolf->GetIdleState());
		m_pWolf->GetGraphicFSM()->ChangeState(m_pWolf->GetIdleAnimationState());
	}
}


void CWolfPursuitState::OnExit( CCharacter* _Character )
{
	if (!m_pWolf) 
	{
		m_pWolf = dynamic_cast<CWolf*> (_Character);
	}

	m_pWolf->GetBehaviors()->SeekOff();
	m_pWolf->GetBehaviors()->PursuitOff();
	m_pWolf->GetBehaviors()->CollisionAvoidanceOff();
	m_pWolf->GetBehaviors()->ObstacleWallAvoidanceOff();

	//_Character->GetBehaviors()->SeparationOff();
	//_Character->GetBehaviors()->CohesionOff();
	//_Character->GetBehaviors()->AlignmentOff();
	
}

bool CWolfPursuitState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
{
	if ( _Telegram.Msg == Msg_Attack ) 
	{
		//CState<CCharacter> * l_State = dynamic_cast<CState<CCharacter>*> (m_pWolf->GetHitAnimationState());
		_Character->GetLogicFSM()->ChangeState(m_pWolf->GetHitAnimationState());
		return true;
	}
	return false;
}