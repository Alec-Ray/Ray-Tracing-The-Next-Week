#ifndef MOVINGSPHEREH
#define MOVINGSPHEREH

#include "RT/hitable.h"

//物体在移动，在time0时在地点center0，在time1时在地点center1
class moving_sphere: public hitable  {
public:
	moving_sphere() {}
	moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m)
		: center0(cen0), center1(cen1), time0(t0),time1(t1), radius(r), mat_ptr(m)  {};
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	vec3 center(float time) const;
	vec3 center0, center1;
	float time0, time1;
	float radius;
	material *mat_ptr;
};

//获取某一时刻的中心点坐标
vec3 moving_sphere::center(float time) const{
	return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
}

//让所有的普通物体都可以实现动态模糊
// replace "center" with "center(r.time())"
bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - a*c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant))/a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}


#endif
