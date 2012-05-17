#include "Enemy.h"
#include "Characters\PlayerDef.h"

#include "Cameras\Camera.h"
#include "Cameras\ThPSCamera.h"
#include "Cameras\FPSCamera.h"

#include "ActionToInput.h"
#include "RenderManager.h"

//#include "PlayerController.h"

#include "RenderableObjects\RenderableObjectsLayersManager.h"
#include "RenderableObjects\RenderableObjectsManager.h"
#include "RenderableObjects\RenderableObject.h"
#include "RenderableObjects\AnimatedModel\AnimatedModelManager.h"
#include "RenderableObjects\AnimatedModel\AnimatedCoreModel.h"
#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"

#include "StatesMachine\EntityManager.h"

#include "Characters\Properties\Properties.h"

#include "Characters\States\AnimationsStates.h"
#include "Characters\States\PursuitState.h"
#include "Characters\States\IdleState.h"
#include "Characters\States\AnimationIdleState.h"
#include "Characters\States\AnimationPursuitState.h"

#include "Math\Matrix44.h"
#include "Math\Plane.h"

#include "Base.h"
#include "Core.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif

// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CEnemy::CEnemy( int _ID )
	: CCharacter				( _ID )								
	, m_pEnemyProperties		( NULL )
	, m_bMoverAutomatico		( false )
	, m_bLockCamera				( false )
	, m_pPursuitState			( NULL )
	, m_pIdleState				( NULL )
	, m_pAnimationPursuitState	( NULL )
	, m_pAnimationIdleState		( NULL )
{
}

CEnemy::CEnemy( int _ID, const std::string &_Name )
	: CCharacter				( _ID, _Name )					
	, m_pEnemyProperties		( NULL )
	, m_bMoverAutomatico		( false )
	, m_bLockCamera				( false )
	, m_pPursuitState			( NULL )
	, m_pIdleState				( NULL )
	, m_pAnimationPursuitState	( NULL )
	, m_pAnimationIdleState		( NULL )
{
}

CEnemy::~CEnemy( void )
{
	Done();
}

// -----------------------------------------
//			METODES PRINCIPALS
// -----------------------------------------
void CEnemy::Done ( void )
{
	if ( IsOK ( ) )
	{
		Release();
		m_bIsOk = false;
	}
}

bool CEnemy::Init ( void )
{
	// Aqu� ya deber�a tener cargadas las propiedades del player
	m_bIsOk = CCharacter::Init( m_pEnemyProperties->GetName(), m_pEnemyProperties->GetPosition(), ::ECG_ENEMICS );
	if ( !m_bIsOk )
		return false;

	// Inicializo estados
	m_pPursuitState				= new CPursuitState();
	m_pIdleState				= new CIdleState();
	m_pAnimationIdleState		= new CAnimationIdleState();
	m_pAnimationPursuitState	= new CAnimationPursuitState();

	CRenderableObjectsLayersManager *l_ROLayerManager = CORE->GetRenderableObjectsLayersManager();
	CRenderableObjectsManager *l_ROManager = l_ROLayerManager->GetResource("solid");
	CRenderableObject *l_RO = l_ROManager->GetInstance( m_pEnemyProperties->GetAnimationInstance() );
		
	if ( !l_RO ) 
		l_ROManager->AddAnimatedMeshInstance( m_pEnemyProperties->GetName(), Vect3f (0.f, 0.f, 0.f ) );
	else
		m_pCurrentAnimatedModel = static_cast<CAnimatedInstanceModel*> (l_RO);

	if ( m_pCurrentAnimatedModel )
	{
		// coloco el primer estado
		m_pLogicStateMachine->SetCurrentState( m_pIdleState );
		m_pGraphicStateMachine->SetCurrentState( m_pAnimationIdleState );
	}
	
	return m_bIsOk;
}

void CEnemy::Release ( void )
{
	m_pEnemyProperties = NULL;
	m_pEnemyAnimationsStates = NULL;
	
	CHECKED_DELETE ( m_pPursuitState );
	CHECKED_DELETE ( m_pIdleState );

	CHECKED_DELETE ( m_pAnimationIdleState );
	CHECKED_DELETE ( m_pAnimationPursuitState );
}

