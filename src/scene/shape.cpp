 
// std
#include <algorithm>
#include <utility>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// project
#include "shape.hpp"

#include <iostream>

using namespace glm;

using namespace std;

RayIntersection AABB::intersect(const Ray &ray) {
	RayIntersection intersect;
	vec3 rel_origin = ray.origin - m_center;

	// start magic
	// x
	float rdx_inv = 1 / ray.direction.x;
	float tx1 = (-m_halfsize.x - rel_origin.x) * rdx_inv;
	float tx2 = (m_halfsize.x - rel_origin.x) * rdx_inv;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	// y
	float rdy_inv = 1 / ray.direction.y;
	float ty1 = (-m_halfsize.y - rel_origin.y) * rdy_inv;
	float ty2 = (m_halfsize.y - rel_origin.y) * rdy_inv;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	// z
	float rdz_inv = 1 / ray.direction.z;
	float tz1 = (-m_halfsize.z - rel_origin.z) * rdz_inv;
	float tz2 = (m_halfsize.z - rel_origin.z) * rdz_inv;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	if (tmax < tmin) return intersect;
	// end magic

	intersect.m_distance = tmin < 0 ? tmax : tmin;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = tmax >= 0;
	vec3 work_out_a_name_for_it_later = abs((intersect.m_position - m_center) / m_halfsize);
	float max_v = std::max(work_out_a_name_for_it_later[0], std::max(work_out_a_name_for_it_later[1], work_out_a_name_for_it_later[2]));
	intersect.m_normal = normalize(mix(intersect.m_position - m_center, vec3(0), lessThan(work_out_a_name_for_it_later, vec3(max_v))));
	intersect.m_uv_coord = (abs(intersect.m_normal.x) > 0) ?
		vec2(intersect.m_position.y, intersect.m_position.z) :
		vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z);
	intersect.m_shape = this;

	return intersect;
}


RayIntersection Sphere::intersect(const Ray &ray) {
	RayIntersection intersect;
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement the intersection method for Sphere that returns
	// a RayIntersection object with valid == false if the 
	// ray doesn't intersect and true otherwise. If true, then
	// remember to fill out each feild in the object correctly:
	// - m_valid : true if object is itersected
	// - m_distance : distance along the ray of the intersection
	// - m_position : position on the surface of the intersection
	// - m_normal : normal on the surface of the intersection
	// - m_shape : set to "this"
	// - m_uv_coord : texture coordinates (challenge only)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	float minimumT = 1000000;
	vec3 d = normalize(ray.direction);
	float r = m_radius;
	float a = length(d);
	vec3 oc = ray.origin - m_center;
	float b = 2.0 * dot(oc,d);
	float c = (length(oc) * length(oc)) - (r * r);
	float bac = b * b - 4.0f * a * c;
	if (bac >= 0) {
		bac = sqrtf((bac));
		float t1 = (-b + bac) / (2.0f * a);
		float t2 = (-b - bac) / (2.0f * a);
		if (t2 >= 0 && t1 >= 0) {
			if (t1 <= minimumT) {
				minimumT = t1;
				//indexOfClosestSphere = i;
			}
			if (t2 <= minimumT) {
				minimumT = t2;
				//indexOfClosestSphere = i;
			}
		}
	}

	



	if (minimumT == 1000000) {
		intersect.m_valid = false;
		//printf("Intersect sphere false\n");
		return intersect;
	}
	intersect.m_valid = true;
	//printf("Intersect sphere true\n");
	intersect.m_position =  minimumT* ray.direction + ray.origin;
	intersect.m_distance = minimumT;
	//intersect.m_normal = vec3(1);

	intersect.m_normal = normalize(intersect.m_position - m_center);
	intersect.m_shape = this;


	float theta = acos(intersect.m_normal.z);
	float gamma = atan(intersect.m_normal.y/ intersect.m_normal.x);
	if (intersect.m_normal.x < 0) {
		gamma += 3.1415926;
	}

	intersect.m_uv_coord = vec2(theta/(2*3.1415926), gamma / (2 * 3.1415926));


	vec3 dD = normalize(ray.direction) - normalize(vec3(-7, -1, -1));
	if (abs(dD.x) < 0.0001 && abs(dD.y) < 0.0001 && abs(dD.z) < 0.0001) {
		//cout << "X" << intersect.m_position.x << "Y" << intersect.m_position.y << "Z" << intersect.m_position.z << endl;
		vec3 reflectionDirection;
		reflectionDirection = reflect(ray.direction, intersect.m_normal);
		//cout << "NX" << intersect.m_normal.x << "RRY" << intersect.m_normal.y << "RRZ" << intersect.m_normal.z << endl;
	}

	
	return intersect;
}

