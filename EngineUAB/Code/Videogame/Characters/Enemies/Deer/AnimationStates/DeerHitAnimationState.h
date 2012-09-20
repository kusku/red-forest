#pragma once

#ifndef _DEER_HIT_ANIMATION_STATE_H_
#define _DEER_HIT_ANIMATION_STATE_H_

#include <string>
#include "StatesMachine\State.h"
#include "Characters\Character.h"

// --- Foward Declaractions ---
class CDeer;
// ----------------------------

class CDeerHitAnimationState : public CState<CCharacter> 
{
public:
	// ------------- Constructors i Destructors --------------
					CDeerHitAnimationState	( CCharacter* );
					CDeerHitAnimationState	( CCharacter*, const std::string &_Name );
	virtual			~CDeerHitAnimationState	( void );

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
	inline CDeer * GetCharacter( void )		{ return m_pDeer; }

	//----Members ---------------------------------------------
private:
	CDeer *		m_pDeer;

};


#endif _DEER_IDLE_ANIMATION_STATE_H_