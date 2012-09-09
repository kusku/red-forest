#include "InstanceMeshHW.h"
#include "StaticMesh.h"
#include "StaticMeshManager.h"
#include "RenderManager.h"
#include "Base.h"
#include "Core.h"
#include "Logger\Logger.h"
#include "Math\Matrix44.h"
#include "Effects\EffectTechnique.h"
#include "Effects\EffectManager.h"
#include "XML\XMLTreeNode.h"
#include "RenderableObjects\RenderableObjectTechnique.h"
#include "Vertexs\VertexType.h"
#include "Object3D.h"
#include "Lights\LightManager.h"

////----PhysX Includes-------------
#undef min
#undef max
/*#include "NxPhysics.h"
#include "NxControllerManager.h"
#include "NxCapsuleController.h"
#include "NxActor.h"*/
#include "PhysicsManager.h"
#include "PhysicActor.h"
#include "PhysicTriggerReport.h"
#include "PhysicCookingMesh.h"
#include "PhysicUserData.h"
////--------------------------------

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CInstanceMeshHW::CInstanceMeshHW(const std::string& coreName)
	: m_InstanceBuffer(NULL)
	, m_InstanceVertex(NULL)
	, m_Initialize(false)
	, m_VisibleObjects(0)
{
	m_StaticMesh = CORE->GetStaticMeshManager()->GetResource(coreName);

	assert(m_StaticMesh);
}

CInstanceMeshHW::~CInstanceMeshHW()
{
	m_StaticMesh = NULL;

	ObjectMapIt it = m_ObjectMap.begin();
	ObjectMapIt itEnd = m_ObjectMap.end();

	for (; it != itEnd; ++it)
	{
		CHECKED_DELETE(it->second);
	}

	m_ObjectMap.clear();

	CHECKED_RELEASE(m_InstanceBuffer);

	CHECKED_DELETE_ARRAY(m_InstanceVertex);
}

bool CInstanceMeshHW::AddHWInstance(CXMLTreeNode &Node)
{
	CObject3D* instance = new CObject3D();

	std::string	name		= Node.GetPszProperty("name", "");
	Vect3f		position	= Node.GetVect3fProperty("position", Vect3f(0.0f, 0.0f, 0.0f));
	float		yaw			= Node.GetFloatProperty("yaw", 0.0f);
	float		pitch		= Node.GetFloatProperty("pitch", 0.0f);
	float		roll		= Node.GetFloatProperty("roll", 0.0f);
	Vect3f		scale		= Node.GetVect3fProperty("scale", Vect3f(1.0f, 1.0f, 1.0f));
	bool		visible		= Node.GetBoolProperty("visible", true, false);

	bool		addPhyX		= Node.GetBoolProperty("create_physics", true, false);
	std::string	typePhyX	= Node.GetPszProperty("physics_type", "", addPhyX);

	instance->SetName(name);
	instance->SetPosition(position);
	instance->SetYaw(yaw);
	instance->SetPitch(pitch);
	instance->SetRoll(roll);
	instance->SetScale(scale);
	instance->SetVisible(visible);

	if(m_ObjectMap.find(name) != m_ObjectMap.end())
	{
		assert(!"HW Instance Repetido 2 veces");
		
		CHECKED_DELETE(instance);

		return false;
	}

	m_ObjectMap[name] = instance;

	if(addPhyX)
	{
		CrearPhysic(name, typePhyX, position);
	}

	m_Initialize = false;

	return true;
}

bool CInstanceMeshHW::BuildInstanceBuffer()
{
	m_Initialize = false;

	CHECKED_RELEASE(m_InstanceBuffer);
	CHECKED_DELETE_ARRAY(m_InstanceVertex);

	uint32 size = (uint32)m_ObjectMap.size();
	void* data = NULL;
	
	m_InstanceVertex = new TINSTANCE_VERTEX[size];
	memset(m_InstanceVertex, 0, sizeof(TINSTANCE_VERTEX) * size);

	LPDIRECT3DDEVICE9 Device = CORE->GetRenderManager()->GetDevice();

	UINT sizeBuffer = sizeof(TINSTANCE_VERTEX) * size;
	DWORD usageVB = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;

	HRESULT hr = Device->CreateVertexBuffer(sizeBuffer, usageVB, NULL, D3DPOOL_DEFAULT, &m_InstanceBuffer, NULL);

	if(hr != D3D_OK)
	{
		CHECKED_DELETE_ARRAY(m_InstanceVertex);
		return false;
	}
	
	m_InstanceBuffer->Lock(0, sizeBuffer, &data, 0);
	memcpy(data, m_InstanceVertex, sizeBuffer);
	m_InstanceBuffer->Unlock();

	m_Initialize = true;

	return true;
}

