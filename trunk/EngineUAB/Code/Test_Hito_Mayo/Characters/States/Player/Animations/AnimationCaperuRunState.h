#pragma once

#ifndef __CLASS_ANIMATION_CAPERU_RUN_STATE_H__
#define __CLASS_ANIMATION_CAPERU_RUN_STATE_H__


#include "StatesMachine\State.h"

//--- Foward Declarations ---//
class CCharacter;
//---------------------------//

class CAnimationCaperuRunState : public CState<CCharacter>
{
public:
	// ------------- Constructors i Destructors ----------------
					CAnimationCaperuRunState	( void );
					~CAnimationCaperuRunState	( void );

	// ------------- M�todes Principals ---------------------
	void	Execute		( CCharacter* _pCharacter );

	// Se ejecuta cuando el estado es entrado
	void	OnEnter		( CCharacter* _pCharacter );

	// Se ejecuta cuando el estado sale
	void	OnExit		( CCharacter* _pCharacter );

	bool	OnMessage	( CCharacter* _pCharacter, const Telegram& );

	// ------------- Membres -----------------------------------
};

#endif __CLASS_ANIMATION_CAPERU_RUN_STATE_H__