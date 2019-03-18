#include "BoundingVolume.h"
#include "Model.h"


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


BoundingVolume::BoundingVolume(Object* obj)
: boundObject(obj)
{

	if (obj->isPrimitive()) {
		extents.clear();
	}
	else {
		Model* model = static_cast<Model*>(obj);
		cout << "MM size " << model->meshes.size() << endl;
		for (Mesh* mesh : model->meshes) {
			extents.push_back(new Extent(mesh));
		}

	}
}


BoundingVolume::~BoundingVolume()
{
	for (Extent* e : extents) {
		delete(e);
	}
	extents.clear();
}


bool BoundingVolume::intersectRay(const Ray &r,
		  	  	  	  	  	  	  GLfloat &minDist,
								  GLfloat* dist,
								  vec3* point,
								  vec3* normal,
								  ObjectTexColors* texColors,
								  ObjectProperties* properties)
{

	bool intersect = false;
	GLfloat tNear;
	GLfloat closestDist = minDist; // We iterate over the extents of the object - if we intersect some
								   // extent which is more far than the current closest object, we
	 	 	 	 	 	 	 	   // don't bother do test with mesh it bounds.
	vec3 tP, tN;
	vec2 ttC;
	MeshProperties meshProps;

	if (boundObject->isPrimitive()) {
		intersect = boundObject->intersectsRay(r, dist, point, normal, texColors, properties);
		if (intersect && *dist < minDist) {
			return true;
		}
		return false;
	}



	// TODO - need to sort them
	for (Extent* e : extents) {

		if (e->intersectRay(r,  &tNear)) {

			if (tNear < closestDist) {

				if (e->mesh->intersectsRay(r, &closestDist, &tP, &tN, &ttC, &meshProps))
				{
					if (point)  *point = tP;
					if (normal) *normal = tN;

					if (texColors) {
						texColors->_ambientTexColor  = e->mesh->getAmbientTextureColor(ttC) * this->boundObject->getAmbientTextureColor(ttC);
						texColors->_diffuseTexColor  = e->mesh->getDiffuseTextureColor(ttC) * this->boundObject->getDiffuseTextureColor(ttC);
						texColors->_specularTexColor = e->mesh->getSpecularTextureColor(ttC) * this->boundObject->getSpecularTextureColor(ttC);
					}

					if (properties) *properties = meshProps * boundObject->properties();

				}
			}
		}
	}

	// If it is still the case - then no new closer intersection was found
	if (closestDist == minDist) {
		return false;
	}

	return true;
}



BoundingVolume::Extent::Extent(Mesh* m)
: mesh(m)
{
	for (int i = 0 ; i < NUM_OF_SET_NORMALS ; ++i)
	{
		this->dists[i].dNear = INFINITY;
		this->dists[i].dFar = -INFINITY;
	}

	__computeBounds(m->getVertices());

}


void BoundingVolume::Extent::__computeBounds(vector<Vertex>& vertices)
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



bool BoundingVolume::Extent::intersectRay(const Ray &r, GLfloat* t_near)
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
		*t_near = t_near_final;
		return true;
	}

	return false;
}



void BoundingVolume::Extent::print() const
{
	for (int i = 0 ; i < NUM_OF_SET_NORMALS ; ++i) {
		printVec3("Norm", planeSetNormals[i]);
		cout << "dNear" << dists[i].dNear << endl;
		cout << "dFear" << dists[i].dFar  << endl;
	}
	// TODO - complete
}



