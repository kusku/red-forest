#pragma once

#ifndef _RABBIT_STILL_ATTACK_STATE_H_
#define _RABBIT_STILL_ATTACK_STATE_H_


#include <string>
#include "StatesMachine\State.h"
#include "Characters\Character.h"

// --- Foward Declarations ---
class CRabbit;
class CParticleEmitter;
class CAnimationCallback;
class CActionStateCallback;
// ---------------------------

class CRabbitStillAttackState : public CState<CCharacter> 
{
public:

	// ------------- Constructors i Destructors --------------
					CRabbitStillAttackState	( CCharacter* );
					CRabbitStillAttackState	( CCharacter* _pCharacter, const std::string &_Name );
	virtual			~CRabbitStillAttackState( void );

	//----Main Functions --------------------------------------
	virtual void	Execute		( CCharacter*, float _ElapsedTime );

	// Se ejecuta cuando el estado es entrado
	virtual void	OnEnter		( CCharacter* );

	// Se ejecuta cuando el estado sale
	virtual void	OnExit		( CCharacter* );

	// Se ejecuta cuando otro caracter env�a un mensaje a este
	virtual bool	OnMessage	( CCharacter*, const STelegram& );

	//----Methods ---------------------------------------------
	void			UpdateParticlesPositions( CCharacter* _pCharacter );
	
	// Para tema de part�culas de impacto
	void			GenerateImpact			( CCharacter* _pCharacter );
	void			UpdateImpact			( CCharacter* _pCharacter );

	//----Members ---------------------------------------------
private:
	CRabbit					  * m_pRabbit;
	CAnimationCallback		  *	m_pAnimationCallback;
	CActionStateCallback	  * m_pActionStateCallback;

	bool						m_SoundPlayed1;				// Indica si el sonido ya se inici�
	bool						m_FirstHitDone;				// Indica que ya se ha lanzado el golpeo
	bool						m_FirstHitReached;			// Indica si alcanc� al player con el golpeo
	bool						m_FirstParticlesHitDone;	// Indiaca que ya se ha lanzado las part�culas de impacto

	// Tema Part�culas
	CParticleEmitter	*m_pParticleEmitter;
};


#endif _RABBIT_STILL_ATTACK_STATE_H_