void CEnemy::Update( float _ElapsedTime ) //, CCamera *_pCamera)
{
	//if (m_bMoverAutomatico)
	//	MoverAutomaticamente( _ElapsedTime );
	//else
	//	MoverManualmente ( _ElapsedTime );

	//// Actualizamos los estados y caracter�sticas generales adem�s de mover el F�sic Controler
	//Vect3f	l_PosAnterior = m_pController->GetController()->GetPosition() ;
	//CCharacter::Update( _ElapsedTime );
	//Vect3f	l_PosActual	= m_pController->GetController()->GetPosition();
	//
	//if ( l_PosAnterior != l_PosActual )
	//{
	//	l_PosActual.y -= 1.5f;
	//	m_pCurrentAnimatedModel->SetPosition ( l_PosActual );
	//	m_pLogicStateMachine->ChangeState	 ( m_pPursuitState);
	//	m_pGraphicStateMachine->ChangeState  ( m_pAnimationPursuitState );


	//	//float l_x = 0.f;
	//	//float l_y = 0.f;
	//	//float l_z = 0.f;

	//	////m_pController->GetController()->GetPosition().GetAngles ( l_x, l_y, l_z );
	//	//m_pCurrentAnimatedModel->SetYaw( m_pController->GetController()->GetYaw() );
	//	//m_pPlayerProperties->SetYaw( m_pController->GetController()->GetYaw() );
	//	//m_pCurrentAnimatedModel->SetYaw(CORE->GetCamera()->GetLookAt().GetAngleZ());
	//}
	//else
	//{
	//	m_pLogicStateMachine->ChangeState	( m_pIdleState );
	//	m_pGraphicStateMachine->ChangeState ( m_pAnimationIdleState );
	//}
}

bool CEnemy::HandleMessage( const Telegram& _Msg, bool _Logic, bool _Graphic )
{
	return CCharacter::HandleMessage( _Msg, _Logic, _Graphic );
}

void CEnemy::MoverAutomaticamente ( float _ElapsedTime )
{
	//srand (static_cast <unsigned int>(time(0)));   // n� entre 0 i 32767
	//int randomNumber = rand ();	
	//int RandomDirection = (randomNumber % 8);		// obtinc un n� entre 0 i 8 - Totes les direccions
	//Vect3f Direccio = ( 0.f, 0.f, 0.f );

	//switch ( RandomDirection )
	//{
	//	case Up: 
	//		Direccio = Vect3f ( cosf ( m_Yaw ) , m_Pitch, sinf ( m_Yaw ) );
	//		m_Position += Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//		break;
	//	case UpRight:
	//		Direccio = Vect3f( cosf( m_Yaw - D3DX_PI/4.f ), m_Pitch, sinf(m_Yaw - D3DX_PI/4.f ) );
	//		m_Position += Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//		break;
	//	case UpLeft:
	//		Direccio = Vect3f( cosf( m_Yaw + D3DX_PI/4.f ), m_Pitch, sinf( m_Yaw + D3DX_PI/4.f ) );
	//		m_Position +=  Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//		break;
	//	case Back:
	//		Direccio = Vect3f ( cosf ( m_Yaw ) , m_Pitch, sinf ( m_Yaw ) );
	//		m_Position -= Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//	case BackLeft: 
	//		Direccio = ( Vect3f (cosf ( m_Yaw - D3DX_PI/4) , m_Pitch, sinf ( m_Yaw - D3DX_PI/4) ) );
	//		m_Position -= Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//	case BackRight:
	//		Direccio = ( Vect3f ( cosf ( m_Yaw + D3DX_PI/4) , m_Pitch, sinf ( m_Yaw + D3DX_PI/4 ) ) );
	//		m_Position -= Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//	case Left:
	//		Direccio = Vect3f ( cosf ( m_Yaw + D3DX_PI/2) , m_Pitch, sinf ( m_Yaw + D3DX_PI/2) );
	//		m_Position += Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//	case Right:
	//		Direccio = Vect3f ( cosf ( m_Yaw + D3DX_PI/2) , m_Pitch, sinf ( m_Yaw + D3DX_PI/2) );
	//		m_Position -= Direccio * QUANTITAT_MOVIMENT * _ElapsedTime;
	//}
}

