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



void BoundingVolume::print() const
{
	// TODO - complete

}