RayIntersection Plane::intersect(const Ray& ray) {
	RayIntersection intersect;
	float d = dot(ray.direction, m_normal);
	if (abs(d) < 0.01) {
		intersect.m_valid = false;
		return intersect;
	} 
	if (d > 0) {
		intersect.m_normal = -m_normal;
	}
	else intersect.m_normal = m_normal;

	intersect.m_valid = true;
	intersect.m_shape = this;

	float t = dot(m_point - ray.origin , m_normal) / d;
	intersect.m_position = ray.origin + ray.direction * t;
	intersect.m_distance = t;
	if (t < 0) {
		intersect.m_valid = false;
	}

	vec3 e1 = normalize(cross(intersect.m_normal, vec3(1,0,0)));
	vec3 e2 = normalize(cross(intersect.m_normal, e1));
	intersect.m_uv_coord = vec2(dot(e1,intersect.m_position) , dot(e2, intersect.m_position)     );
	intersect.m_uv_coord /= vec2(20);
	return intersect;
}


RayIntersection Disc::intersect(const Ray& ray) {
	RayIntersection intersect;
	float d = dot(ray.direction, m_normal);
	if (abs(d) < 0.01) {
		intersect.m_valid = false;
		return intersect;
	}
	if (d > 0) {
		intersect.m_normal = -m_normal;
	}
	else intersect.m_normal = m_normal;

	intersect.m_valid = true;
	intersect.m_shape = this;

	float t = dot(m_point - ray.origin, m_normal) / d;
	intersect.m_position = ray.origin + ray.direction * t;
	intersect.m_distance = t;
	if (t < 0) {
		intersect.m_valid = false;
	}

	if (length(intersect.m_position -  m_point) > radius) {
		intersect.m_valid = false;
	}


	vec3 e1 = normalize(cross(intersect.m_normal, vec3(1, 0, 0)));
	vec3 e2 = normalize(cross(intersect.m_normal, e1));
	intersect.m_uv_coord = vec2(dot(e1, intersect.m_position), dot(e2, intersect.m_position));
	intersect.m_uv_coord /= vec2(2);
	return intersect;
}


RayIntersection Triangle::intersect(const Ray& ray) {
	RayIntersection intersect;
	float d = dot(ray.direction, m_normal);
	if (abs(d) < 0.01) {
		intersect.m_valid = false;
		return intersect;
	}
	if (d > 0) {
		intersect.m_normal = -m_normal;
	}
	else intersect.m_normal = m_normal;

	intersect.m_valid = true;
	intersect.m_shape = this;

	float t = dot(m_point1 - ray.origin, m_normal) / d;
	if (t < 0) {
		intersect.m_valid = false;
	}



	intersect.m_distance = t;
	intersect.m_position = ray.origin + ray.direction * t;
	vec3 f1 = cross(m_point2 - m_point1, intersect.m_position - m_point1);
	vec3 f2 = cross(m_point3 - m_point2, intersect.m_position - m_point2);
	vec3 f3 = cross(m_point1 - m_point3, intersect.m_position - m_point3);





	if (dot(m_normal, f1) <= 0 || dot(m_normal, f2) <= 0 || dot(m_normal, f3) <= 0) {
		intersect.m_valid = false;
	}


	vec3 e1 = normalize(cross(intersect.m_normal, vec3(1, 0, 0)));
	vec3 e2 = normalize(cross(intersect.m_normal, e1));
	intersect.m_uv_coord = vec2(dot(e1, intersect.m_position), dot(e2, intersect.m_position));
	intersect.m_uv_coord /= vec2(1);
	vec3 dD = normalize(ray.direction) - normalize(vec3(-1, 2, 1));
	if ( abs(dD.x) < 0.0002 && abs(dD.y) < 0.0002 && abs(dD.z) < 0.0002  ) {
	//	cout << "X" << intersect.m_position.x << "Y" << intersect.m_position.y << "Z" << intersect.m_position.z << endl;
	
	}
	cout << "X" << intersect.m_normal.x << "Y" << intersect.m_normal.y << "Z" << intersect.m_normal.z << endl;


	return intersect;
}