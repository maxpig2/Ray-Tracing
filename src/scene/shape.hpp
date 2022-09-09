
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "ray.hpp"
#include "scene.hpp"


class Shape {
public:
	virtual RayIntersection intersect(const Ray &ray) = 0;
};


class AABB : public Shape {
private:
	glm::vec3 m_center;
	glm::vec3 m_halfsize;

public:
	AABB(const glm::vec3 &c, float hs) : m_center(c), m_halfsize(hs) { }
	AABB(const glm::vec3 &c, const glm::vec3 &hs) : m_center(c), m_halfsize(hs) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};


class Sphere : public Shape {
private:
	glm::vec3 m_center;
	float m_radius;

public:
	Sphere(const glm::vec3 &c, float radius) : m_center(c), m_radius(radius) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

//-------------------------------------------------------------
// [Assignment 4] :
// Implement the following additional Shapes :
// - Plane
// - Disk
// - Triangle
// Follow the pattern shown by AABB and Sphere for implementing
// a class that subclasses Shape making sure that you implement
// the intersect method for each new Shape.
//-------------------------------------------------------------

// YOUR CODE GOES HERE
// ...

class Plane : public Shape {
private:
	glm::vec3 m_point;
	glm::vec3 m_normal;

public:
	Plane(const glm::vec3& c, const glm::vec3& norm) : m_point(c) , m_normal(normalize(norm)) { }
	virtual RayIntersection intersect(const Ray& ray) override;
};

class Disc : public Shape {
private:
	glm::vec3 m_point;
	glm::vec3 m_normal;
	float radius;

public:
	Disc(const glm::vec3& c, const glm::vec3& norm, float r) : m_point(c), m_normal(normalize(norm)), radius(r) { }
	virtual RayIntersection intersect(const Ray& ray) override;
};

class Triangle : public Shape {
private:
	glm::vec3 m_point1;
	glm::vec3 m_point2;
	glm::vec3 m_point3;
	glm::vec3 m_normal;

public:
	Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) : 
		m_point1(p1), m_point2(p2), m_point3(p3) {
		m_normal = normalize(cross(p2 - p1, p3 - p1));
	}
	virtual RayIntersection intersect(const Ray& ray) override;
};

