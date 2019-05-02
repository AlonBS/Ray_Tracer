#ifndef MESH_H
#define MESH_H


#include <glm/glm.hpp>
#include <vector>

#include "Triangle.h"
#include "Box.h"

using namespace std;


typedef struct MeshProperties : ObjectProperties {
	/* Currently doesn't have extra features */

	friend ObjectProperties operator*(const MeshProperties& mp, const ObjectProperties& op);
	friend ObjectProperties operator*(const ObjectProperties& op, const MeshProperties& mp);

}MeshProperties;


typedef struct EnvMaps {

	vector<Image*> maps;
	bool refractiveMapping;
	GLfloat refractiveIndex; // This could be different from refractive Index of the mesh.

}EnvMaps;



class Mesh : Object {

private:

	using super = Object;

	vector<Triangle*> triangles;

	vector<Vertex> _vertices;

	Image* _meshAmbientTexture;
	Image* _meshDiffuseTexture;
	Image* _meshSpecularTexture;

	vector<Image*> _envMaps;
	bool _envMapped = false;
	bool _refractiveMapping;
	GLfloat _envMapRefIndex;


public:


	Mesh(vector<Vertex>& vertices,
		 vector<unsigned int>& indices,
		 ObjectProperties& properties,
		 Image *ambientTexture,
		 Image *diffuseTexture,
		 Image *specualrTexture,
		 Image *generalTexture,
		 EnvMaps &envMaps);

	virtual ~Mesh();

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);
	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const;


	vec3 getAmbientTextureColor(vec2& uv);
	vec3 getDiffuseTextureColor(vec2& uv);
	vec3 getSpecularTextureColor(vec2& uv);

	const vector<Vertex>& getVertices() const { return _vertices; }
	const vector<Vertex>& getVertices() { return const_cast<vector<Vertex>&>(static_cast<const Mesh &>(*this).getVertices()); }


	void print() const;

private:


	void __triangulate(vector<Vertex> vertices, vector<unsigned int> indices);

	void __mapEnvironment(const vec3& r, vec2& uv, GLuint& texIndex);
	void convert_xyz_to_cube_uv(float x, float y, float z, int *index, float *u, float *v);

};


#endif
