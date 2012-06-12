#include "Character.h"
#include "CharacterWrapper.h"
#include "StatesMachine\MessageDispatcher.h"
#include "PhysicController.h"
#include "PhysicUserData.h"
#include "PhysicsManager.h"
#include "Scripting\ScriptManager.h"
#include "RenderableObjects\AnimatedModel\AnimatedModelManager.h"
#include "RenderableObjects\AnimatedModel\AnimatedInstanceModel.h"
#include "RenderableObjects\RenderableObjectsManager.h"
#include "RenderableObjects\RenderableObjectsLayersManager.h"
#include "Properties/Properties.h"
#include "Core.h"
#include "Base.h"

#if defined (_DEBUG)
#include "Memory\MemLeaks.h"
#endif

// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------
CCharacter::CCharacter()
	: CBaseGameEntity			( CBaseGameEntity::GetNextValidID() )
	, m_pLogicStateMachine		( NULL )
	, m_pGraphicStateMachine	( NULL )
	, m_pCurrentAnimatedModel	( NULL )
	, m_pController				( NULL )
	, m_pAnimationsStates		( NULL )
	, m_pPhysicUserDataJugador	( NULL )
	, CNamed					( )
	, CObject3D					( )
	, m_PrevPosition			( Vect3f(0.f, 0.f, 0.f) )
	, m_bLocked					( false )
	, m_bIsEnable				( true )
{
	// coloco la m�quina de estados i el controler de f�sica
    m_pLogicStateMachine	= new CStateMachine<CCharacter>( this );
	m_pGraphicStateMachine	= new CStateMachine<CCharacter>( this );
   // m_pController			= new CPhysicController();

	//m_pCurrentAnimatedModel = static_cast<CAnimatedInstanceModel*>(CORE->GetRenderableObjectsLayersManager()->GetResource("solid")->GetInstance("caperucita1"));
}

CCharacter::CCharacter( const std::string &_Name )
	: CBaseGameEntity			( CBaseGameEntity::GetNextValidID() )
	, m_pLogicStateMachine		( NULL )
	, m_pGraphicStateMachine	( NULL )
	, m_pCurrentAnimatedModel	( NULL )
	, m_pController				( NULL ) 
	, m_pAnimationsStates		( NULL )
	, m_pProperties				( NULL )
	, m_pPhysicUserDataJugador	( NULL )
	, CNamed					( _Name )
	, CObject3D					( )
	, m_PrevPosition			( Vect3f(0.f, 0.f, 0.f) )
	, m_bLocked					( false )
	, m_bIsEnable				( true )
{
	// coloco la m�quina de estados
    m_pLogicStateMachine	= new CStateMachine<CCharacter>( this );
	m_pGraphicStateMachine	= new CStateMachine<CCharacter>( this );
  //  m_pController			= new CPhysicController();

	//m_pCurrentAnimatedModel = static_cast<CAnimatedInstanceModel*>(CORE->GetRenderableObjectsLayersManager()->GetResource("solid")->GetInstance("caperucita1"));
}

CCharacter::CCharacter(int _ID, const std::string &_Name)
	: CBaseGameEntity			( _ID )
	, CNamed					( _Name )
	, CObject3D					( )
	, m_pLogicStateMachine		( NULL )
	, m_pGraphicStateMachine	( NULL )
	, m_pCurrentAnimatedModel	( NULL )
	, m_pController				( NULL ) 
	, m_pAnimationsStates		( NULL )
	, m_pProperties				( NULL )
	, m_pPhysicUserDataJugador	( NULL )
	, m_PrevPosition			( Vect3f(0.f, 0.f, 0.f) )
	, m_bLocked					( false )
	, m_bIsEnable				( true )
{
	// coloco la m�quina de estados
    m_pLogicStateMachine	= new CStateMachine<CCharacter>( this );
	m_pGraphicStateMachine	= new CStateMachine<CCharacter>( this );

}


