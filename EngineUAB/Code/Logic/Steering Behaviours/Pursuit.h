#pragma once

#ifndef __CLASS_PURSUIT_H__
#define __CLASS_PURSUIT_H__

#include "Math\Vector2.h"
#include "Steering Behaviours\Steering.h"
#include "SteeringBehavioursDefs.h"

//--- Foward Declarations ---
class CSteeringEntity;
class CArrive;
//---------------------------

class CPursuit : public CSteering
{
public:
	//--- Init and End protocols------------------------------------------
							CPursuit			( eDeceleration _Deceleration, float _DecelerationBeginsAtDistance );
	virtual					~CPursuit			( void );

	//---- Main Functions ---------------------------------------
	virtual	Vect2f			CalculateSteering	( CSteeringEntity *_pEntity );					// M�todo que deber� ser sobreescrito en cada clase que herede de Steering
    void					UpdateEvaderEntity	( CSteeringEntity *_pEvader );

	//---- Functions ---------------------------------------
	
	//---- Properties ( get & Set )---------------------------------------
	inline eBehaviorType		GetType			( void ) const		{ return m_Type; }		// Tipo del steering concreto, se establece en el constructor

	//---- Register Methods -------------------------------------
	
	//---- Members -----------------------------------------------
private:
		Vect2f				m_Target;
		bool				m_EvaderUpdated;
		CArrive			*	m_pArrive;
        CSteeringEntity *	m_pEvader;
};

#endif __CLASS_PURSUIT_H__