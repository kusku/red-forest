#include "DirectionalLight.h"
#include "RenderManager.h"
#include "Textures\Texture.h"
#include "Math\Matrix44.h"
#include "XML\XMLTreeNode.h"
#include "Effects\EffectManager.h"
#include "Base.h"
#include "Core.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CDirectionalLight::CDirectionalLight( CXMLTreeNode &XMLNode )
{
	m_Type = DIRECTIONAL;
	m_Name = XMLNode.GetPszProperty("name", "");
	m_Position = XMLNode.GetVect3fProperty("pos", Vect3f(0.0f, 0.0f, 0.0f));
	m_Direction = XMLNode.GetVect3fProperty("dir", Vect3f(0.0f, 0.0f, 0.0f));
	m_StartRangeAttenuation = XMLNode.GetFloatProperty("att_start_range", 0.0f);
	m_EndRangeAttenuation = XMLNode.GetFloatProperty("att_end_range", 0.0f);
	m_RenderShadows = XMLNode.GetBoolProperty("render_shadows", false);
	Vect3f l_Color = XMLNode.GetVect3fProperty("color", Vect3f(0.0f, 0.0f, 0.0f));
	m_Color = CColor( l_Color.x, l_Color.y, l_Color.z );

	//Shadow Map Params
	m_GenerateDynamicShadowMap = XMLNode.GetBoolProperty("generate_shadow_map", false, false);
	m_GenerateStaticShadowMap = XMLNode.GetBoolProperty("generate_static_shadow_map", false, false);

	if( m_GenerateDynamicShadowMap )
	{
		std::string l_DynamicType = XMLNode.GetPszProperty("shadow_map_format_type", "");
		uint32 l_WidthDynamicShadowMap = XMLNode.GetIntProperty("shadow_map_width", 512);
		uint32 l_HeightDynamicShadowMap = XMLNode.GetIntProperty("shadow_map_height", 512);

		m_pDynamicShadowMap = new CTexture();
		CTexture::TFormatType l_Format = m_pDynamicShadowMap->GetFormatTypeFromString(l_DynamicType);
		m_pDynamicShadowMap->Create(m_Name, l_WidthDynamicShadowMap, l_HeightDynamicShadowMap, 3, 
			CTexture::DYNAMIC, CTexture::DEFAULT, l_Format);
	}

	if( m_GenerateStaticShadowMap )
	{
		std::string l_StaticType = XMLNode.GetPszProperty("static_shadow_map_format_type", "");
		uint32 l_WidthStaticShadowMap = XMLNode.GetIntProperty("static_shadow_map_width", 512);
		uint32 l_HeightStaticShadowMap = XMLNode.GetIntProperty("static_shadow_map_height", 512);

		m_pStaticShadowMap = new CTexture();
		CTexture::TFormatType l_Format = m_pStaticShadowMap->GetFormatTypeFromString(l_StaticType);
		m_pStaticShadowMap->Create(m_Name, l_WidthStaticShadowMap, l_HeightStaticShadowMap, 3, 
			CTexture::DYNAMIC, CTexture::DEFAULT, l_Format);
	}

}

CDirectionalLight::CDirectionalLight()
	: m_Direction( Vect3f(0.0f, 0.0f, 0.0f) )
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
	RM->DrawTower(0.3f, colYELLOW);

	mat.SetIdentity();
	RM->SetTransform(mat);
	Vect3f l_Dir = l_Pos + m_Direction;
	RM->DrawLine( l_Pos , l_Dir );

}

void CDirectionalLight::SetShadowMap(CRenderManager *RM)
{
	D3DXMATRIX l_View;
	D3DXMATRIX l_Projection;

	CEffectManager* l_EffectManager = CORE->GetEffectManager();

    D3DXVECTOR3 l_Eye = D3DXVECTOR3(m_Position.x, m_Position.y, m_Position.z);

    Vect3f lookat = m_Direction;
    D3DXVECTOR3 l_LookAt(lookat.x, lookat.y, lookat.z);

    Vect3f vup(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 l_VUP(vup.x, vup.y, vup.z);

    //Setup Matrix view
	D3DXMatrixLookAtLH( &l_View, &l_Eye, &l_LookAt, &l_VUP);
	
    //Setup Matrix projection
	D3DXMatrixOrthoLH( &l_Projection, m_OrthoShadowMapSize.x, m_OrthoShadowMapSize.y, 1.0f, m_EndRangeAttenuation);

	m_ViewShadowMap = Mat44f(l_View);
	m_ProjectionShadowMap= Mat44f(l_Projection);

	l_EffectManager->ActivateCamera(m_ViewShadowMap, m_ProjectionShadowMap, m_Position);
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