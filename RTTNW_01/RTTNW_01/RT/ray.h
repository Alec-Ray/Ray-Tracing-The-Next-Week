#ifndef RAYH
#define RAYH
#include "RT/vec3.h"

class ray
{
public:
	ray() {}
	//给_time赋值
	ray(const vec3& a, const vec3& b, float ti = 0.0) { A = a; B = b; _time = ti;}  
	vec3 origin() const       { return A; }
	vec3 direction() const    { return B; }

	//存储时间的变量
	float time() const    { return _time; }
	vec3 point_at_parameter(float t) const { return A + t*B; }

	vec3 A;
	vec3 B;
	float _time;
};

#endif 

