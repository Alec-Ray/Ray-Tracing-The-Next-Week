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
	//�����rec������sphere::hit ()�д�������ײ����Ĳ������ԣ�ָ��һ�����ʶ����ָ��mat_ptr����
	//�������ָ����Ի�ȡ���϶���ĳ�Ա����scatter()�ͳ�Ա����albedo������˥��������
	hit_record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			//��ȡ�����������scattered�ͷ���˥������attenuation
			return attenuation*Color(scattered, world, depth + 1);
			//������ߵ�ǿ����Ҫ���Է���˥����������Ӧ���������Ϊ�µ���������
			//Ȼ������߾Ͱ���֮ǰ��ԭʼ���ߡ��Ľ�ɫ������ٴ�ײ����С�򣬾��ٴη��䣬ֱ������ײ�����κ���Ϊֹ
		}
		else {
			return vec3(0.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		//���Ʊ���
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f*(unit_direction.y() + 1.0f);

		//���Ի�ϣ�t=1ʱ��ɫ��t=0ʱ��ɫ��t�����м�ʱ�ǻ����ɫ
		//blended_value = (1-t)*start_value + t*end_value
		return (1.0f - t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);

		//ע�����ԭʼ���ߺͷ��������󶼻��ܵ���������
		//��������ɫ��ԭʼ���ߵķ���������ӳ��
		//��������Ϻ;�����ϵ���ɫ�����һ�η�����ߵķ���������ӳ�� *  ���з���˥��ϵ���ĳ˻���
		//������;��淴����������ڣ��������ÿ�η��䷽���������
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
	//��������
	int ns = 100;
	outfile << "P3\n" << nx << " " << ny << "\n255\n";

	hitable *world = RandomScene();

	vec3 lookform(13.0f, 2.0f, 3.0f);
	vec3 lookat(0, 0, 0);
	float dist_to_focus =  (lookform - lookat).length();
	float aperture = 0.0f;

	camera cam(lookform, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, 0.7*dist_to_focus,0.0,1.0);

	//�����
	default_random_engine reng;
	uniform_real_distribution<float> uni_dist(0.0f, 1.0f);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0.0f, 0.0f, 0.0f);
			//ÿ���������ns��
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + uni_dist(reng)) / float(nx);
				float v = float(j + uni_dist(reng)) / float(ny);
				ray r = cam.getray(u,v);
				//vec3 p = r.point_at_parameter(2.0);
				//��������(u,v)��(u+1,v+1)������ɫֵ�ۼ�
				col += Color(r, world, 0);
			}
			//����������ɫ��ֵ
			col /= float(ns);
			//gamma����
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