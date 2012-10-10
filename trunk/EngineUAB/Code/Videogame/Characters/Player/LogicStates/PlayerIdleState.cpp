#include "PlayerIdleState.h"
#include "StatesMachine\Telegram.h"
#include "DebugInfo\DebugRender.h"
#include "DebugGUIManager.h"
#include "ActionToInput.h"
#include "Helpers\MathHelper.h"
#include "Utils\Random.h"
#include "GameProcess.h"
#include "EngineProcess.h"
#include "SoundManager.h"

#include "Characters\CharacterManager.h"
#include "Characters\Player\Player.h"
#include "Characters\StatesDefs.h"
#include "Characters\Player\LogicStates\PlayerHitState.h"
#include "Steering Behaviors\SteeringEntity.h"

#include "Cameras\ThPSCharacterCamera.h"
#include "PhysicController.h"

#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"

#include "Logger\Logger.h"
#include "Core.h"
#include "Base.h"

#if defined _DEBUG
#include "Memory\MemLeaks.h"
#endif

CPlayerIdleState::CPlayerIdleState( CCharacter * _pCharacter, const std::string &_Name )
	: CState(_pCharacter, _Name)
	, m_pPlayer(NULL)
{
	m_pProcess	= static_cast<CGameProcess*>(CORE->GetProcess());
	m_pInput	= CORE->GetActionToInput();
}

CPlayerIdleState::~CPlayerIdleState()
{
	m_pProcess	= NULL;
	m_pInput	= NULL;
	m_pPlayer	= NULL;
}

void CPlayerIdleState::OnEnter( CCharacter* _pCharacter )
{
#if defined _DEBUG
	if( CORE->IsDebugMode() )
	{
		CORE->GetDebugGUIManager()->GetDebugRender()->SetStateName("Idle");
	}
#endif

	if( m_pPlayer == NULL )
		m_pPlayer = static_cast<CPlayer*>(_pCharacter);

	/*_pCharacter->SetYaw(2.56f);
	_pCharacter->GetAnimatedModel()->SetYaw(-1.20f);*/


	// Ahora debemos actualizar las partículas
	//UpdateParticlesPositions(_pCharacter);

	UpdateImpact(_pCharacter);
	GenerateImpact(_pCharacter);
}

