#include "BoundingVolume.h"


using namespace glm;


const vec3 BoundingVolume::planeSetNormals[7] =
{
		vec3(1, 0, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 1),
		vec3( sqrtf(3) / 3.f,  sqrtf(3) / 3.f, sqrtf(3) / 3.f),
		vec3(-sqrtf(3) / 3.f,  sqrtf(3) / 3.f, sqrtf(3) / 3.f),
		vec3(-sqrtf(3) / 3.f, -sqrtf(3) / 3.f, sqrtf(3) / 3.f),
		vec3( sqrtf(3) / 3.f, -sqrtf(3) / 3.f, sqrtf(3) / 3.f)
};


BoundingVolume::BoundingVolume()
{
	for (int i = 0 ; i < NUM_OF_SET_NORMALS ; ++i)
	{
		this->dists[i].dNear = INFINITY;
		this->dists[i].dFar = -INFINITY;
	}
}


void BoundingVolume::computeBounds(vector<Vertex>& vertices)
{
	GLfloat d;

	for (int i = 0 ; i < NUM_OF_SET_NORMALS ; ++i ) {

		for (Vertex v : vertices)
		{
			d = dot(planeSetNormals[i], v.Position);
			dists[i].dNear = glm::min(dists[i].dNear, d);
			dists[i].dFar  = glm::max(dists[i].dFar, d);
		}
	}
}

bool BoundingVolume::intersectRay(const Ray &r)
{

	GLfloat t_far_final = INFINITY, t_near_final = -INFINITY;


	for (int i = 0 ; i < NUM_OF_SET_NORMALS ; ++i)
	{
		GLfloat nDotO = dot(planeSetNormals[i], r.origin);
		GLfloat nDotD = dot(planeSetNormals[i], r.direction);

		if (abs(nDotD) < EPSILON)
			continue;

		GLfloat tNear = (dists[i].dNear - nDotO) / nDotD;
		GLfloat tFar  = (dists[i].dFar  - nDotO) / nDotD;

		if (nDotD < 0) {
			swap(tNear, tFar);
		}

		t_near_final = glm::max(tNear, t_near_final);
		t_far_final = glm::min(tFar, t_far_final);

	}

	if (t_far_final > t_near_final) {
		return true;
	}

	return false;
}



void BoundingVolume::print() const
{
	for (int i = 0 ; i < NUM_OF_SET_NORMALS ; ++i) {
		printVec3("Norm", planeSetNormals[i]);
		cout << "dNear" << dists[i].dNear << endl;
		cout << "dFear" << dists[i].dFar  << endl;
	}
	// TODO - complete
}



