#include "RabbitStillAttackAnimationState.h"
#include "Characters\StatesDefs.h"
#include "Characters\Enemies\Rabbit\Rabbit.h"
#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CRabbitStillAttackAnimationState::CRabbitStillAttackAnimationState( void )
	: CState	("CRabbitStillAttackAnimationState")
	, m_pRabbit	( NULL )
{
}

CRabbitStillAttackAnimationState::CRabbitStillAttackAnimationState( const std::string &_Name )
	: CState	(_Name)
	, m_pRabbit	( NULL )
{
}

CRabbitStillAttackAnimationState::~CRabbitStillAttackAnimationState( void )
{
	if ( m_pRabbit )
	{	
		int l_Num = m_pRabbit->GetAnimationID(STILL_ATTACK_STATE);
		m_pRabbit->GetAnimatedModel()->ClearCycle( l_Num, 0.3f );
	}
	
	m_pRabbit = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CRabbitStillAttackAnimationState::Execute( CCharacter*, float _ElapsedTime )
{
}

void CRabbitStillAttackAnimationState::OnEnter( CCharacter* _Character )
{
	if (!m_pRabbit) 
	{
		m_pRabbit = dynamic_cast<CRabbit*> (_Character);
	}

	int l_Num = _Character->GetAnimationID(STILL_ATTACK_STATE);
	m_pRabbit->GetAnimatedModel()->BlendCycle( l_Num, 0.3f );
}

void CRabbitStillAttackAnimationState::OnExit( CCharacter* _Character )
{
	if (m_pRabbit) 
	{
		int l_Num = m_pRabbit->GetAnimationID(STILL_ATTACK_STATE);
		m_pRabbit->GetAnimatedModel()->ClearCycle( l_Num, 0.3f );
	}
}

bool CRabbitStillAttackAnimationState::OnMessage( CCharacter*, const STelegram& _Telegram )
{
	return false;
}