void CPlayerIdleState::Execute( CCharacter* _pCharacter, float _fElapsedTime )
{
	// Ahora debemos actualizar las partículas
	UpdateParticlesPositions(_pCharacter);

	// Gestión de partículas
	UpdateImpact(_pCharacter);
	//GenerateImpact(_pCharacter);
	
	if( !_pCharacter->GetLocked() )
	{
		if( m_pPlayer == NULL )
			m_pPlayer = static_cast<CPlayer*>(_pCharacter);

		CCharacter *l_pEnemyDetected	= m_pPlayer->DetectEnemy();

		//if( l_pPlayer->IsTargetFixed() )
		//{
		//	if( l_pEnemyDetected != NULL )
		//	{
		//		if( !l_pEnemyDetected->IsAlive() )
		//		{
		//			static_cast<CGameProcess*>(CORE->GetProcess())->GetCharactersManager()->SetTargetEnemy(NULL);
		//			l_pPlayer->SetTargetFixed(false);
		//		}
		//	}
		//	else
		//	{
		//		static_cast<CGameProcess*>(CORE->GetProcess())->GetCharactersManager()->SetTargetEnemy(NULL);
		//		l_pPlayer->SetTargetFixed(false);
		//	}
		//}

		//if( l_pInput->DoAction("PlayerTarget") )
		//{
		//	//Se asigna un target
		//	if( !l_pPlayer->IsTargetFixed() )
		//	{
		//		if( l_pEnemyDetected != NULL )
		//		{
		//			static_cast<CGameProcess*>(CORE->GetProcess())->GetCharactersManager()->SetTargetEnemy( l_pEnemyDetected );
		//			l_pPlayer->SetTargetFixed(true);
		//		}
		//	}
		//	else
		//	{
		//		static_cast<CGameProcess*>(CORE->GetProcess())->GetCharactersManager()->SetTargetEnemy(NULL);
		//		l_pPlayer->SetTargetFixed(false);
		//	}
		//}

		if( m_pInput->DoAction("HardAttackPlayer") )
		{
			_pCharacter->GetLogicFSM()->ChangeState( _pCharacter->GetLogicState("attack4") );
			_pCharacter->GetGraphicFSM()->ChangeState( _pCharacter->GetAnimationState("animattack4") );
		}
		else if( m_pInput->DoAction("AttackPlayer") )
		{
			//El jugador ataca
			if( m_pPlayer->IsTargetFixed() )
			{
				_pCharacter->GetLogicFSM()->ChangeState( _pCharacter->GetLogicState("targetattack1") );
				_pCharacter->GetGraphicFSM()->ChangeState( _pCharacter->GetAnimationState("animattack1") );
			}
			else
			{
				_pCharacter->GetLogicFSM()->ChangeState( _pCharacter->GetLogicState("attack1") );
				_pCharacter->GetGraphicFSM()->ChangeState( _pCharacter->GetAnimationState("animattack1") );
			}
		}
		else if(	m_pInput->DoAction("MovePlayerUp") || 
					m_pInput->DoAction("MovePlayerDown") || 
					m_pInput->DoAction("MovePlayerLeft") ||
					m_pInput->DoAction("MovePlayerRight") )
		{
			/*if( l_pPlayer->IsTargetFixed() )
			{
				_pCharacter->GetLogicFSM()->ChangeState( _pCharacter->GetLogicState("targetrun") );
				_pCharacter->GetGraphicFSM()->ChangeState( _pCharacter->GetAnimationState("animrun") );
			}
			else
			{*/
				_pCharacter->GetLogicFSM()->ChangeState( _pCharacter->GetLogicState("run") );
				_pCharacter->GetGraphicFSM()->ChangeState( _pCharacter->GetAnimationState("animrun") );
			//}
		}

		//Actualización del yaw a partir del movimiento del mouse
		/*if( l_pPlayer->IsTargetFixed() )
		{
		Vect3f l_PlayerDir	= static_cast<CGameProcess*>(CORE->GetProcess())->GetPlayerCamera()->GetDirection();
		Vect3f l_EnemyDir	= static_cast<CGameProcess*>(CORE->GetProcess())->GetCharactersManager()->GetTargetEnemy()->GetPosition() - _pCharacter->GetPosition();

		l_PlayerDir.y		= 0.f;
		l_EnemyDir.y		= 0.f;

		l_PlayerDir.Normalize(1.f);
		l_EnemyDir.Normalize(1.f);

		float l_fDelta		= l_EnemyDir.Dot( l_PlayerDir );
		l_fDelta			= mathUtils::ACos<float>( l_fDelta );

		if( mathUtils::Rad2Deg( l_fDelta ) > 0.1f )
		{
		_pCharacter->SetYaw( Helper::AngleFilter(_pCharacter->GetYaw() - l_fDelta) );
		}
		}*/
	}

	//Actualiza el personaje
	_pCharacter->GetController()->Move( v3fZERO, _fElapsedTime );
}

void CPlayerIdleState::OnExit( CCharacter* _pCharacter )
{
	/*GetParticleEmitter("BloodSplash")->StopEjectParticles();
	GetParticleEmitter("Impact")->StopEjectParticles();
	GetParticleEmitter("ExpandWave")->StopEjectParticles();
	GetParticleEmitter("Streaks")->StopEjectParticles();*/
}

