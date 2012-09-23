#pragma once

#ifndef _RABBIT_DEFENSE_STATE_H_
#define _RABBIT_DEFENSE_STATE_H_


#include <string>
#include "StatesMachine\State.h"
#include "Callbacks\State\ActionStateCallback.h"
#include "Characters\Character.h"

// --- Foward Declarations ---
class CRabbit;
// ---------------------------

class CRabbitDefenseState : public CState<CCharacter> 
{
public:

	// ------------- Constructors i Destructors --------------
					CRabbitDefenseState	( CCharacter* );
					CRabbitDefenseState	( CCharacter* _pCharacter, const std::string &_Name );
	virtual			~CRabbitDefenseState( void );

	//----Main Functions --------------------------------------
	virtual void	Execute		( CCharacter*, float _ElapsedTime );

	// Se ejecuta cuando el estado es entrado
	virtual void	OnEnter		( CCharacter* );

	// Se ejecuta cuando el estado sale
	virtual void	OnExit		( CCharacter* );

	// Se ejecuta cuando otro caracter env�a un mensaje a este
	virtual bool	OnMessage	( CCharacter*, const STelegram& );

	//----Methods ---------------------------------------------
	
	//----Properties ------------------------------------------
	const bool		IsHitBlocked	( void )		{ return m_HitBlocked; }

	//----Members ---------------------------------------------
private:
	CActionStateCallback		m_ActionTime;
	CRabbit					  * m_pRabbit;

	bool						m_HitBlocked;			// Me dice si bloqueo
	float						m_HitDistance;			// Me dice la distancia que recorro cuando paga y bloqueo hacia atras
	int							m_TotalHitBlocked;		// Me dice el total de bloqueos que har� hasta que me pueda volver a golpear
	int							m_HitBlockedCount;		// Me dice el n� de veces que el player me pega mientras bloqueo

	float						m_OldMaxSpeed;			// Permite almacenar la vieja velocidad para posteriormente recuperarla
	float						m_OldMass;				// Permite almacenar la vieja masa para posteriormente recuperarla
};
#endif _RABBIT_DEFENSE_STATE_H_