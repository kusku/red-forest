#include "RabbitTiredState.h"
#include "Utils\BoostRandomHelper.h"

// --- Per pintar l'estat enemic ---
#include "DebugGUIManager.h"
#include "DebugInfo\DebugRender.h"
#include "LogRender\LogRender.h"
#include "Core.h"
// ---------------------------------

#include "Characters\Enemies\Rabbit\Rabbit.h"

#include "RabbitPursuitState.h"
#include "RabbitHitState.h"

#include "RabbitIdleAnimationState.h"
#include "RabbitIdle2AnimationState.h"
#include "RabbitHitAnimationState.h"

#include "Steering Behaviors\SteeringEntity.h"
#include "Steering Behaviors\SteeringBehaviors.h"


#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CRabbitTiredState::CRabbitTiredState( void )
	: CState		("CRabbitTiredState")
	, m_ActionTime	( CActionStateCallback( 0.2f, 0.5f ) )
	, m_pRabbit		( NULL )
{
}

CRabbitTiredState::CRabbitTiredState( const std::string &_Name )
	: CState		(_Name)
	, m_ActionTime	( CActionStateCallback( 0.2f, 0.5f ) )
	, m_pRabbit		( NULL )
{
}


CRabbitTiredState::~CRabbitTiredState(void)
{
	m_pRabbit = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CRabbitTiredState::OnEnter( CCharacter* _Character )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}

	m_ActionTime.StartAction();
	int l_Valor = BoostRandomHelper::GetInt(1, 3);
	// Me gusta darle doble opci�n al idle 2... 
	/*if ( l_Valor == 1 || l_Valor == 2 ) 
	{
		m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdle2AnimationState());
	}
	else
	{*/
		m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());
	//}

	//LOGGER->AddNewLog(ELL_INFORMATION, "Valor : %d", l_Valor);
	#if defined _DEBUG
		if( CORE->IsDebugMode() )
		{
			CORE->GetDebugGUIManager()->GetDebugRender()->SetEnemyStateName("Fatigued");
		}
	#endif
}


void CRabbitTiredState::Execute( CCharacter* _Character, float _ElapsedTime )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}
	
	if ( m_ActionTime.IsActionFinished() ) 
	{
		// restauramos la variable que determina si est� cansado
		m_pRabbit->SetHitsDone(0);
		m_pRabbit->GetLogicFSM()->RevertToPreviousState();
		//m_pRabbit->GetGraphicFSM()->ChangeState(m_pRabbit->GetIdleAnimationState());
	}

	m_ActionTime.Update(_ElapsedTime);

	// Mentre espero miro al player
	m_pRabbit->FaceTo( m_pRabbit->GetPlayer()->GetPosition(), _ElapsedTime);
}


void CRabbitTiredState::OnExit( CCharacter* _Character )
{
}

bool CRabbitTiredState::OnMessage( CCharacter* _Character, const STelegram& _Telegram )
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

std::string CRabbitTiredState::GetRandomAttackName( void )
{
	int l_AttackType = BoostRandomHelper::GetInt(1, 12);
	std::string l_Result;

	if ( l_AttackType == 1 ) 
		l_Result = "attack_1";
	else if  ( l_AttackType == 2 ) 
		l_Result = "attack_1";
	else if  ( l_AttackType == 3 ) 
		l_Result = "attack_1";
	else if  ( l_AttackType == 4 ) 
		l_Result = "attack_2";
	else if  ( l_AttackType == 5 ) 
		l_Result = "attack_2";
	else if  ( l_AttackType == 6 ) 
		l_Result = "attack_2";
	else if  ( l_AttackType == 7 ) 
		l_Result = "defense";
	else if  ( l_AttackType == 8 ) 
		l_Result = "jump";
			
	// Damos la opci�n de tener m�s probabilidades de ir al fustrum que no atacar
	else if  ( l_AttackType == 9 ) 
		l_Result = "go_in_to_fustrum";
	else if  ( l_AttackType == 10 )
		l_Result = "go_in_to_fustrum";
	else if  ( l_AttackType == 11 ) 
		l_Result = "go_in_to_fustrum";
	else if  ( l_AttackType == 12 ) 
		l_Result = "go_in_to_fustrum";

	return l_Result;
}