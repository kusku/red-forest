
#include "ParticleSettingsManager.h"
#include "XML\XMLTreeNode.h"
#include "Base.h"
#include "Logger\Logger.h"
#include "Math\Vector4.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CParticleSettingsManager::CParticleSettingsManager()
	: m_Filename("")
{

}

CParticleSettingsManager::~CParticleSettingsManager()
{
	CleanUp();
}

void CParticleSettingsManager::CleanUp()
{
	this->Destroy();
}

bool CParticleSettingsManager::Load( const std::string& filename )
{
	m_Filename = filename;

	return Reload();
}

bool CParticleSettingsManager::Reload()
{
	CXMLTreeNode newFile;
	if (!newFile.LoadFile(m_Filename.c_str()))
	{
		std::string msg_error = "CParticleSettingsManager::Load->Error al intentar leer el archivo xml: " + m_Filename;
		LOGGER->AddNewLog(ELL_ERROR, msg_error.c_str());
		return false;
	}

	CXMLTreeNode l_xml = newFile["particle_settings"];
	if( l_xml.Exists() )
	{
		uint16 l_Count = l_xml.GetNumChildren();

		for(uint16 i=0; i<l_Count; ++i)
		{
			std::string l_Type = l_xml(i).GetName();

			if( l_Type == "settings" )
			{
				CXMLTreeNode child = l_xml(i);

				TParticleSystemSettings* settings = new TParticleSystemSettings();

				settings->m_Name = child.GetPszProperty("name", "", true);

				uint16 l_CountChild = child.GetNumChildren();
				for(uint16 j = 0; j < l_CountChild; ++j)
				{
					std::string l_TypeChild = child(j).GetName();

					if(l_TypeChild == "TextureName")
					{
						settings->m_TextureName = child(j).GetPszProperty("value", "", true);
					}
					else if(l_TypeChild == "MaxParticles")
					{
						settings->m_MaxParticles = (uint32)child(j).GetIntProperty("value", 0, true);
					}
					else if(l_TypeChild == "Duration")
					{
						settings->m_Duration = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "DurationRandomness")
					{
						settings->m_DurationRandomness = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "EmitterVelocitySensitivity")
					{
						settings->m_EmitterVelocitySensitivity = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MinHorizontalVelocity")
					{
						settings->m_MinHorizontalVelocity = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MaxHorizontalVelocity")
					{
						settings->m_MaxHorizontalVelocity = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MinVerticalVelocity")
					{
						settings->m_MinVerticalVelocity = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MaxVerticalVelocity")
					{
						settings->m_MaxVerticalVelocity = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "Gravity")
					{
						settings->m_Gravity = child(j).GetVect3fProperty("value", Vect3f(0, 0, 0), true);
					}
					else if(l_TypeChild == "EndVelocity")
					{
						settings->m_EndVelocity = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MinColor")
					{
						Vect4f temp = child(j).GetVect4fProperty("value", Vect4f(0, 0, 0, 0), true) / 255.0f;
						settings->m_MinColor = CColor(temp);
					}
					else if(l_TypeChild == "MaxColor")
					{
						Vect4f temp = child(j).GetVect4fProperty("value", Vect4f(0, 0, 0, 0), true) / 255.0f;
						settings->m_MaxColor = CColor(temp);
					}
					else if(l_TypeChild == "MinRotateSpeed")
					{
						settings->m_MinRotateSpeed = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MaxRotateSpeed")
					{
						settings->m_MaxRotateSpeed = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MinStartSize")
					{
						settings->m_MinStartSize = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MaxStartSize")
					{
						settings->m_MaxStartSize = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MinEndSize")
					{
						settings->m_MinEndSize = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "MaxEndSize")
					{
						settings->m_MaxEndSize = child(j).GetFloatProperty("value", 0, true);
					}
					else if(l_TypeChild == "BlendState")
					{
						std::string type = child(j).GetPszProperty("value", 0, true);

						if(type.compare("NonPremultiplied") == 0)
						{
							settings->m_BlendState = TGraphicBlendStates::NonPremultiplied;
						}
						else if (type.compare("Additive") == 0)
						{
							settings->m_BlendState = TGraphicBlendStates::Additive;
						} 
						else if (type.compare("DefaultState") == 0)
						{
							settings->m_BlendState = TGraphicBlendStates::DefaultState;
						}
					}
				}

				bool isOk = this->AddResource(settings->m_Name, settings);
				assert(isOk);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

