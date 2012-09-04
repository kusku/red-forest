#include "MathHelper.h"

#if defined _DEBUG
#include "Memory/MemLeaks.h"
#endif

namespace Helper
{
	float AngleFilter(float _angle)
	{
		float l_fAngle	= _angle;
		float l_2PI		= e2PI<float>();

		if( l_fAngle >  l_2PI)
		{
			l_fAngle = l_fAngle - l_2PI;
		}
		else if( l_fAngle < -l_2PI )
		{
			l_fAngle = l_fAngle + l_2PI;
		}

		return l_fAngle;	
	}

} //namespace Helper