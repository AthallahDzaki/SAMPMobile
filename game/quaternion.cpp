#include "main.h"
#include "game.h"

void CQuaternion::SetFromMatrix(MATRIX4X4 mat)
{
	w = sqrt( std::max( (float)0, 1.0f + mat.right.X + mat.up.Y + mat.at.Z ) ) * 0.5f;
	x = sqrt( std::max( (float)0, 1.0f + mat.right.X - mat.up.Y - mat.at.Z ) ) * 0.5f;
	y = sqrt( std::max( (float)0, 1.0f - mat.right.X + mat.up.Y - mat.at.Z ) ) * 0.5f;
	z = sqrt( std::max( (float)0, 1.0f - mat.right.X - mat.up.Y + mat.at.Z ) ) * 0.5f;

	x = static_cast < float > ( copysign( x, mat.at.Y - mat.up.Z ) );
	y = static_cast < float > ( copysign( y, mat.right.Z - mat.at.X ) );
	z = static_cast < float > ( copysign( z, mat.up.X - mat.right.Y ) );
}

void CQuaternion::GetMatrix(PMATRIX4X4 mat)
{
	if(!mat) return;

	float sqw = w * w; // v13 = a1 * a1;
	float sqx = x * x; // v14 = a2 * a2;
	float sqy = y * y; // v15 = a3 * a3;
	float sqz = z * z; // v16 = a4 * a4;

	mat->right.X = ( sqx - sqy - sqz + sqw); 	// a5 = v14 - v15 - v16 + v13;
	mat->up.Y = (-sqx + sqy - sqz + sqw);		// a9 = v15 - v14 - v16 + v13;
	mat->at.Z = (-sqx - sqy + sqz + sqw);		// a13 = v16 - (v15 + v14) + v13;

	float tmp1 = x * y;				// v17 = a2 * a3;
	float tmp2 = z * w;				// v18 = a1 * a4;
	mat->up.X = 2.0 * (tmp1 + tmp2);		// a8 = v18 + v17 + v18 + v17;
	mat->right.Y = 2.0 * (tmp1 - tmp2);		// a6 = v17 - v18 + v17 - v18;

	tmp1 = x*z;						// v20 = a2 * a4;
	tmp2 = y*w;						// v21 = a1 * a3;
	mat->at.X = 2.0 * (tmp1 - tmp2);		// a11 = v20 - v21 + v20 - v21;
	mat->right.Z = 2.0 * (tmp1 + tmp2);		// a7 = v21 + v20 + v21 + v20;
	tmp1 = y*z;						// v22 = a3 * a4;
	tmp2 = x*w;						// v23 = a1 * a2;
	mat->at.Y = 2.0 * (tmp1 + tmp2);		// a12 = v23 + v22 + v23 + v22;
	mat->up.Z = 2.0 * (tmp1 - tmp2);		// a10 = v22 - v23 + v22 - v23;
}

void CQuaternion::SetFromEulerAngles(float yaw, float pitch, float roll)
{
	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);

	w = cy * cp * cr + sy * sp * sr;
	x = cy * cp * sr - sy * sp * cr;
	y = sy * cp * sr + cy * sp * cr;
	z = sy * cp * cr - cy * sp * sr;
}

void CQuaternion::ToEulerAngles(float& roll, float& pitch, float& yaw)
{
	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (w * x + y * z);
	double cosr_cosp = +1.0 - 2.0 * (x * x + y * y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (w * y - z * x);
	if (fabs(sinp) >= 1.0f)
		pitch = copysign(M_PI / 2.0, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (w * z + x * y);
	double cosy_cosp = +1.0 - 2.0 * (y * y + z * z);
	yaw = atan2(siny_cosp, cosy_cosp);
}

void CQuaternion::Normalize()
{
	double n = sqrt(x*x + y*y + z*z + w*w);

	w /= n;
	x /= n;
	y /= n;
	z /= n;
}

#define SLERP_DELTA 0.1
void CQuaternion::Slerp(CQuaternion *pQ1, CQuaternion *pQ2, float t)
{
	CQuaternion *pOut = this;

	if(!pQ1 || !pQ2) return;
	if(t > 1) return;

	float p1[4];
	double omega, cosom, sinom, scale0, scale1;
	cosom = pQ1->x*pQ2->x + pQ1->y*pQ2->y + pQ1->z*pQ2->z + pQ1->w*pQ2->w;

	if(cosom < 0.0)
	{
		cosom = -cosom;
		p1[0] = - pQ2->x;  p1[1] = - pQ2->y;
		p1[2] = - pQ2->z;  p1[3] = - pQ2->w;
	}	
	else
	{
		p1[0] = pQ2->x;    p1[1] = pQ2->y;
		p1[2] = pQ2->z;    p1[3] = pQ2->w;
	}

	if((1.0 - cosom) > SLERP_DELTA)
	{
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
	}
	else
	{
		scale0 = 1.0 - t;
		scale1 = t;
	}
	Set(scale0 * pQ1->x + scale1 * p1[0],
		scale0 * pQ1->y + scale1 * p1[1],
		scale0 * pQ1->z + scale1 * p1[2],
		scale0 * pQ1->w + scale1 * p1[3]);
}