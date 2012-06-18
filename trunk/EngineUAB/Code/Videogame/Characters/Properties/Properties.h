#pragma once
#ifndef __CLASS_PROPERTIES_H__
#define __CLASS_PROPERTIES_H__

#include <string>

#include "Utils\Active.h"
#include "Utils\Named.h"
#include "Math\Vector3.h"
#include "Object3D.h"

class CProperties : public CObject3D, public CActive
{
public:
	//--- Init and End protocols ----------------------------------------
						CProperties				( const std::string &_Name, int _Life = 0, int _Strong = 0, float _Speed = 0.f , 
												float _HeightController = 1.f, float _WidthController = 1.f, float _Slope  = 45.f, 
												float _SkinWidth  = 0.1f, float _StepOffset = 0.5, 
												const Vect3f &_Direction = Vect3f(0.f,0.f,0.f), const Vect3f &_Position = NULL );

						CProperties				( void );
						~CProperties			( void );

	//---- Main Methods --------------------------------------------------
	//---- Methods -------------------------------------------------------
	//---- Properties ( get & Set )---------------------------------------

	inline void					SetCore					( const std::string &_CoreName )					{ m_Core = _CoreName; }
	inline const std::string	GetCore					( void ) const										{ return m_Core; }

	inline void					SetAnimationInstance	( const std::string &_AnimationInstanceName )		{ m_AnimationInstanceName = _AnimationInstanceName; }
	inline std::string			GetAnimationInstance	( void ) const										{ return m_AnimationInstanceName; }

	inline void					SetLife					( int _Life )										{ m_Life = _Life; }
	inline const int			GetLife					( void ) const										{ return m_Life; }

	inline void					SetStrong				( int _Strong )										{ m_Strong = _Strong; }
	inline const int			GetStrong				( void ) const										{ return m_Strong; }

	inline void					SetSpeed				( float _Speed )									{ m_Speed = _Speed; }
	inline const float			GetSpeed				( void ) const										{ return m_Speed; }

	inline void					SetRotationSpeed		( float _RotationSpeed )							{ m_RotationSpeed = _RotationSpeed; }
	inline const float			GetRotationSpeed		( void ) const										{ return m_RotationSpeed; }

	inline void					SetDirection			( const Vect3f &_Direction )						{ m_Direction = _Direction; }
	inline const Vect3f			GetDirection			( void ) const										{ return m_Direction; }

	inline void					SetRespawnPosition		( const Vect3f &_RespawnPosition )					{ m_RespawnPosition = _RespawnPosition; }
	inline const Vect3f			GetRespawnPosition		( void ) const										{ return m_RespawnPosition; }

	inline void					SetDetectionDistance	( float _DetectionDistance )						{ m_DetectionDistance = _DetectionDistance; }
	inline float				GetDetectionDistance	( void ) const										{ return m_DetectionDistance; }
	
	inline void					SetChaseDistance		( float _ChaseDistance )							{ m_ChaseDistance = _ChaseDistance; }
	inline float				GetChaseDistance		( void ) const										{ return m_ChaseDistance; }

	inline void					SetFollowPathDistance	( float _FollowPathDistance )						{ m_FollowPathDistance = _FollowPathDistance; }
	inline float				GetFollowPathDistance	( void ) const										{ return m_FollowPathDistance; }

	inline void					SetPanicDistance		( float _PanicDistance )							{ m_PanicDistance = _PanicDistance; }
	inline float				GetPanicDistance		( void ) const										{ return m_PanicDistance; }

	inline void					SetTiredDistance		( float _TiredDistance )							{ m_TiredDistance = _TiredDistance; }
	inline float				GetTiredDistance		( void ) const										{ return m_TiredDistance; }

	inline void					SetAttackDistance		( float _AttackDistance )							{ m_AttackDistance = _AttackDistance; }
	inline float				GetAttackDistance		( void ) const										{ return m_AttackDistance; }

	inline void					SetHeightController		( float _Height )									{ m_HeightController = _Height; }
	inline const float			GetHeightController		( void ) const										{ return m_HeightController; }

	inline void					SetWidthController		( float _Width )									{ m_WidthController = _Width; }
	inline const float			GetWidthController		( void ) const										{ return m_WidthController; }

	inline void					SetSlopeController		( float _Slope )									{ m_Slope = _Slope; }
	inline const float			GetSlopeController		( void ) const										{ return m_Slope; }

	inline void					SetSkinWidthController	( float _SkinWidth )								{ m_SkinWidth = _SkinWidth; }
	inline const float			GetSkinWidthController	( void ) const										{ return m_SkinWidth; }

	inline void					SetStepOffsetController	( float _StepOffset )								{ m_StepOffset = _StepOffset; }
	inline const float			GetStepOffsetController	( void ) const										{ return m_StepOffset; }

	inline void					SetAnimationOffset		( float _AnimationOffset )							{ m_AnimationOffset = _AnimationOffset; }
	inline const float			GetAnimationOffset		( void ) const										{ return m_AnimationOffset; }

	inline void					SetBoundingRadious		( float _BoundingRadius )							{ m_BoundingRadius = _BoundingRadius; }
	inline const float			GetBoundingRadious		( void ) const										{ return m_BoundingRadius; }

	inline void					SetMaxSpeed				( float _MaxSpeed )									{ m_MaxSpeed = _MaxSpeed; }
	inline const float			GetMaxSpeed				( void ) const										{ return m_MaxSpeed; }

	//---- Members -------------------------------------------------------
private:
	std::string			m_Core;
	std::string			m_AnimationInstanceName;
	int					m_Life;
	int					m_Strong;
	float				m_Speed;
	float				m_RotationSpeed;								
	Vect3f				m_Direction;
	Vect3f				m_RespawnPosition;
	float				m_DetectionDistance;		// distancia de detecci�n de otros car�cteres
	float				m_ChaseDistance;			// distancia de persecuci�n 
	float				m_FollowPathDistance;		// distancia de persecuci�n a trav�s de waypoints
	float				m_AttackDistance;			// distancia de para poder atacar
	float				m_PanicDistance;			// distancia que recorre en estado de p�nico. Este para el estado de evade o flee
	float				m_TiredDistance;			// distancia de alejamiento del enemigo respecto al player cuando est� cansado. como flee pero de cara al player

	// Propiedades del controller y animacion
	float				m_HeightController;			// altura del controller
	float				m_WidthController;			// anchura del controller
	float				m_Slope;					// Slope del controller
	float				m_SkinWidth;				// par�metro que da el ancho de una piel que envuelve el controler con la que hace colision. Recomendado 0.1f
	float				m_StepOffset;				// cantidad para subir un escalon
	float				m_AnimationOffset;			// Offset para restar la coordenada Y de la animaci�n 

	// Propiedades de IA 
	float				m_BoundingRadius;
	float				m_MaxSpeed;

};

#endif __CLASS_PROPERTIES_H__