void CEnemy::MoverManualmente ( float _ElapsedTime )
{
	//float	l_Dt;
	//float	l_fYaw		= m_pEnemyProperties->GetYaw();
	//float	l_fPitch	= m_pEnemyProperties->GetPitch();
	//float	l_fRoll		= m_pEnemyProperties->GetRoll();
	//Vect3f	l_Position	= m_pEnemyProperties->GetPosition();

	//if ( CORE->GetActionToInput()->DoAction( ACTION_LOCK_FREE_CAMERA , l_Dt  ) )
	//	m_bLockCamera=!m_bLockCamera;
	//
	//if(m_bLockCamera)
	//	return;

	//Vect3f l_Direccio = ( 0.f, 0.f, 0.f );
	//
	//// Comprovem la Rotaci� del player
	//if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_FREE_CAMERA_X , l_Dt ) )
	//{
	//	l_fYaw += l_Dt;
	//	if ( l_fYaw > 2.0f * FLOAT_PI_VALUE )
	//		l_fYaw -= 2.0f * FLOAT_PI_VALUE;
	//	else if ( l_fYaw < -2.0f * FLOAT_PI_VALUE )
	//		l_fYaw -= 2.0f * FLOAT_PI_VALUE;
	//}

	//if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_FREE_CAMERA_Y, l_Dt ) )
	//{
	//	l_fPitch += l_Dt;
	//	l_fPitch = min ( max ( -FLOAT_PI_VALUE/2.1f, l_fPitch ), FLOAT_PI_VALUE/2.1f );
	//}

	//// Comprovem el moviment del player	
	//if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_FOWARD ) )
	//{
	//	if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_LEFT ) )
	//	{
	//		l_Direccio = Vect3f( cosf( l_fYaw + D3DX_PI/4.f ), l_fPitch, sinf( l_fYaw + D3DX_PI/4.f ) );
	//		l_Position +=  l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	else if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_RIGHT ) )
	//	{
	//		l_Direccio = Vect3f( cosf( l_fYaw - D3DX_PI/4.f ), l_fPitch, sinf(l_fYaw - D3DX_PI/4.f ) );
	//		l_Position += l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	else
	//	{
	//		l_Direccio = Vect3f ( cosf ( l_fYaw ) , l_fPitch, sinf ( l_fYaw ) );
	//		l_Position += l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	
	//	if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_UP ) )
	//	{
	//		l_Direccio = Vect3f ( 0 , 1, 0 );
	//		l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	else if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_DOWN ) )
	//	{
	//		l_Direccio = Vect3f ( 0, 1, 0 );
	//		l_Position += l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//}
	//else if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_BACK ) )
	//{
	//	if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_LEFT ) )
	//	{
	//		l_Direccio = ( Vect3f (cosf ( l_fYaw - D3DX_PI/4) , 0, sinf ( l_fYaw - D3DX_PI/4) ) );
	//		l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	
	//	}
	//	else if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_RIGHT ) )
	//	{
	//		l_Direccio = ( Vect3f ( cosf ( l_fYaw + D3DX_PI/4) , 0, sinf ( l_fYaw + D3DX_PI/4 ) ) );
	//		l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	else
	//	{
	//		l_Direccio = Vect3f ( cosf ( l_fYaw ) , l_fPitch, sinf ( l_fYaw ) );
	//		l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	
	//	if ( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_UP ) )
	//	{
	//		l_Direccio = Vect3f ( 0 , 1, 0 );
	//		l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}
	//	else if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_DOWN ) )
	//	{
	//		l_Direccio = Vect3f ( 0, 1, 0 );
	//		l_Position += l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//	}

	//}
	//else if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_RIGHT ) )
	//{
	//	l_Direccio = Vect3f ( cosf ( l_fYaw + D3DX_PI/2) , 0, sinf ( l_fYaw + D3DX_PI/2) );
	//	l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//}
	//else if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_LEFT ) )
	//{
	//	l_Direccio = Vect3f ( cosf ( l_fYaw + D3DX_PI/2) , 0, sinf ( l_fYaw + D3DX_PI/2) );
	//	l_Position += l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//}
	//else if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_UP ) )
	//{
	//	l_Direccio = Vect3f ( 0 , 1, 0 );
	//	l_Position -= l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//}
	//else if( CORE->GetActionToInput()->DoAction( ACTION_MOVE_CAMERA_DOWN ) )
	//{
	//	l_Direccio = Vect3f ( 0, 1, 0 );
	//	l_Position += l_Direccio * QUANTITAT_MOVIMENT_CAMERA * _ElapsedTime;
	//}
}