CCharacter::~CCharacter( void )
{
	CHECKED_DELETE ( m_pLogicStateMachine );
	CHECKED_DELETE ( m_pGraphicStateMachine );
	// Amb lua no cal eliminar l'objecte. Lua ja se'n ocupa.
	CORE->GetPhysicsManager()->ReleasePhysicController(m_pController);
	CHECKED_DELETE ( m_pController );
	CHECKED_DELETE ( m_pPhysicUserDataJugador );
	m_pCurrentAnimatedModel = NULL;
}

// -----------------------------------------
//			METODES PRINCIPALS
// -----------------------------------------
bool CCharacter::Init( void )
{
	// Metodo y cosas a implementar en Lua
	//if ( m_pCurrentAnimatedModel )
	//{
	//	// coloco el primer estado
	//	m_pLogicStateMachine->SetCurrentState  ( m_pIdleState );
	//	m_pGraphicStateMachine->SetCurrentState( m_pAnimationIdleState );
	//}

	return true;
}

bool CCharacter::Initialize ( const std::string &_Name, const Vect3f &_InitialPosicion, ECollisionGroup _Grup )
{
	// Primero debemos adjuntar el modelo animado. Ojo! este nos da la posici�n
	CRenderableObjectsLayersManager *l_ROLayerManager = CORE->GetRenderableObjectsLayersManager();
	CRenderableObjectsManager *l_ROManager = l_ROLayerManager->GetResource("solid");
	CRenderableObject *l_RO = l_ROManager->GetInstance( m_pProperties->GetAnimationInstance() );

	if ( !l_RO ) 
	{
		l_ROManager->AddAnimatedMeshInstance( m_pProperties->GetName(), Vect3f (0.f, 0.f, 0.f ) );
	}
	else
	{
		m_pCurrentAnimatedModel = static_cast<CAnimatedInstanceModel*> (l_RO);
	}

	// Ahora creamos la parte f�sica del controller del jugador
	m_pPhysicUserDataJugador = new CPhysicUserData( m_Name );
	m_pPhysicUserDataJugador->SetColor( colYELLOW );
	m_pPhysicUserDataJugador->SetPaint( true );

	// Creo el controlador del jugador
	m_pController = new CPhysicController( m_pProperties->GetWidthController(), m_pProperties->GetHeightController(), m_pProperties->GetSlopeController(), 
										   m_pProperties->GetSkinWidthController(), m_pProperties->GetStepOffsetController(), _Grup, m_pPhysicUserDataJugador );
	Vect3f l_Position; 
	if ( _InitialPosicion != NULL )
	{
		l_Position = _InitialPosicion;
	}
	else
	{
		l_Position = m_pCurrentAnimatedModel->GetPosition();
	}
	m_pController->SetPosition( l_Position );
	m_pController->SetVisible( true );
	
	CORE->GetPhysicsManager()->AddPhysicController( m_pController );
	m_pController->Move( l_Position, 0.f );

	// Metemos el yaw y posici�n del modelo animado al controller
	if ( m_pCurrentAnimatedModel )
	{
		// Actualizamos el Yaw y lo asignamos al controler
		float l_Yaw = m_pCurrentAnimatedModel->GetYaw();
		m_pCurrentAnimatedModel->SetYaw( l_Yaw + mathUtils::Rad2Deg( m_pProperties->GetYaw() ) );
		m_pController->SetYaw( m_pProperties->GetYaw() );
	}
	
	MoveTo( l_Position, 0.0f );
	return true;
}

void CCharacter::Update ( float _ElapsedTime )			
{ 
	/*m_pLogicStateMachine->Update( );
	m_pGraphicStateMachine->Update( );*/
}

//-------------------------------------------------------------------------------------------------
// HandleMessage
//-------------------------------------------------------------------------------------------------
bool CCharacter::HandleMessage( const Telegram& _Msg, bool _Logic, bool _Graphic )
{
	if ( _Logic )
		m_pLogicStateMachine->HandleMessage( _Msg );
	
	if ( _Graphic )
		m_pGraphicStateMachine->HandleMessage( _Msg );

	return true;
}

//-------------------------------------------------------------------------------------------------
// HandleMessage
//-------------------------------------------------------------------------------------------------
bool CCharacter::HandleMessage( const Telegram& _Msg )
{
	bool l_CanHandle;
	l_CanHandle = m_pLogicStateMachine->HandleMessage		( _Msg );
	l_CanHandle &= m_pGraphicStateMachine->HandleMessage	( _Msg );

	return l_CanHandle;
}

