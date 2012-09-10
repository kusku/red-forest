#include "DirectionalLight.h"
#include "RenderManager.h"
#include "Textures\Texture.h"
#include "Math\Matrix44.h"
#include "XML\XMLTreeNode.h"
#include "Effects\EffectManager.h"
#include "Base.h"
#include "Core.h"
#include "Cameras\Camera.h"
#include "Cameras\Frustum.h"
#include "Math\Vector3.h"
#include "BoundingObjects.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CDirectionalLight::CDirectionalLight( CXMLTreeNode &XMLNode )
{
	m_Type = DIRECTIONAL;
	m_Direction = XMLNode.GetVect3fProperty("dir", Vect3f(0.0f, 0.0f, 0.0f));
	//m_Direction.Normalize();

	m_OrthoShadowMapSize.x = XMLNode.GetIntProperty("directional_light_camara_size_w", 256);
	m_OrthoShadowMapSize.y = XMLNode.GetIntProperty("directional_light_camara_size_h", 256);

	//Extract Common Info
	ExtractCommonLightInfo(XMLNode);
}

CDirectionalLight::CDirectionalLight()
	: m_Direction( Vect3f(0.0f, 0.0f, 0.0f) )
	, m_OrthoShadowMapSize(Vect2i(256, 256))
{
	m_Type = DIRECTIONAL;
}

CDirectionalLight::~CDirectionalLight()
{
}

void CDirectionalLight::Render(CRenderManager *RM)
{
	Mat44f mat, matRotYaw, matRotPitch;
	Vect3f l_Pos = GetPosition();

	mat.SetIdentity();
	matRotYaw.SetIdentity();
	matRotPitch.SetIdentity();

	mat.Translate( l_Pos );
	matRotYaw.RotByAngleZ( GetYaw() );
	matRotPitch.RotByAngleX( GetPitch() );

	mat = mat * matRotYaw * matRotPitch;

	RM->SetTransform( mat );
	RM->DrawTower(0.4f, colYELLOW);

	mat.SetIdentity();
	RM->SetTransform(mat);
	Vect3f l_Dir = l_Pos + (m_Direction * 20.0f);
	RM->DrawLine( l_Pos , l_Dir );

	RM->SetTransform(m44fIDENTITY);

	if(m_LightFrustum != NULL)
	{
		m_LightFrustum->DrawFrustum();
	}

}

void CDirectionalLight::SetShadowMap()
{
	D3DXMATRIX l_View;
	D3DXMATRIX l_Projection;

	CEffectManager* l_EffectManager = CORE->GetEffectManager();

	D3DXVECTOR3 l_Eye = D3DXVECTOR3(m_Position.x, m_Position.y, m_Position.z);

	//static Vect3f temp = (-m_Direction).GetNormalized();
	//m_Direction = -temp;

	//Vect3f lookat = m_Direction.GetNormalized();
	////lookat = m_Direction;
	D3DXVECTOR3 l_LookAt(m_Direction.x, m_Direction.y, m_Direction.z);

	Vect3f vup(0, 1, 0);
	D3DXVECTOR3 l_VUP(vup.x, vup.y, vup.z);

	//Setup Matrix view
	D3DXMatrixLookAtLH( &l_View, &l_Eye, &l_LookAt, &l_VUP);

	//Setup Matrix projection
	D3DXMatrixOrthoLH( &l_Projection, static_cast<float>(m_OrthoShadowMapSize.x), static_cast<float>(m_OrthoShadowMapSize.y), 1.0f, m_EndRangeAttenuation);
	m_ViewShadowMap = Mat44f(l_View);
	m_ProjectionShadowMap= Mat44f(l_Projection);

	m_LightFrustum->Update((m_ProjectionShadowMap * m_ViewShadowMap).GetD3DXMatrix());

	l_EffectManager->SetShadowCameraEye(m_Position);
	l_EffectManager->SetShadowProjectionMatrix(m_ProjectionShadowMap);
	l_EffectManager->SetShadowViewMatrix(m_ViewShadowMap);
}

void CDirectionalLight::CalculateOrientationDebugRender()
{
	Vect2f l_XYCam = Vect2f(0.0f, 1.0f);
	Vect2f l_YZCam = Vect2f(1.0f, 0.0f);

	Vect2f l_DirPlane;
	l_DirPlane.x = m_Direction.x;
	l_DirPlane.y = m_Direction.y;
	float l_Yaw = 0.f;
	if( l_DirPlane.Length() != 0.f)
	{
		if( m_Direction.y > 0.f )
		{
			if( m_Direction.x < 0.f )
				l_Yaw = mathUtils::ACos<float>((l_XYCam * l_DirPlane)/(l_XYCam.Length()*l_DirPlane.Length()));
			else
				l_Yaw = -mathUtils::ACos<float>((l_XYCam * l_DirPlane)/(l_XYCam.Length()*l_DirPlane.Length()));
		}
		else
		{
			if( m_Direction.x < 0.f )
				l_Yaw = mathUtils::ACos<float>((l_XYCam * l_DirPlane)/(l_XYCam.Length()*l_DirPlane.Length()));
			else
				l_Yaw =-mathUtils::ACos<float>((l_XYCam * l_DirPlane)/(l_XYCam.Length()*l_DirPlane.Length()));
		}
	}


	l_DirPlane.x = m_Direction.y;
	l_DirPlane.y = m_Direction.z;
	float l_Pitch = 0.f;
	if( l_DirPlane.Length() != 0.f)
	{
		if( m_Direction.y > 0.f )
		{
			if( m_Direction.z > 0.f )
				l_Pitch = mathUtils::ACos<float>((l_YZCam*l_DirPlane)/(l_YZCam.Length()*l_DirPlane.Length()));
			else
				l_Pitch =-mathUtils::ACos<float>((l_YZCam*l_DirPlane)/(l_YZCam.Length()*l_DirPlane.Length()));
		}
		else
		{
			if( m_Direction.z > 0.f )
				l_Pitch = D3DX_PI-mathUtils::ACos<float>((l_YZCam*l_DirPlane)/(l_YZCam.Length()*l_DirPlane.Length()));
			else
				l_Pitch = -D3DX_PI + mathUtils::ACos<float>((l_YZCam*l_DirPlane)/(l_YZCam.Length()*l_DirPlane.Length()));
		}
	}

	SetYaw(l_Yaw);
	SetPitch(l_Pitch);
}