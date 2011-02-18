#ifndef VECTOR3_H
#define VECTOR3_H
#include <math.h>

class CVector3
{
public:
    CVector3();
    CVector3(float i_x, float i_y, float i_z);

    float	 	x,y,z;
    CVector3		sub(CVector3 in);
    CVector3		add(CVector3 in);
    CVector3		mul(float in);
    CVector3		div(float in);
    CVector3		cross(CVector3 in);
    float		dot();
    float		dot(CVector3 in);
    float		length();
    void		normalize();
    void		zero();
    bool		isZero();
};
#endif //VECTOR3_H