#include "DeerDefenseAnimationState.h"
#include "Characters\StatesDefs.h"
#include "Characters\Enemies\Deer\Deer.h"
#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CDeerDefenseAnimationState::CDeerDefenseAnimationState( CCharacter* _pCharacter )
	: CState	(_pCharacter,"CDeerDefenseAnimationState")
	, m_pDeer	( NULL )
{
}

CDeerDefenseAnimationState::CDeerDefenseAnimationState( CCharacter* _pCharacter, const std::string &_Name )
	: CState	(_pCharacter,_Name)
	, m_pDeer	( NULL )
{}


CDeerDefenseAnimationState::~CDeerDefenseAnimationState(void)
{
	if (m_pDeer)
	{
		int l_Num = m_pDeer->GetAnimationID(DEER_DEFENSE_STATE);
		m_pDeer->GetAnimatedModel()->ClearCycle( l_Num, 0.1f );
	}
	m_pDeer = NULL;
}


// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------
void CDeerDefenseAnimationState::Execute( CCharacter*, float _ElapsedTime )
{
}

void CDeerDefenseAnimationState::OnEnter( CCharacter* _pCharacter )
{
	if ( !m_pDeer ) 
	{
		// Almacenamos el enemigo
		m_pDeer = dynamic_cast<CDeer*> (_pCharacter);
	}

	int l_Num = m_pDeer->GetAnimationID(DEER_DEFENSE_STATE);
	m_pDeer->GetAnimatedModel()->BlendCycle( l_Num, 0.1f );
}

void CDeerDefenseAnimationState::OnExit( CCharacter* _pCharacter )
{
	if (!m_pDeer) 
	{
		m_pDeer = dynamic_cast<CDeer*> (_pCharacter);
	}

	int l_Num = m_pDeer->GetAnimationID(DEER_DEFENSE_STATE);
	m_pDeer->GetAnimatedModel()->ClearCycle( l_Num, 0.1f );
}

bool CDeerDefenseAnimationState::OnMessage( CCharacter*, const STelegram& _Telegram )
{
	return false;
}