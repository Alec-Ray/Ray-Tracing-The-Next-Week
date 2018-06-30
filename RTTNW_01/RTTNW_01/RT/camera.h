#ifndef cameraH
#define cameraH

//M_PI 是一个宏定义，圆周率的定义，math文件中也有这家伙。
#define M_PI 3.14159265358979323846

#include "RT/ray.h"

vec3 random_in_unit_disk() {
	/*在z=0平面上产生一个“起点在原点，长度小于1，方向随机”的向量。
	为什么是z=0平面，这个和相机的倾斜方向有关。
	（相机的倾斜方向为view up （简称vup，一般设置为（0，1，0）））
	*/
	vec3 p;
	do {
		//p = 2.0*vec3(drand48(),drand48(),0) - vec3(1,1,0);
		p = 2.0*vec3((rand() % (100) / (float)(100)), (rand() % (100) / (float)(100)), 0) - vec3(1, 1, 0);
	} while (dot(p,p) >= 1.0);
	return p;
}

class camera
{
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
	float time0, time1;  //射线开始时间和结束时间

public:
	//构造函数中加入时间t0和t1
	camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist,
		float t0,float t1)
	{
		time0 = t0;
		time1 = t1;
		lens_radius = aperture / 2;
		float theta = vfov*M_PI/180;
		float half_height = tan(theta/2);
		float half_width = aspect * half_height;
		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
		lower_left_corner = origin  - half_width*focus_dist*u -half_height*focus_dist*v - focus_dist*w;
		horizontal = 2*half_width*focus_dist*u;
		vertical = 2*half_height*focus_dist*v;
	}

	//time为一个时间在time0到time1的随机时间
	ray getray(float s, float t) {
		vec3 rd = lens_radius*random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		float time = time0 + (rand() % (100) / (float)(100))*(time1 - time0);
		return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset,time); 
	}

};
#endif




