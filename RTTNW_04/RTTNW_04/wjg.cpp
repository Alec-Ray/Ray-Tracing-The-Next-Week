#include <iostream>
#include <fstream>
#include <random>
#include "RT/sphere.h"
#include "RT/hitable_list.h"
#include "RT/camera.h"
#include "RT/material.h"
#include "RT/moving_sphere.h"

using namespace std;

vec3 Color(const ray& r, hitable *world, int depth)
{
	//这个“rec”会在sphere::hit ()中带上来被撞击球的材料属性（指向一个材质对象的指针mat_ptr）。
	//根据这个指针可以获取材料对象的成员方法scatter()和成员变量albedo（反射衰减向量）
	hit_record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			//获取反射光线向量scattered和反射衰减向量attenuation
			return attenuation*Color(scattered, world, depth + 1);
			//反射光线的强度需要乘以反射衰减向量（对应坐标相乘作为新的向量）。
			//然后反射光线就扮演之前“原始光线”的角色。如果再次撞击到小球，就再次反射，直到不再撞击到任何球为止
		}
		else {
			return vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		//绘制背景
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f*(unit_direction.y() + 1.0f);

		//线性混合，t=1时蓝色，t=0时白色，t介于中间时是混合颜色
		//blended_value = (1-t)*start_value + t*end_value
		return (1.0f - t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);

		//注意这里，原始光线和反射光线最后都会跑到这里来。
		//背景的颜色：原始光线的方向向量的映射
		//漫反射材料和镜面材料的颜色：最后一次反射光线的方向向量的映射 *  所有反射衰减系数的乘积。
		//漫反射和镜面反射的区别在于，漫反射的每次反射方向是随机的
	}
}

hitable *RandomScene() {
	texture *pertext = new noise_texture(3);
	hitable **list = new hitable*[2];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));

	return new hitable_list(list, 2);

}

int main()
{
	ofstream outfile;
	outfile.open("IMG07.ppm");

	int nx = 800;
	int ny = 400;
	//采样次数
	int ns = 100;
	outfile << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = RandomScene();

	vec3 lookform(13.0f, 2.0f, 3.0f);
	vec3 lookat(0, 0, 0);
	float dist_to_focus =  (lookform - lookat).length();
	float aperture = 0.0f;

	camera cam(lookform, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, 0.7*dist_to_focus,0.0,1.0);

	//随机数
	default_random_engine reng;
	uniform_real_distribution<float> uni_dist(0.0f, 1.0f);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0.0f, 0.0f, 0.0f);
			//每个区域采样ns次
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + uni_dist(reng)) / float(nx);
				float v = float(j + uni_dist(reng)) / float(ny);
				ray r = cam.getray(u,v);
				//vec3 p = r.point_at_parameter(2.0);
				//将本区域（(u,v)到(u+1,v+1)）的颜色值累加
				col += Color(r, world, 0);
			}
			//获得区域的颜色均值
			col /= float(ns);
			//gamma矫正
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			outfile << ir << " " << ig << " " << ib << "\n";
		}
	}
	outfile.close();
	return 0;
}