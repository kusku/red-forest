#pragma once

#ifndef _RABBIT_DEFENSE_ANIMATION_STATE_H_
#define _RABBIT_DEFENSE_ANIMATION_STATE_H_

#include <string>
#include "StatesMachine\State.h"
#include "Characters\Enemies\Rabbit\Rabbit.h"

class CRabbitDefenseAnimationState : public CState<CCharacter> 
{
public:
	// ------------- Constructors i Destructors --------------
					CRabbitDefenseAnimationState	( void );
					CRabbitDefenseAnimationState	( const std::string &_Name );
	virtual			~CRabbitDefenseAnimationState	( void );

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
	inline CRabbit * GetCharacter( void )		{ return m_pRabbit; }

	//----Members ---------------------------------------------
private:
	CRabbit *		m_pRabbit;

};


#endif _RABBIT_DEFENSE_ANIMATION_STATE_H_