//----------------------------------------------------------------------------------
// CPhysicCookingMesh class
// Author: Enric Vergara
//
// Description:
// Clase encargarda de gestionar las mallas triangulares utilizadas para la f�sica
//----------------------------------------------------------------------------------
#pragma once

#ifndef INC_PHYSIC_COOKING_MESH_H_
#define INC_PHYSIC_COOKING_MESH_H_

#include <map>
#include <vector>
#include "Math\Vector3.h"

//---Forward Declarations---
class NxPhysicsSDK;
class CPhysicUserAllocator;
class NxCookingInterface;
class NxTriangleMesh;
//Cloth
class NxStream;
class NxClothMeshDesc;
//--------------------------

using namespace std;

class CPhysicCookingMesh
{
public:
	typedef std::map<std::string,NxTriangleMesh*> VecMeshes;

public:
	//--- Init and End protocols	
	CPhysicCookingMesh();
	~CPhysicCookingMesh()		{ Done(); }

	bool				Init					(NxPhysicsSDK* physicSDK, CPhysicUserAllocator* myAllocator);
	void				Done					();
	bool				IsOk					() const		{ return m_bIsOk; }

	NxTriangleMesh*		GetPhysicMesh			( const std::string& name );

	bool				CreatePhysicMesh		( const std::string& binFilename, const std::string& nameMesh );
	bool				CreatePhysicMesh		( const std::vector<Vect3f>& vertices, const std::vector<uint32>& faces, const std::string &nameMesh );
	bool				SavePhysicMesh			( const std::vector<Vect3f>& vertices, const std::vector<uint32>& faces, const std::string &binFilename );
	bool				ReadMeshFromASE			( std::string fileName, std::vector<Vect3f>&vertices, std::vector<unsigned int>& faces );
	bool				CookClothMesh			( const NxClothMeshDesc& desc, NxStream& stream );

private:
	void				Release					();

private:
	bool							m_bIsOk;
	VecMeshes						m_TriangleMeshes;
	NxPhysicsSDK					*m_pPhysicSDK;
	CPhysicUserAllocator			*m_pMyAllocator;
	NxCookingInterface				*m_pCooking;
};

#endif // INC_PHYSIC_COOKING_MESH_H_