// Codi MARC
//void CEnemy::UpdateInputActions( float _ElapsedTime ) //, CCamera *_pCamera)
//{
//	float d = 0.0f;
//	CActionToInput *l_pAction2Input = CORE->GetActionToInput();
//	//CThPSCamera* l_ThPSCamera = static_cast<CThPSCamera*>(_pCamera);
//	if( l_pAction2Input->DoAction("YawViewerCam", d) )
//	{
//		m_fYaw += d;
//		if( m_fYaw > e2PIf )
//		{
//			m_fYaw -= e2PIf;
//		}
//		else if( m_fYaw < -e2PIf )
//		{
//			m_fYaw += e2PIf;
//		}
//	}
//
//	if( l_pAction2Input->DoAction("PitchViewerCam", d) )
//	{
//		float l_Pitch = m_fPitch + d;
//		if( abs(l_Pitch) < ePIf/2 )
//			m_fPitch = l_Pitch;
//	}
//
//	/*if( action2Input->DoAction("MoveZViewerCam", d) )
//	{
//		m_Dir = Vect3f(mathUtils::Cos<float>(m_fYaw), m_fPitch, mathUtils::Sin<float>(m_fYaw));
//		if( d > 0 )
//		{
//			m_Position -= m_Dir * 50.f * _ElapsedTime;
//		}
//		else
//		{
//			m_Position += m_Dir * 50.f * _ElapsedTime;
//		}
//	}
//
//	if( action2Input->DoAction("SlowZoomViewerCam", d) )
//	{
//		float zoom = l_ThPSCamera->GetZoom() + d;
//		l_ThPSCamera->SetZoom(zoom);
//	}
//	else if( action2Input->DoAction("ZoomViewerCam", d) )
//	{
//		float zoom = l_ThPSCamera->GetZoom() + d;
//		l_ThPSCamera->SetZoom(zoom);
//	}*/
//
//
//	if( l_pAction2Input->DoAction("MovePlayerUp") )
//	{
//		if( l_pAction2Input->DoAction("MovePlayerLeft") )
//		{
//			m_vDirection = Vect3f(mathUtils::Cos<float>(m_fYaw + ePIf/4.f), m_fPitch, mathUtils::Sin<float>(m_fYaw + ePIf/4.f));
//			m_Position += m_vDirection * 15.f * _ElapsedTime;
//		}
//		else if( l_pAction2Input->DoAction("MovePlayerRight") )
//		{
//			m_vDirection = Vect3f(mathUtils::Cos<float>(m_fYaw - ePIf/4.f), m_fPitch, mathUtils::Sin<float>(m_fYaw - ePIf/4.f));
//			m_Position += m_vDirection * 15.f * _ElapsedTime;
//		}
//		else
//		{
//			m_vDirection = Vect3f(mathUtils::Cos<float>(m_fYaw), m_fPitch, mathUtils::Sin<float>(m_fYaw));
//			m_Position += m_vDirection * 15.f * _ElapsedTime;
//		}
//	}
//	else if( l_pAction2Input->DoAction("MovePlayerDown") )
//	{
//		if( l_pAction2Input->DoAction("MovePlayerLeft") )
//		{
//			m_vDirection = Vect3f(mathUtils::Cos<float>(m_fYaw - ePIf/4.f), m_fPitch, mathUtils::Sin<float>(m_fYaw - ePIf/4.f));
//			m_Position -= m_vDirection * 15.f * _ElapsedTime;
//		}
//		else if( l_pAction2Input->DoAction("MovePlayerRight") )
//		{
//			m_vDirection = Vect3f(mathUtils::Cos<float>(m_fYaw + ePIf/4.f), m_fPitch, mathUtils::Sin<float>(m_fYaw + ePIf/4.f));
//			m_Position -= m_vDirection * 15.f * _ElapsedTime;
//		}
//		else
//		{
//			m_vDirection = Vect3f(mathUtils::Cos<float>(m_fYaw), m_fPitch, mathUtils::Sin<float>(m_fYaw));
//			m_Position -= m_vDirection * 15.f * _ElapsedTime;
//		}
//	}
//	else
//	{
//		if( l_pAction2Input->DoAction("MovePlayerLeft") )
//		{
//			m_vDirection = Vect3f(cosf(m_fYaw + ePIf/2.f), 0.0f, sinf(m_fYaw + ePIf/2.f));
//			m_Position += m_vDirection * 15.f * _ElapsedTime;
//		}
//
//		if( l_pAction2Input->DoAction("MovePlayerRight") )
//		{
//			m_vDirection = Vect3f(cosf(m_fYaw + ePIf/2.f), 0.0f, sinf(m_fYaw + ePIf/2.f));
//			m_Position -= m_vDirection * 15.f * _ElapsedTime;
//		}
//	}
//}


