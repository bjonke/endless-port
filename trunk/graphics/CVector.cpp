//-------------------------------------------------------------------------
// Namn: Rickard Skeppström
// E-mail: rickard_skeppstrom@msn.com
//-------------------------------------------------------------------------

#include "CVector.h"
CVector3::CVector3()
{
    zero();
}

CVector3::CVector3(float i_x, float i_y, float i_z)
{
    x = i_x;
    y = i_y;
    z = i_z;
}

void CVector3::zero()
{
    x = y = z = 0.0f;
}

bool CVector3::isZero()
{
    //return (x == y == z == 0);
	return true;
}

CVector3 CVector3::add(CVector3 in)
{
    CVector3 out;
    out.x = x + in.x;
    out.y = y + in.y;
    out.z = z + in.z;
    return out;
}

CVector3 CVector3::sub(CVector3 in)
{
    CVector3 out;
    out.x = x - in.x;
    out.y = y - in.y;
    out.z = z - in.z;
    return out;
}
 
CVector3 CVector3::mul(float in)
{
    CVector3 out;
    out.x = x * in;
    out.y = y * in;
    out.z = z * in;
    return out;
}
 
float CVector3::dot()
{
    float out;
    out = (x*x)+(y*y)+(z*z);
    return out;
}

float CVector3::dot(CVector3 in)
{
    float out;
    out = (x*in.x)+(y*in.y)+(z*in.z);
    return out;
}
 
float CVector3::length()
{
    float out;
    out = sqrt(dot());
    return out;
}
 
void CVector3::normalize()
{
    if(! isZero())
    {
        float len;
		len = 1.0f/length();
        x *= len;
        y *= len;
        z *= len;
    }
}

CVector3 CVector3::cross(CVector3 in)
{
    CVector3 out;
    out.x = y * in.z - z * in.y;
    out.y = z * in.x - x * in.z;
    out.z = x * in.y - y * in.x;
    return out;
}