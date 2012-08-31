#include "ObstacleWallAvoidance.h"
#include "StatesMachine\BaseGameEntity.h"
#include "Steering Behaviors\SteeringBehaviorsDefs.h"
#include "Steering Behaviors\SteeringEntity.h"

//#include "Characters\CharacterManager.h"
//#include "Characters\Character.h"
#include "Utils\TemplatedVectorMapManager.h"
//#include "GameProcess.h"

#include "PhysicsManager.h"
#include "PhysicController.h"
#include "PhysicUserData.h"
#include "PhysicActor.h"

#include "RenderManager.h"
#include "Fonts\FontManager.h"

#include "Math\Vector3.h"
#include "Math\Matrix44.h"
#include "Logger\Logger.h"
#include "Core.h"
#include "Base.h"

#if defined(_DEBUG)
	#include "Memory\MemLeaks.h"
#endif


// -----------------------------------------
//		  CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------

CObstacleWallAvoidance::CObstacleWallAvoidance( float _DetectionRayLength )
	: CSteering								( ::collision_avoidance )
	, m_ObstacleWallDetectionFeelerLength	( _DetectionRayLength )
{
}


CObstacleWallAvoidance::~CObstacleWallAvoidance(void)
{
}

// -----------------------------------------
//				MAIN METHODS
// -----------------------------------------

Vect3f CObstacleWallAvoidance::CalculateSteering( CSteeringEntity *_pEntity )	
{
	// Creo los bigotitos...
	CreateFeelers();

	SCollisionInfo sInfo;
	
	Vect3f l_SteeringForce;
	l_SteeringForce.SetZero();

	uint32 l_Mask = 1 << ECG_LIMITS;
	
	// Distancia al punto de intersecci�n m�s proximo
	float l_DistanceToClosestIP = 0.f;
	
	// Punto m�s proximo de colision
	Vect3f l_ClosestPoint;
	l_ClosestPoint.SetZero();

	// Vector normal de la superficie mas proxima
	Vect3f l_ClosestNormal;
	l_ClosestNormal.SetZero();

	for ( size_t flr = 0; flr < m_Feelers.size(); ++flr)
	{
		// Siempre hay que pasar la direcci�n normalizada
		CPhysicUserData * l_Data  = CORE->GetPhysicsManager()->RaycastClosestActor( _pEntity->GetInitialPositionToThrowRay(), m_Feelers[flr], l_Mask, sInfo, m_ObstacleWallDetectionFeelerLength );
		
		// Si ha colisionado con algo miramos si est� m�s proximo
		if ( l_Data ) 
		{
			if ( sInfo.m_fDistance < l_DistanceToClosestIP )
			{
				l_DistanceToClosestIP = sInfo.m_fDistance;		// metemos la nueva m�nima distancia de una entidad m�s pr�xima
					
				l_ClosestPoint = sInfo.m_CollisionPoint;		// Guardamos el punto de colisi�n m�s pr�ximo encontrado
				
				l_ClosestNormal = sInfo.m_Normal;				// Guardamos la normal de la superficie interceptada
			}
		
			// Si encuentro colision de algun rayo con algun actor le a�ado la fuerza correspondiente
			// Calculo por qu� distancia la posici�n proyectada del agente sobrepasar� el l�mite
			Vect3f l_OverShoot = m_Feelers[flr] - l_ClosestPoint;

			// Creamos una fuerza en la direcci�n de la normal del l�mite, con una magnitud create a force in the direction of the wall normal, con una magnitud de sobrepasado
			l_SteeringForce += l_ClosestNormal * l_OverShoot.Length();	
		}
	}
	
	return l_SteeringForce;
}

//------------------------------- CreateFeelers --------------------------
//
//  Creates the antenna utilized by WallAvoidance
//------------------------------------------------------------------------
void CObstacleWallAvoidance::CreateFeelers( void )
{
	// Rayo frontal 
	Vect3f l_Front = m_pEntity->GetFront();
	m_Feelers.push_back(m_pEntity->GetInitialPositionToThrowRay() + m_ObstacleWallDetectionFeelerLength * l_Front);

	// Rayo a la izquierda
	l_Front.RotateY(mathUtils::Deg2Rad(30.f));
	m_Feelers.push_back(m_pEntity->GetInitialPositionToThrowRay() + m_ObstacleWallDetectionFeelerLength/2.0f * l_Front);
	
	// Rayo a la derecha
	l_Front = m_pEntity->GetFront();
	l_Front.RotateY(mathUtils::Deg2Rad(-30.f));
	m_Feelers.push_back(m_pEntity->GetInitialPositionToThrowRay() + m_ObstacleWallDetectionFeelerLength/2.0f * l_Front);
}

