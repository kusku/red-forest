#include "Object3D.h"

#if defined(_DEBUG)
#include "Memory\MemLeaks.h"
#endif

CObject3D::CObject3D(const Vect3f& pos, float yaw, float pitch, float roll)
: m_Position(pos)
, m_fYaw(yaw)
, m_fPitch(pitch)
, m_fRoll(roll)
{

}

CObject3D::CObject3D()
: m_Position( Vect3f(0.f,0.f,0.f))
, m_fYaw(0.f)
, m_fPitch(0.f)
, m_fRoll(0.f)
{
}