void CCharacter::MoveController(const Vect3f &_Dir, float _ElapsedTime)
{
	m_pController->Move( _Dir, _ElapsedTime );
}

void CCharacter::FaceTo( const Vect3f &_Position, float _ElapsedTime )
{
	Vect3f v = (_Position - m_Position);
	float l_RotationSpeed = m_pProperties->GetRotationSpeed();
	float l_back = v.Dot(GetFront());

	if(abs(l_back) < .9f)
	{
		return;
	}

	if ( l_back < 0 )
	{
		m_fYaw += (mathUtils::Deg2Rad(l_RotationSpeed) * _ElapsedTime);
	}
	else
	{
		m_fYaw += (-mathUtils::Deg2Rad(l_RotationSpeed) * _ElapsedTime);
	}
}

void CCharacter::MoveTo( const Vect3f &_Position, float _ElapsedTime )
{
	Vect2f pointA(_Position.x, _Position.z);
	Vect2f pointB(m_Position.x, m_Position.z);

	if(pointA.SqDistance(pointB) <= m_pProperties->GetAttackDistance())
	{
		FaceTo( _Position, _ElapsedTime );
		m_pController->SetYaw(m_fYaw);
		Vect3f l_Position = Vect3f(0.0f, 0.0f, 0.0f);
		MoveController(l_Position, _ElapsedTime);

		m_Position = m_pController->GetPosition();
		m_Position.y = m_Position.y - m_pController->GetHeight() + 0.4f;
		float l_Yaw = mathUtils::Rad2Deg(m_fYaw);
		m_pCurrentAnimatedModel->SetYaw(-l_Yaw + 90.f );
		m_pCurrentAnimatedModel->SetPosition( m_Position );
		return;
	}

	FaceTo( _Position, _ElapsedTime );

	Vect3f pointA2(_Position.x, 0, _Position.z);
	Vect3f pointB2(m_Position.x, 0, m_Position.z);
	Vect3f l_Position = Vect3f(0.0f, 0.0f, 0.0f);
	Vect3f l_Dir = (pointA2 - pointB2).Normalize();

	l_Position += l_Dir * m_pProperties->GetSpeed() * _ElapsedTime;

	m_pController->SetYaw(m_fYaw);
	MoveController(l_Position, _ElapsedTime);
	
	m_Position = m_pController->GetPosition();
	m_Position.y = m_Position.y - m_pController->GetHeight() + 0.4f;
	float l_Yaw = mathUtils::Rad2Deg(m_fYaw);
	m_pCurrentAnimatedModel->SetYaw(-l_Yaw + 90.f );
	m_pCurrentAnimatedModel->SetPosition( m_Position );
}

// -----------------------------------------
//				PROPERTIES
// -----------------------------------------

void CCharacter::AddLife( int _Life )								
{ 
	m_pProperties->SetLife( m_pProperties->GetLife() + _Life ); 
}

void CCharacter::RestLife( int _Life )								
{ 
	m_pProperties->SetLife( m_pProperties->GetLife() - _Life ); 
}

void CCharacter::AddSpeed( int _Speed )
{
	m_pProperties->SetSpeed( m_pProperties->GetSpeed() - _Speed ); 
}

void CCharacter::RestSpeed( int _Speed )
{
	m_pProperties->SetSpeed( m_pProperties->GetSpeed() - _Speed ); 
}

void CCharacter::AddStrong( int _Strong )
{
	m_pProperties->SetStrong( m_pProperties->GetStrong() - _Strong ); 
}

void CCharacter::RestStrong( int _Strong )
{
	m_pProperties->SetStrong( m_pProperties->GetStrong() - _Strong ); 
}

int CCharacter::GetAnimationID( const std::string &_AnimationName )
{
	CAnimatedCoreModel * l_Core =  m_pCurrentAnimatedModel->GetAnimatedCoreModel();
	return l_Core->GetCoreModel()->getCoreAnimationId( _AnimationName );
}

void CCharacter::SetEnable( bool _Enable )
{
	m_bIsEnable = _Enable;
}