//CPhysicUserData* CObstacleWallAvoidance::ThrowCollisionRays( CSteeringEntity *_pEntity, SCollisionInfo& _Info, float _MaxDistance )
//{
//	Vect3f l_SteeringForce;
//	l_SteeringForce.SetZero();
//
//	uint32 l_Mask = 1 << ECG_LIMITS;
//	
//	// Distancia al punto de intersecci�n m�s proximo
//	float l_DistanceToClosestIP = 0.f;
//	
//	// Punto m�s proximo de colision
//	Vect3f l_ClosestPoint;
//	l_ClosestPoint.SetZero();
//
//	// Vector normal de la superficie mas proxima
//	Vect3f l_ClosestNormal;
//	l_ClosestNormal.SetZero();
//
//	for ( int flr = 0; flr < m_Feelers.size(); ++flr)
//	{
//		// Siempre hay que pasar la direcci�n normalizada
//		CPhysicUserData * l_Data = CORE->GetPhysicsManager()->RaycastClosestActor( _pEntity->GetInitialPositionToThrowRay(), m_Feelers[flr].Normalize(), l_Mask, sInfo, _MaxDistance );
//		
//		// Si ha colisionado con algo miramos si est� m�s proximo
//		if ( l_Data ) 
//		{
//			if ( sInfo.m_fDistance < l_DistanceToClosestIP )
//			{
//				l_DistanceToClosestIP = sInfo.m_fDistance;		// metemos la nueva m�nima distancia de una entidad m�s pr�xima
//					
//				l_ClosestPoint = sInfo.m_CollisionPoint;		// Guardamos el punto de colisi�n m�s pr�ximo encontrado
//				
//				l_ClosestNormal = sInfo.m_Normal;				// Guardamos la normal de la superficie interceptada
//			}
//		
//			// Si encuentro colision de algun rayo con algun actor le a�ado la fuerza correspondiente
//			// Calculo por qu� distancia la posici�n proyectada del agente sobrepasar� el l�mite
//			Vect3f l_OverShoot = m_Feelers[flr] - l_ClosestPoint;
//
//			// Creamos una fuerza en la direcci�n de la normal del l�mite, con una magnitud create a force in the direction of the wall normal, con una magnitud de sobrepasado
//			l_SteeringForce += l_ClosestNormal * l_OverShoot.Length();	
//		}
//	}
//	
//	//Vect3f l_Direction = GetTarget() - _pEntity->GetPosition(); //+ _pEntity->GetFront();
//	//l_Direction.Normalize();
//
//	////Vect3f l_Position = _pEntity->GetPosition() + _pEntity->GetBoundingRadius() + _pEntity->GetFront()/2 ;
//	//Vect3f l_Position = _pEntity->GetPosition();
//	//l_Position.x += _pEntity->GetBoundingRadius() * 2;
//	//l_Position.y += _pEntity->GetBoundingRadius() / 2:
//	//l_Position.z += _pEntity->GetBoundingRadius() * 2;
//
//	// El rayo frontal no funcion� o no col�lision�. Lanzo otro en un lado del fustrum
//	//if ( !l_Data ) 
//	//{
//	//	Vect3f l_Front = _pEntity->GetFront();
//	//	l_Front.RotateY(mathUtils::Deg2Rad(30.f));
//	//	l_Dir = Vect3f ( GetTarget().x + l_Front.x + l_Front.x  , GetTarget().y + _pEntity->GetHeight(), GetTarget().z + l_Front.z + l_Front.z );
//	//	l_Dir.Normalize();
//	//	l_Data  = CORE->GetPhysicsManager()->RaycastClosestActor( l_Pos, l_Dir, l_Mask , s, _MaxDistance );
//
//	//	if ( l_Data )
//	//	{
//	//		std::string l_Name = l_Data->GetName().c_str();
//	//		if ( l_Name.compare(_pEntity->GetName()) != 0 ) 
//	//		{
//	//			LOGGER->AddNewLog( ELL_INFORMATION, "Colisi� LATERAL 30� d'enemic %s amb el player: %s", _pEntity->GetName().c_str(), l_Data->GetName().c_str());
//	//		}
//	//	}
//	//}
//
//	//// El rayo frontal no funcion� o no col�lision�. Lanzo otro en un lado del fustrum
//	//if ( !l_Data ) 
//	//{
//	//	Vect3f l_Front = _pEntity->GetFront();
//	//	l_Front.RotateY(mathUtils::Deg2Rad(30.f));
//	//	l_Dir = Vect3f ( GetTarget().x + l_Front.x + l_Front.x  , GetTarget().y + _pEntity->GetHeight(), GetTarget().z + l_Front.z + l_Front.z );
//	//	l_Dir.Normalize();
//	//	l_Data  = CORE->GetPhysicsManager()->RaycastClosestActor( l_Pos, l_Dir, l_Mask , s, _MaxDistance );
//
//	//	if ( l_Data )
//	//	{
//	//		std::string l_Name = l_Data->GetName().c_str();
//	//		if ( l_Name.compare(_pEntity->GetName()) != 0 ) 
//	//		{
//	//			LOGGER->AddNewLog( ELL_INFORMATION, "Colisi� LATERAL -30� d'enemic %s amb el player: %s", _pEntity->GetName().c_str(), l_Data->GetName().c_str());
//	//		}
//	//	}
//	//}
//	
//	return l_SteeringForce;
//}