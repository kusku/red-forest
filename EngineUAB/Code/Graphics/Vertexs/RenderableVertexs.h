#pragma once

#ifndef _RENDERABLEVERTEXS_H
#define _RENDERABLEVERTEXS_H

#include <d3dx9.h>
#include "Base.h"

class CRenderManager;

class CRenderableVertexs
{
public:
	CRenderableVertexs()
		: m_VB(NULL)
		, m_IB(NULL)
		, m_IndexCount(0)
		, m_VertexCount(0)
	{
	}
	virtual ~CRenderableVertexs()
	{
		CHECKED_RELEASE(m_VB);
		CHECKED_RELEASE(m_IB);
	}

	virtual bool		Render(CRenderManager *RM) = 0;

	virtual inline size_t				GetFacesCount () const		{ return m_IndexCount / 3; }
	virtual inline size_t				GetVertexsCount() const		{ return m_VertexCount; }
	virtual inline unsigned short		GetVertexType() const = 0;
	virtual inline size_t				GetVertexSize() = 0;
	virtual inline size_t				GetIndexSize() = 0;



protected:
	LPDIRECT3DVERTEXBUFFER9		m_VB;
	LPDIRECT3DINDEXBUFFER9		m_IB;
	size_t						m_IndexCount, m_VertexCount;
};

#endif