bool CPlayerIdleState::OnMessage( CCharacter* _pCharacter, const STelegram& _Message )
{
	if( _Message.Msg == Msg_Attack )
	{
		CRandom	l_Randomize;

		CCharacter *l_pEnemy	= m_pProcess->GetCharactersManager()->GetCharacterById(_Message.Sender);
		float l_fReceivedPain	= l_Randomize.getRandFloat( (float)(l_pEnemy->GetProperties()->GetStrong() / 2), (float)l_pEnemy->GetProperties()->GetStrong());
		float l_fPainToHit		= l_pEnemy->GetProperties()->GetStrong() * 0.95f;

		/*if( l_fReceivedPain >= l_fPainToHit )
		{*/
		STelegram l_Message = _Message;
		CPlayerHitState *l_HitState = dynamic_cast<CPlayerHitState*> (_pCharacter->GetLogicState("hit"));
		l_HitState->UpdateParameters(l_Message);
		_pCharacter->GetLogicFSM()->ChangeState( _pCharacter->GetLogicState("hit") );
		_pCharacter->GetGraphicFSM()->ChangeState( _pCharacter->GetAnimationState("animhit") );
		//}

		return true;
	}
	else if( _Message.Msg == Msg_Push )
	{
		/*CCharacter *l_pEnemy	= static_cast<CGameProcess*>(CORE->GetProcess())->GetCharactersManager()->GetCharacterById(_Message.Sender);
		
		sDireccion * l_Info = (struct sDireccion *) _Message.ExtraInfo;
		_pCharacter->MoveTo2(l_Info->Direccion*2.0f, l_Info->ElapsedTime);
		_pCharacter->GetSteeringEntity()->SetVelocity(Vect3f(0,0,0));*/

		return true;
	}

	return false;
}

void CPlayerIdleState::UpdateParticlesPositions( CCharacter* _pCharacter )
{
}

void CPlayerIdleState::GenerateImpact( CCharacter* _pCharacter )
{
	//GetParticleEmitterInstance("Twister", _pCharacter->GetName() + "_Twister")->EjectParticles();
	//CORE->GetSoundManager()->PlayEvent( _pCharacter->GetSpeakerName(), "Play_EFX_51467_missile_explosion" );
	
	//GetParticleEmitter("FireSwordBlur")->EjectParticles();
	//GetParticleEmitter("FireSwordSmoke")->EjectParticles();
	/*GetParticleEmitterInstance("DeerBloodSplash", _pCharacter->GetName() + "_DeerBloodSplash")->EjectParticles();
	GetParticleEmitterInstance("DeerBloodDust",	  _pCharacter->GetName() + "_DeerBloodDust")->EjectParticles();
	GetParticleEmitterInstance("DeerBlood",		  _pCharacter->GetName() + "_DeerBlood")->EjectParticles();*/

}

void CPlayerIdleState::UpdateImpact( CCharacter* _pCharacter )
{
	//Vect3f l_Pos = _pCharacter->GetPosition() + _pCharacter->GetFront();
	//l_Pos.y += _pCharacter->GetProperties()->GetHeightController();

	//l_Pos = _pCharacter->GetPosition();

	//SetParticlePosition(_pCharacter, "Twister", _pCharacter->GetName() + "_Twister", "", l_Pos );

	/*SetParticlePosition(_pCharacter, "DeerBloodSplash", _pCharacter->GetName() + "_DeerBloodSplash", "", l_Pos );
	SetParticlePosition(_pCharacter, "DeerBloodDust",	_pCharacter->GetName() + "_DeerBloodDust",	 "", l_Pos);
	SetParticlePosition(_pCharacter, "DeerBlood",		_pCharacter->GetName() + "_DeerBlood",	"", l_Pos);*/
	/*SetParticlePosition(_pCharacter, "FireSwordBlur", "CHR_CAP L Hand" );
	SetParticlePosition(_pCharacter, "FireSwordSmoke", "CHR_CAP L Hand" );*/

	//Vect3f l_Pos = _pCharacter->GetPosition() + _pCharacter->GetFront();
	//l_Pos.y += _pCharacter->GetProperties()->GetHeightController();
	//SetParticlePosition(_pCharacter, "BloodSplash", "", l_Pos );
	//SetParticlePosition(_pCharacter, "Impact", "", l_Pos );
	//SetParticlePosition(_pCharacter, "Streaks", "", l_Pos );

	////l_Pos.z -= 10.f;
	//SetParticlePosition(_pCharacter, "ExpandWave", "", l_Pos );
	
}
