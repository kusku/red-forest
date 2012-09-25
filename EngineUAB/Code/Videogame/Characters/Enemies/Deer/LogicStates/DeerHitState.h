#pragma once

#ifndef _DEER_HIT_STATE_H_
#define _DEER_HIT_STATE_H_

#include <string>
#include "StatesMachine\State.h"
#include "Characters\Character.h"
#include "Callbacks\State\ActionStateCallback.h"

// --- Foward Declarations ---
class CDeer;
class CAnimationCallback;
// ---------------------------

class CDeerHitState : public CState<CCharacter> 
{
public:

	// ------------- Constructors i Destructors --------------
					CDeerHitState	( CCharacter* );
					CDeerHitState	( CCharacter*, const std::string &_Name );
	virtual			~CDeerHitState	( void );

	//----Main Functions --------------------------------------
	virtual void	Execute		( CCharacter*, float _ElapsedTime );

	// Se ejecuta cuando el estado es entrado
	virtual void	OnEnter		( CCharacter* );

	// Se ejecuta cuando el estado sale
	virtual void	OnExit		( CCharacter* );

	// Se ejecuta cuando otro caracter env�a un mensaje a este
	virtual bool	OnMessage	( CCharacter*, const STelegram& );

	//----Methods ---------------------------------------------
	//void			PlayRandomSound( void );
	void			UpdateParticlesPositions( CCharacter* _pCharacter );

	//----Members ---------------------------------------------
private:
	CDeer					  * m_pDeer;
	CAnimationCallback		  *	m_pAnimationCallback;
	CActionStateCallback		m_pActionStateCallback;
	
	float						m_ActionDuration;
	bool						m_IsCommingFromTired;
	float						m_RecoverMinTiredTime;
	float						m_RecoverMaxTiredTime;
	
	float						m_OldMaxSpeed;
	Vect3f						m_HitDirection;			// Direcci�n hacia atr�s donde se dirige en el golpeo
	float						m_MaxHitDistance;		// Distancia m�xima de recorrido del hit
	Vect3f						m_InitialHitPoint;		// Punto final de retroceso
	
};
#endif _DEER_HIT_STATE_H_