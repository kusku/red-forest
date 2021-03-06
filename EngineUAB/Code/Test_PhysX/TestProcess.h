#pragma once

#ifndef _TESTPROCESS_H
#define _TESTPROCESS_H

class CCamera;
class CThPSCamera;
class CPhysicActor;
class CTrigger;
class CPhysicController;
class CActionToInput;

#include "Player.h"
#include "EngineProcess.h"
#include "Math\Vector2.h"
#include <vector>

class CTestProcess : public CEngineProcess
{
public:
	CTestProcess();
	virtual ~CTestProcess();

	bool Init	( void );
	void Done	( void );

	void Update (float elapsedTime);
	void Render (CRenderManager &RM);

protected:
	virtual void		Release				( void );

	void UpdateInputs( float elapsedTime );
	void UpdateDebugInputs( float elapsedTime, CActionToInput &action2Input );
	void CreateSphereActor();

	Vect2i m_Posicion;
	Vect2i m_Screen;
	Vect3f m_PosAnchor;
	Vect3f m_PosActor;

	CPlayer m_Player;
	CThPSCamera *m_pThPSCamera;

	CPhysicActor *m_pSphere;
	CPhysicActor *m_pPlane;
	std::vector<CPhysicActor*> m_Actors;

	CTrigger	*m_pTrigger;
	CPhysicController *m_Controller;
};

#endif