void CEnemy::Render( CRenderManager *_RM )
{
	//Mat44f matTotal, matTranslacio, matRotacioYaw, matRotacioPitch;

	//matTotal.SetIdentity ();
	//matTranslacio.SetIdentity();
	//matRotacioYaw.SetIdentity();
	//matRotacioPitch.SetIdentity();

	//matRotacioYaw.SetRotByAngleY ( -m_fYaw );
	//matRotacioPitch.SetRotByAngleZ ( m_fPitch );
	//matTranslacio.Translate( m_Position) ;							// moc segons tecles pitjades

	//matTotal = matTranslacio * matRotacioYaw * matRotacioPitch;		// Obtinc la matriu final

	//_RM->SetTransform( matTotal );									// Roto + Trasllado 
	////_RM->DrawSphere(0.5f, 7, colWHITE);	
	//_RM->DrawCube ( Vect3f ( 1.f, 1.f, 1.f) , colWHITE );			// Dibuixo
}

void CEnemy::MoveCharacterToDestination ( Vect3f _Destination, float _ElapsedTime )
{
	// Pillo la ultima posici�n a conseguir y me encaro sobre ella. 
	Vect3f l_LastPos = _Destination;		
	Vect3f l_LastPositionNormalizated = (l_LastPos - this->GetPosition() ).Normalize();		// Normalizo el vector direcci�n entre ambas posiciones

	// Calculo el �ngulo
	float l_vAngle = GetFront() * l_LastPositionNormalizated;
	// Si el �ngulo es cerrado y est� entre la posici�n a donde ir voy y sino roto hasta encontrarla. Valores entre -1 i 1 (sin/cos)
	if ( l_vAngle >= 0.9f ) 
	{
		Vect3f l_NewPosition			 = GetPosition( ) + GetFront( ) * _ElapsedTime * ( (float) m_pEnemyProperties->GetSpeed() );
		Vect3f l_NewPositionNormalizated = ( l_LastPos - l_NewPosition ).Normalize();
		if ( ( l_NewPositionNormalizated * l_LastPositionNormalizated ) > 0  )
		{
			SetPosition( l_NewPosition );
		}
		else {
			SetPosition( l_LastPos );
		}
	}
	else 
	{
		float l_RotationSpeed =	0.5;	//GetRotationSpeed ();
		if( isPointAtLeft( l_LastPos ) ) {
			AddYaw( -l_RotationSpeed * _ElapsedTime );
			if( !isPointAtLeft( l_LastPos ) ) 
			{
				float l_fYaw = l_LastPositionNormalizated.GetAngleY(); // .xzToAngle ();
				SetYaw ( l_fYaw );
			}
		} else {
			AddYaw( l_RotationSpeed * _ElapsedTime );
			if( isPointAtLeft( l_LastPos ) ) 
			{
				float l_fYaw = l_LastPositionNormalizated.GetAngleY(); // xzToAngle ();
				SetYaw ( l_fYaw );
			}
		}
	}
}