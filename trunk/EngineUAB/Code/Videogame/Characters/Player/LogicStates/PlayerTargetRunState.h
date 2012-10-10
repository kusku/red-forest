#pragma once

#ifndef _PLAYER_TARGET_RUN_STATE_H_
#define _PLAYER_TARGET_RUN_STATE_H_

#include "StatesMachine\State.h"
#include "Characters\Character.h"
#include <string>

struct STelegram;

// --- Foward Declarations ---
class CGameProcess;
// ---------------------------


class CPlayerTargetRunState : public CState< CCharacter >
{
public:
	CPlayerTargetRunState( CCharacter * _pCharacter, const std::string &_Name );
	~CPlayerTargetRunState();

	void	OnEnter		(  CCharacter* _pCharacter );
	void	Execute		(  CCharacter* _pCharacter, float _fElapsedTime );
	void	OnExit		(  CCharacter* _pCharacter );

	bool	OnMessage	(  CCharacter* _pCharacter,  const STelegram& _Message );

private:
	CCharacter		*m_pTargetEnemy;
	CGameProcess	*m_pProcess;
	float			m_fAccelerationStart;
	float			m_fAccelerationEnd;
	float			m_fMaxVelocityMovement;
	float			m_fCurrentVelocityMovement;

	bool			m_bStartState;
	bool			m_bEndState;
	Vect3f			m_LastDirection;
};

#endif //_PLAYER_TARGET_RUN_STATE_H_