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

	shared_ptr<const Image> ambientTexture;
	shared_ptr<const Image> diffuseTexture;
	shared_ptr<const Image> specularTexture;
	shared_ptr<const Image> normalsMap;
	shared_ptr<const Image> generalTexture;

	EnvMaps					envMaps;

	//vector<shared_ptr<const Image>> envMapsTextures;

}MeshTextures ;



class Mesh : Object {

private:

	using super = Object;

	vector<unique_ptr<const Triangle>> _triangles;
//	vector<Vertex> _vertices;

	MeshTextures _textures;
	bool _envMapped = false;
//	bool _refractiveMapping;
//	GLfloat _envMapRefIndex;

//	Image* _meshAmbientTexture;
//	Image* _meshDiffuseTexture;
//	Image* _meshSpecularTexture;

//	vector<Image*> _envMaps;




public:


	Mesh(vector<Vertex>& vertices,
		 vector<GLuint>& indices,
		 ObjectProperties& properties,
		 MeshTextures& textures);

	virtual ~Mesh();

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);
	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const;


	vec3 getAmbientTextureColor(vec2& uv) const ;
	vec3 getDiffuseTextureColor(vec2& uv) const ;
	vec3 getSpecularTextureColor(vec2& uv) const;

	const vector<unique_ptr<const Triangle>>& getTriangles() const;
	const vector<unique_ptr<const Triangle>>& getTriangles();

	void print() const;

private:

	void __triangulate(vector<Vertex>& vertices, vector<GLuint>& indices);

};


#endif
