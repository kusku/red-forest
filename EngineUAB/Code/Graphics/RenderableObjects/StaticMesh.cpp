#include "StaticMesh.h"
#include "..\RenderManager.h"
#include "..\Vertexs\RenderableVertexs.h"
#include "..\Vertexs\IndexedVertexs.h"
#include "..\Textures\Texture.h"
#include "..\Vertexs\VertexType.h"
#include "Base.h"
#include "Core.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CStaticMesh::CStaticMesh()
	: m_NumVertexs(0)
	, m_NumFaces(0)
	, m_FileName("")
{
}

CStaticMesh::~CStaticMesh()
{
	Unload();
}

bool CStaticMesh::Load(const std::string &FileName)
{
	m_FileName = FileName;
	return LoadFile();
}

bool CStaticMesh::Reload()
{
	Unload();
	return LoadFile();
}

void CStaticMesh::Unload()
{
	std::vector<CRenderableVertexs*>::iterator l_It = m_RVs.begin();
	std::vector<CRenderableVertexs*>::iterator l_End = m_RVs.end();

	for(; l_It != l_End; ++l_It)
	{
		CHECKED_DELETE( (*l_It) );
	}

	m_RVs.clear();

	std::vector<std::vector<CTexture*>>::iterator l_ItTex = m_Textures.begin();
	std::vector<std::vector<CTexture*>>::iterator l_EndTex = m_Textures.end();
	for(; l_ItTex != l_EndTex; ++l_ItTex)
	{
		uint16 l_NumTexs = static_cast<uint16>((*l_ItTex).size());
		for(uint16 i = 0; i < l_NumTexs; ++i)
		{
			CHECKED_DELETE( (*l_ItTex)[i] );
		}
		(*l_ItTex).clear();
	}

	m_Textures.clear();
}

bool CStaticMesh::LoadFile()
{
	FILE *l_File = NULL;
	uint16 l_Type;
	uint16 l_ICount;
	uint16 l_VCount;
	std::string l_TexturePath;

	fopen_s( &l_File, m_FileName.c_str(), "rb" );
	if( l_File != NULL )
	{
		uint16 l_Data;
		fread( &l_Data, sizeof(uint16), 1, l_File );
		if( l_Data == 0xCACA ) //Header correcto
		{
			fread( &l_Type, sizeof(uint16), 1, l_File ); //Leer tipo de v�rtice

			fread( &l_Data, sizeof(uint16), 1, l_File ); //Material Path Length

			char *l_Path = new char[l_Data+2];
			fgets(l_Path, l_Data+2, l_File); //Material Path
			l_TexturePath = l_Path;
			CHECKED_DELETE_ARRAY(l_Path);

			if(  l_Type == TNORMALTEXTURE1_VERTEX::GetVertexType() )
			{
				TNORMALTEXTURE1_VERTEX *l_Vertexs = NULL;
				uint16 *l_Indices = NULL;

				fread( &l_VCount, sizeof(uint16), 1, l_File ); //Vertex Count

				l_Vertexs = new TNORMALTEXTURE1_VERTEX[l_VCount]; //Read Vertexs
				fread( l_Vertexs, sizeof(TNORMALTEXTURE1_VERTEX), l_VCount, l_File );

				fread( &l_ICount, sizeof(uint16), 1, l_File ); //Index Count

				l_Indices = new uint16[l_ICount]; //Read Indices
				fread( l_Indices, sizeof(uint16), l_ICount, l_File);

				CRenderableVertexs *l_RV = new CIndexedVertexs<TNORMALTEXTURE1_VERTEX>(CORE->GetRenderManager(), l_Vertexs, l_Indices, l_VCount, l_ICount);
				m_RVs.push_back(l_RV);

				l_RV = NULL;
				CHECKED_DELETE_ARRAY(l_Indices);
				CHECKED_DELETE_ARRAY(l_Vertexs);
			}
			else if( l_Type == TCOLORED_VERTEX::GetVertexType() )
			{
				TCOLORED_VERTEX *l_Vertexs = NULL;
				uint16 *l_Indices = NULL;

				fread( &l_VCount, sizeof(uint16), 1, l_File ); //Vertex Count

				l_Vertexs = new TCOLORED_VERTEX[l_VCount]; //Read Vertexs
				fread( l_Vertexs, sizeof(TCOLORED_VERTEX), l_VCount, l_File );

				fread( &l_ICount, sizeof(uint16), 1, l_File ); //Index Count

				l_Indices = new uint16[l_ICount]; //Read Indices
				fread( l_Indices, sizeof(uint16), l_ICount, l_File);

				CRenderableVertexs *l_RV = new CIndexedVertexs<TCOLORED_VERTEX>(CORE->GetRenderManager(), l_Vertexs, l_Indices, l_VCount, l_ICount);
				m_RVs.push_back(l_RV);

				l_RV = NULL;
				CHECKED_DELETE_ARRAY(l_Indices);
				CHECKED_DELETE_ARRAY(l_Vertexs);
			}


			fread( &l_Data, sizeof(uint16), 1, l_File );
			if( l_Data != 0xACAC ) //Footer
			{
				fclose(l_File);
				return false;
			}
		}
		else
		{
			fclose(l_File);
			return false;
		}

		fclose(l_File);

		CTexture *l_Texture = new CTexture();
		l_Texture->Load(l_TexturePath);
		std::vector<CTexture*> l_Texs;
		l_Texs.push_back(l_Texture);
		m_Textures.push_back(l_Texs);

		l_Texture = NULL;
		return true;
	}

	return false;
}

void CStaticMesh::Render(CRenderManager *RM) const
{
	uint16 l_Size = static_cast<uint16>(m_RVs.size());
	for(uint16 i=0; i<l_Size; ++i)
	{
		uint16 l_NumTexs = static_cast<uint16>(m_Textures[i].size());
		for( uint16 j=0; j < l_NumTexs; ++j)
		{
			m_Textures[i][j]->Activate(0);
		}
		m_RVs[i]->Render(RM);
	}
}