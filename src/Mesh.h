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

	vector<shared_ptr<const Image>> maps;
	bool refractiveMapping;
	GLfloat refractiveIndex; // This could be different from refractive Index of the mesh.

}EnvMaps;



typedef struct MeshTextures {


	// These are mesh specific
	shared_ptr<const Image> ambientTexture;
	shared_ptr<const Image> diffuseTexture;
	shared_ptr<const Image> specularTexture;
	shared_ptr<const Image> normalMap;

	// These are common amongst all meshes of this object
	shared_ptr<const Image> globalTexture;
	shared_ptr<const Image> globalNormalMap;

	EnvMaps					envMaps;

	//vector<shared_ptr<const Image>> envMapsTextures;

}MeshTextures ;



class Mesh : public Object {

private:

	using super = Object;

	vector<unique_ptr<const Triangle>> _triangles;

	MeshTextures _textures;
	bool _envMapped = false;

public:


	Mesh(vector<Vertex>& vertices,
		 vector<GLuint>& indices,
		 ObjectProperties& properties,
		 MeshTextures& textures);

	virtual ~Mesh();

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);
	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const;


	vec3 getAmbientTextureColor(const vec2& uv) const ;
	vec3 getDiffuseTextureColor(const vec2& uv) const ;
	vec3 getSpecularTextureColor(const vec2& uv) const;
	vec3 getNormalFromMap(const vec2& uv) const;

	const vector<unique_ptr<const Triangle>>& getTriangles() const;
	const vector<unique_ptr<const Triangle>>& getTriangles();

	void print() const;

private:

	void __triangulate(vector<Vertex>& vertices, vector<GLuint>& indices);

};


#endif
