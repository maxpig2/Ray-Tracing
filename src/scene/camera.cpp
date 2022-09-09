
#include <iostream>

// glm
#include <glm/gtc/matrix_transform.hpp>

// project
#include "camera.hpp"
#include "opengl.hpp"


using namespace std;
using namespace glm;


void Camera::setPositionOrientation(const vec3 &pos, float yaw, float pitch) {
	m_position = pos;
	m_yaw = yaw;
	m_pitch = pitch;

	// update rotation matrix (based on yaw and pitch)
	m_rotation = rotate(mat4(1), m_yaw, vec3(0, 1, 0)) * rotate(mat4(1), m_pitch, vec3(1, 0, 0));
}


Ray Camera::generateRay(const vec2 &pixel) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Generate a ray in the scene using the camera position,
	// rotation field of view on the y axis (fovy) and the image
	// size. The pixel is given in image coordinates [0, imagesize]
	// This COULD be done by first creating the ray in ViewSpace
	// then transforming it by the position and rotation to get
	// it into worldspace.
	//-------------------------------------------------------------
	
	Ray ray;

	// YOUR CODE GOES HERE
	// ...
	
	//float Ppy = tan(m_fovy/2);
	//float Ppx = ((Ppy * m_image_size.x) / m_image_size.y);

	//Pi = PL + (i + 0.5) / N ×(PR – PL)
	//vec2 point = vec2(2,-2) * pixel + vec2(1,1);

	//float Piy = (-Ppy + ((pixel.y) / m_image_size.y)) * (2 * Ppy);
	//float Pix = (-Ppx + ((pixel.x) / m_image_size.x)) * (2 * Ppx);

	//ray.direction = normalize(vec3(Pix,Piy,-1));
	//ray.direction = vec4(ray.direction,0) * (m_rotation);
	//
	//ray.origin = this->m_position;                                     

	vec2 Ndc = (pixel / m_image_size) * vec2(2) - vec2(1);   
	m_fovy = atan(2);
	float Ppy = tan(m_fovy / 2);                                    
	float Ppx = Ppy * (m_image_size.x / m_image_size.y);   

	m_fovy = atan(4);
	Ppx = tan(m_fovy / 2);
	Ppy = Ppx * (m_image_size.y / m_image_size.x);

	vec3 cameraRay = normalize(vec3(Ndc * vec2(Ppx, Ppy), -1));   

	ray.direction = (m_rotation) *vec4(cameraRay, 0);
	ray.origin = m_position;

	mat4 cameraView = glm::lookAt(vec3(2,2,3), vec3(1, 2, 5), vec3(0, 1, 0));

	vec3 c = vec3(1, 2, 5);
	vec3 e = vec3(2, 2, 3);
	vec3 up = vec3(0, 1, 0);

	vec3 b3 = -(c - e);
	vec3 b1 = cross(up , b3);
	vec3 b2 = cross(b3 , b1);

	//cameraView

	//cout << "B1 X: " << b1.x << " B1 Y: " << b1.y << " B1 Z: " << b1.z << endl;
	//cout << "B2 X: " << b2.x << " B2 Y: " << b2.y << " B2 Z: " << b2.z << endl;
//	cout << "B3 X: " << b3.x << " B3 Y: " << b3.y << " B3 Z: " << b3.z << endl;
	return ray;
}
