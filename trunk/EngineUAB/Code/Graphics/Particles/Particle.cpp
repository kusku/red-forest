#include "Particle.h"
#include "Cameras\Camera.h"

#if defined (_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CParticle::CParticle()
	: m_fLifeTime(1.f)
	, m_fAge(0.f)
	, m_Direction(Vect3f(0.f, 1.f, 0.f))
{
}

CParticle::CParticle( float lifeTime, const Vect3f& direction)
	: m_fLifeTime(lifeTime)
	, m_fAge(0.f)
	, m_Direction(direction)
{
}

CParticle::~CParticle()
{
}

bool CParticle::Update(float elapsedTime, CCamera &camera)
{
	m_fAge += elapsedTime;
	if( m_fAge >= m_fLifeTime )
	{
		return false;
	}

	m_vPosition = m_vPosition + m_Direction * elapsedTime;

	CBillboard::Update(camera);

	return true;
}