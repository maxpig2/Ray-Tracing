
// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>


// std
#include <random>

// project
#include "scene.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "material.hpp"
#include "path_tracer.hpp"


using namespace std;
using namespace glm;


vec3 SimplePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// simple grey shape shading
		float f = abs(dot(-ray.direction, intersect.m_normal));
		vec3 grey(0.5, 0.5, 0.5);
		return mix(grey / 2.0f, grey, f);
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}


float max(float f1, float f2) {
	if (f1 > f2) {
		return f1;
	}
	else {
		return f2;
	}
}



vec3 CorePathTracer::sampleRay(const Ray &ray, int) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the ambient, diffuse
	// and specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays). You do
	// not need to use the depth argument for this implementation.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {
		// simple grey shape shading
		float f = abs(dot(-ray.direction, intersect.m_normal));
		vec3 diffuse(0.5, 0.5, 0.5);
		vec3 specular(0, 0, 0);
		diffuse = intersect.m_material->diffuse();
		diffuse = mix(diffuse / 2.0f, diffuse, f);

		vec3 lightValue = vec3(0);
		vec3 lightAmbience = vec3(0);
		for ( std::shared_ptr<Light> l : m_scene->lights()) {
			if (!l->occluded(m_scene, intersect.m_position)) {
				float specularStrength = intersect.m_material->shininess();
				vec3 reflectDir = reflect(-l->incidentDirection(intersect.m_position), intersect.m_normal);
				vec3 viewDir = normalize(-ray.direction);

				float spec = pow(max(dot(viewDir, reflectDir), 0), specularStrength);
				specular += spec * intersect.m_material->specular() * l->irradiance(intersect.m_position);

				float iD = abs(dot(l->incidentDirection(intersect.m_position), intersect.m_normal));
				lightValue += l->irradiance(intersect.m_position) * iD;
			}
				lightAmbience += l->ambience();
		}

	





		//specular = intersect.m_material->specular();
		//float shininess = (intersect.m_material->shininess());
		return ((diffuse * lightAmbience) + (diffuse * lightValue) + specular);// / vec3(4);// +(specular * shininess);
	}





	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 CompletionPathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Using the same requirements for the CorePathTracer add in 
	// a recursive element to calculate perfect specular reflection.
	// That is compute the reflection ray off your intersection and
	// sample a ray in that direction, using the result to additionally
	// light your object. To make this more realistic you may weight
	// the incoming light by the (1 - (1/shininess)).
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {
		// simple grey shape shading
		float f = abs(dot(-ray.direction, intersect.m_normal));
		vec3 diffuse(0.5, 0.5, 0.5);
		vec3 specular(0, 0, 0);
		diffuse = intersect.m_material->diffuse();
		diffuse = mix(diffuse / 2.0f, diffuse, f);

		vec3 lightValue = vec3(0);
		vec3 lightAmbience = vec3(0);
		for (std::shared_ptr<Light> l : m_scene->lights()) {
			if (!l->occluded(m_scene, intersect.m_position)) {
				float specularStrength = intersect.m_material->shininess();
				vec3 reflectDir = reflect(-l->incidentDirection(intersect.m_position), intersect.m_normal);
				vec3 viewDir = normalize(-ray.direction);
				float spec = pow(max(dot(viewDir, reflectDir), 0), specularStrength);
				float iD = abs(dot(l->incidentDirection(intersect.m_position), intersect.m_normal));
				specular += spec * intersect.m_material->specular() * l->irradiance(intersect.m_position);
				lightValue += l->irradiance(intersect.m_position) * iD;
			}
			lightAmbience += l->ambience();
		}
		vec3 ir(0);
		if (depth > 0) {
			Ray r;
			r.origin = intersect.m_position;
			r.direction = reflect(ray.direction, intersect.m_normal);
			ir = sampleRay(r, depth - 1);
			


			ir = ir * (float)log(intersect.m_material->shininess());
			float metalyness = intersect.m_material->metallyness();
			if (metalyness == 1) // only metals reflect their colour
				ir = diffuse * ir;
			else
				ir = ir / 20.0f;
		}
		vec3 il = (ir + (diffuse * lightAmbience) + (diffuse * lightValue) + specular);

		return il;
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



vec3 ChallengePathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the diffuse and 
	// specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays).
	// Implement support for textured materials (using a texture
	// for the diffuse portion of the material).
	//
	// EXTRA FOR EXPERTS :
	// Additionally implement indirect diffuse and specular instead
	// of using the ambient lighting term.
	// The diffuse is sampled from the surface hemisphere and the
	// specular is sampled from a cone of the phong lobe (which
	// gives a glossy look). For best results you need to normalize
	// the lighting (see http://www.thetenthplanet.de/archives/255)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...
	const float PI = 3.1415926;
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {
		vec3 diffuse;
		vec3 specular(0, 0, 0);

		if (intersect.m_material->isTextured()) {
			vec2 uv = intersect.m_uv_coord;
			diffuse = intersect.m_material->getTexture().sample(uv);
		}
		else {
			diffuse = intersect.m_material->diffuse();
		}
		float specularStrength = intersect.m_material->shininess();

		vec3 DiffuseS(0);
		vec3 SpecularS(0);

		if (depth > 0) {
			float rand1 = 2 * PI * ((float)rand() / (float)RAND_MAX);
			float rand2 = (float)rand() / (float)RAND_MAX;
			float rand2s = sqrt(rand2);
			vec3 intersectNormal = intersect.m_normal;
			vec3 u;
			if (abs(intersectNormal.x) > 0.01) {
				u = vec3(0, 1, 0);
			}
			else {
				u = vec3(1, 0, 0);
			}
			u = normalize(cross(u, intersectNormal));
			vec3 v = cross(intersectNormal, u);
			vec3 d = normalize(u * cos(rand1) * rand2s + v * sin(rand1) * rand2s + intersectNormal * sqrt(1 - rand2));

			Ray r;
			r.origin = intersect.m_position;
			r.direction = d;
			DiffuseS = diffuse * sampleRay(r, depth - 2);

			intersectNormal = reflect(ray.direction, intersect.m_normal);
			if (abs(intersectNormal.x) > 0.01f) {
				u = vec3(0, 1, 0);
			}
			else {
				u = vec3(1, 0, 0);
			}
			u = normalize(cross(u, intersectNormal));
			float n = (specularStrength - 1) / 30 + 1;
			rand2 = pow(rand2, n);
			rand2s = sqrt(rand2);
			v = cross(intersectNormal, u);
			d = normalize(u * cos(rand1) * rand2s + v * sin(rand1) * rand2s + intersectNormal * sqrt(1 - rand2));

			r.origin = intersect.m_position;
			r.direction = d;
			if (specularStrength < 10) depth = 1;
			SpecularS = sampleRay(r, depth - 1);
			float metalyness = intersect.m_material->metallyness();
			if (metalyness == 1)
				SpecularS = diffuse * SpecularS * 24.0f;
		}
		vec3 lightValue = vec3(0);
		vec3 lightAmbience = vec3(0);
		for (std::shared_ptr<Light> l : m_scene->lights()) {
			if (!l->occluded(m_scene, intersect.m_position)) {
				vec3 reflectDir = reflect(-l->incidentDirection(intersect.m_position), intersect.m_normal);
				vec3 viewDir = normalize(-ray.direction);
				float spec = pow(max(dot(viewDir, reflectDir), 0), specularStrength);
				float iD = abs(dot(l->incidentDirection(intersect.m_position), intersect.m_normal));
				specular += spec * intersect.m_material->specular() * l->irradiance(intersect.m_position);
				lightValue += l->irradiance(intersect.m_position) * iD;
			}
			lightAmbience += l->ambience();
		}
		vec3 il = (DiffuseS + (diffuse * lightAmbience) + (diffuse * lightValue) + specular + SpecularS) / 16.0f;
		return il;
	}
	return { 0.3f, 0.3f, 0.4f };
}

//Reference used for Challenge
// http://kevinbeason.com/smallpt/