void CInstanceMeshHW::UpdateBuffer()
{
	if(!m_Initialize)
	{
		BuildInstanceBuffer();
	}

	m_VisibleObjects = 0;
	ObjectMapIt it = m_ObjectMap.begin();
	ObjectMapIt itEnd = m_ObjectMap.end();

	for (uint32 i = 0; it != itEnd; ++it)
	{
		CObject3D* instance = it->second;

		if(instance->GetVisible())
		{
			bool draw = false;
			D3DXMATRIX WorldMatrix = instance->GetTransform().GetD3DXMatrix();
			CFrustum* frus = NULL;

			if(CORE->IsDrawingShadows())
			{
				frus = CORE->GetLightManager()->GetCurrentFrustum();

				if(frus == NULL)
				{
					draw = true;
				}
			}
			else
			{
				frus = CORE->GetRenderManager()->GetFrustum();
			}

			if(frus != NULL)
			{
				TBoundingBox bb = m_StaticMesh->GetBoundingBox();

				D3DXVECTOR3 minTrans(bb.m_MinPos.x, bb.m_MinPos.y, bb.m_MinPos.z);
				D3DXVECTOR3 maxTrans(bb.m_MaxPos.x, bb.m_MaxPos.y, bb.m_MaxPos.z);

				D3DXVECTOR4 minTrans4, maxTrans4;

				D3DXVec3Transform(&minTrans4, &minTrans, &WorldMatrix);
				D3DXVec3Transform(&maxTrans4, &maxTrans, &WorldMatrix);

				minTrans = D3DXVECTOR3(minTrans4.x, minTrans4.y, minTrans4.z);
				maxTrans = D3DXVECTOR3(maxTrans4.x, maxTrans4.y, maxTrans4.z);

				draw = frus->BoxVisible(minTrans, maxTrans);
			}

			if(draw)
			{
				memcpy(&m_InstanceVertex[i], &WorldMatrix, sizeof(TINSTANCE_VERTEX));

				++i;
				++m_VisibleObjects;

				//CORE->GetRenderManager()->DrawAABB(bb, instance->GetTransform());
			}
		}
	}

	if(m_VisibleObjects == 0)
	{
		return;
	}

	void* data = NULL;
	m_InstanceBuffer->Lock(0, sizeof(TINSTANCE_VERTEX) * m_VisibleObjects, &data, D3DLOCK_NOOVERWRITE);
	memcpy(data, m_InstanceVertex, sizeof(TINSTANCE_VERTEX) * m_VisibleObjects);
	m_InstanceBuffer->Unlock();
}

void CInstanceMeshHW::Render(CRenderManager *RM)
{
	if( m_StaticMesh == NULL )
	{
		return;
	}

	if(!m_Initialize)
	{
		BuildInstanceBuffer();
	}

	UpdateBuffer();

	if(m_VisibleObjects == 0)
	{
		return;
	}

	m_StaticMesh->RenderInstance(RM, m_InstanceBuffer, m_VisibleObjects);
}

CObject3D* CInstanceMeshHW::GetInstance(const std::string& name)
{
	if(m_ObjectMap.find(name) == m_ObjectMap.end())
	{
		return NULL;
	}

	return m_ObjectMap[name];
}

/*
void CInstanceMeshHW::CreateASEMesh ( const std::string &_Filename, const std::string &_Name  )
{
	CPhysicUserData* l_pPhysicUserDataASEMesh;
	CPhysicActor*	 l_AseMeshActor;
	
	CPhysicCookingMesh* l_pCM = CORE->GetPhysicsManager()->GetCookingMesh();
	if ( l_pCM->CreateMeshFromASE ( _Filename, _Name ) )
	{
		l_pPhysicUserDataASEMesh = new CPhysicUserData( _Name );
		l_pPhysicUserDataASEMesh->SetPaint (true);
		l_AseMeshActor = new CPhysicActor( l_pPhysicUserDataASEMesh );
		l_AseMeshActor->AddMeshShape ( l_pCM->GetPhysicMesh(_Name), Vect3f (0.f, 0.f, 0.f ) );
		//m_AseMeshActor->CreateBody ( 10.f );
		CORE->GetPhysicsManager()->AddPhysicActor ( l_AseMeshActor );
	}

	l_pCM						= NULL;
	l_pPhysicUserDataASEMesh	= NULL;
	l_AseMeshActor				= NULL;
}*/

// TODO:: Otra manera de meter la f�sica a partir de los ficheros binarios que creamos desde MAX.
void CInstanceMeshHW::CrearPhysic( const std::string& _Name, const std::string& typePhysic, const Vect3f& position )
{
	if(typePhysic == "bounding_box")
	{
		CPhysicUserData* l_pPhysicUserDataMesh = new CPhysicUserData( _Name  );

		CPhysicActor* l_MeshActor = new CPhysicActor(l_pPhysicUserDataMesh);
		l_pPhysicUserDataMesh->SetPaint (true);
		TBoundingBox bb = m_StaticMesh->GetBoundingBox();

		Vect3f size = (bb.m_MaxPos - bb.m_MinPos);
		size = Vect3f(abs(size.x), abs(size.y), abs(size.z));
		size /= 4;
		l_MeshActor->AddBoxSphape(size, position, Vect3f(0, size.y * 1.5f, 0));
		
		//l_MeshActor->CreateBody(1);

		CORE->GetPhysicsManager()->AddPhysicActor(l_MeshActor);
	}
	else if(typePhysic == "triangle_mesh")
	{
		CPhysicUserData* l_pPhysicUserDataMesh;
		CPhysicActor*	 l_MeshActor;

		CPhysicCookingMesh* l_pCM = CORE->GetPhysicsManager()->GetCookingMesh();
		if ( l_pCM->CreatePhysicMesh ( m_StaticMesh->GetVertexBuffer(), m_StaticMesh->GetFacesBuffer(), _Name ) )
		{
			l_pPhysicUserDataMesh = new CPhysicUserData( _Name  );
			l_pPhysicUserDataMesh->SetPaint (true);
			l_MeshActor = new CPhysicActor( l_pPhysicUserDataMesh );
			l_MeshActor->AddMeshShape ( l_pCM->GetPhysicMesh(_Name), position);
			//m_AseMeshActor->CreateBody ( 10.f );
			CORE->GetPhysicsManager()->AddPhysicActor ( l_MeshActor );
		}

		l_pCM					= NULL;
		l_pPhysicUserDataMesh	= NULL;
		l_MeshActor				= NULL;
	}
}