

#include "Mesh.h"


ObjectProperties operator*(const MeshProperties& mp, const ObjectProperties& op)
{
	ObjectProperties ret =
	{
			._ambient         = mp._ambient   * op._ambient,
			._emission        = mp._emission  * op._emission,
			._diffuse         = mp._diffuse   * op._diffuse,
			._specular        = mp._specular  * op._specular,
			._shininess       = mp._shininess * op._shininess,

			//TODO - add these properties per mesh
			._reflection      = 1.f * op._reflection,
			._reflectionBlur  =	1.f * op._reflectionBlur,
			._refraction      = 1.f * op._refraction,
			._refractionIndex = 1.f * op._refractionIndex,
			._refractionBlur  =	1.f * op._refractionBlur
	};
	return ret;
}

ObjectProperties operator*(const ObjectProperties& op, const MeshProperties& mp)
{
	return mp*op;
}


    /*  Functions  */
    // constructor
Mesh::Mesh(vector<Vertex>& vertices,
		   vector<unsigned int>& indices,
		   ObjectProperties& properties,
		   Image *ambientTexture,
		   Image *diffuseTexture,
		   Image *specularTexture,
		   Image *generalTexture,
		   vector<Image*> envMaps)

: Object(),
 _meshAmbientTexture(ambientTexture),
 _meshDiffuseTexture(diffuseTexture),
 _meshSpecularTexture(specularTexture)
{
	super::setTexture(generalTexture);
	super::properties() = properties;

	_envMapped = envMaps.size() > 0;
	_envMaps = envMaps;

	_vertices = vertices; // Yes - this is shit, but a must for now
	__triangulate(vertices, indices);
}


Mesh::~Mesh()
{
	for (Triangle *t : triangles) {
		delete(t);
		t = nullptr;
	}

	triangles.clear();


}


void
Mesh::__triangulate(vector<Vertex> vertices, vector<unsigned int> indices)
{
	vec3 A, B, C;
	vec3 An, Bn, Cn;
	vec2 Auv, Buv, Cuv;


	for (unsigned int i = 0 ; i < indices.size() ; i+=3) {

		A   = vertices[indices[i  ]].Position;
		An  = vertices[indices[i  ]].Normal;
		Auv = vertices[indices[i  ]].TexCoords;

		B   = vertices[indices[i+1]].Position;
		Bn  = vertices[indices[i+1]].Normal;
		Buv = vertices[indices[i+1]].TexCoords;

		C   = vertices[indices[i+2]].Position;
		Cn  = vertices[indices[i+2]].Normal;
		Cuv = vertices[indices[i+2]].TexCoords;

		this->triangles.push_back(new Triangle(A, B, C, An, Bn, Cn, Auv, Buv, Cuv));

	}
}


bool
Mesh::intersectsRay(const Ray &r,
					GLfloat* dist,
					vec3* point,
					vec3* normal,
					ObjectTexColors* texColors,
					ObjectProperties* properties)
{
	GLfloat minDist = INFINITY;

	GLfloat tDist;
	vec3 tP, tN;
	vec2 ttC;


	for (Triangle *t : triangles) {

		/* When we iterate over triangles as part of mesh - we take the properties of the mesh
		 * and not the triangle. In fact, this triangle doesn't have other but default properties
		 */
		if (t->intersectsRayM(r, &tDist, &tP, &tN, &ttC)) {

			if (tDist < minDist) {

				*dist = minDist = tDist;
				if (point) *point = tP;
				if (normal) *normal = tN;

				if (texColors) {

					texColors->_ambientTexColor  = this->getAmbientTextureColor(ttC);
					texColors->_diffuseTexColor  = this->getDiffuseTextureColor(ttC);
					texColors->_specularTexColor = this->getSpecularTextureColor(ttC);

					if (_envMapped) {

						GLuint index;
						vec2 uv;
						vec3 refDir = glm::reflect(r.direction, tN);
						cubeMap(refDir, &index, &uv);
						vec3 envColor = getTextureColor(_envMaps[index], uv);

						texColors->_ambientTexColor  *= envColor;
						texColors->_diffuseTexColor  *= envColor;
						texColors->_specularTexColor *= envColor;
					}
				}

				if (properties) *properties = this->_properties;
			}
		}
	}

	return minDist != INFINITY;
}



bool
Mesh::intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const
{
	Mesh* m = (Mesh*)(this);
	return m->intersectsRay(r, dist, point, normal, texColors, properties);

}



vec3 Mesh::getAmbientTextureColor(vec2& uv)
{
	return getTextureColor(_meshAmbientTexture, uv) * super::getAmbientTextureColor(uv);
}

vec3 Mesh::getDiffuseTextureColor(vec2& uv)
{
	return getTextureColor(_meshDiffuseTexture, uv) * super::getDiffuseTextureColor(uv);
}

vec3 Mesh::getSpecularTextureColor(vec2& uv)
{
	return getTextureColor(_meshSpecularTexture, uv) * super::getSpecularTextureColor(uv);
}


void Mesh::print() const
{
	super::print();
}

