/*
 * Ray.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#ifndef RAY_H_
#define RAY_H_

#include <iostream>
#include <glm/glm.hpp>

class Ray {
public:

	glm::vec3 origin;
	glm::vec3 direction;

	glm::vec3 invDirection; // Used to speed bounding box intersection tests
	int sign[3];

	Ray();
	Ray(glm::vec3& origin, glm::vec3& direction);
	Ray(const Ray& ray);

	virtual ~Ray();

	friend Ray operator* (const glm::mat4& M, const Ray& r);

	void print() const
	{
		std::cout << "O: (" << origin.x << "," << origin.y << "," << origin.z << ") |" ;
		std::cout << "D: (" << direction.x << "," << direction.y << "," << direction.z << ")" << std::endl;
	}

private:

	void _computeInverse();
};

#endif /* RAY_H_ */
