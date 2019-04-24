

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
					if (_envMapped) {

						GLint index;
						vec2 uv;
						vec3 xx = normalize(glm::reflect(r.direction, tN));
						printVec3("tN", tN);
						convert_xyz_to_cube_uv(xx.x, xx.y, xx.z, &index, &uv.x, &uv.y);


						GLuint index2;
						vec2 uv2;
						vec3 xx2 = normalize(glm::reflect(r.direction, tN));
						__mapEnvironment(xx2, uv2, index2);


						// TODO - remove once made sure this is stable
						if (!equalToVec3(xx, xx2)) {
							printVec3("XX", xx);
							printVec3("XX2", xx2);

						}

						if (index != index2) {
							cout << index << endl;
							cout << index2 << endl;
						}
//

//
//
//
//
////						Image* tex = this->getTextureColor(_envMaps[index], uv);
//
////						vec2 uv;
////						Image* tex;
////						__mapEnvironment(r.direction, tN, uv, tex);
//
//
//						printVec2("UV", uv);
//						cout << "INDEX: " << index << endl;;
//						printVec2("UV 2:", uv2);
//						cout << "INDEX 2: " << index2 << endl;;



						vec3 envColor = this->getTextureColor(_envMaps[index], uv);

						texColors->_ambientTexColor  = envColor * this->getAmbientTextureColor(ttC);;
						texColors->_diffuseTexColor  = envColor * this->getDiffuseTextureColor(ttC);;
						texColors->_specularTexColor = envColor * this->getSpecularTextureColor(ttC);;

					}
					else {
						texColors->_ambientTexColor  = this->getAmbientTextureColor(ttC);
						texColors->_diffuseTexColor  = this->getDiffuseTextureColor(ttC);
						texColors->_specularTexColor = this->getSpecularTextureColor(ttC);
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
	return this->getTextureColor(_meshAmbientTexture, uv) * super::getAmbientTextureColor(uv);
}

vec3 Mesh::getDiffuseTextureColor(vec2& uv)
{
	return this->getTextureColor(_meshDiffuseTexture, uv) * super::getDiffuseTextureColor(uv);
}

vec3 Mesh::getSpecularTextureColor(vec2& uv)
{
	return this->getTextureColor(_meshSpecularTexture, uv) * super::getSpecularTextureColor(uv);
}



void Mesh::__mapEnvironment(const vec3& r, vec2& uv, GLuint& texIndex)
{
	if (glm::abs(r.x) > glm::max(abs(r.y), abs(r.z)))
	{
		uv.x = (r.y/r.x + 1)/2;
		uv.y = (r.z/r.x + 1)/2;
		texIndex = (r.x > 0) ? 0 : 1;
	}

	else if (glm::abs(r.y) > glm::max(abs(r.x), abs(r.z)))
	{
		uv.x = (r.x/r.y + 1)/2;
		uv.y = (r.z/r.y + 1)/2;
		texIndex = (r.y > 0) ? 2 : 3;
	}

	else /* (glm::abs(r.z) > glm::max(abs(r.x), abs(r.z))) */
	{
		uv.x = (r.x/r.z + 1)/2;
		uv.y = (r.y/r.z + 1)/2;
		texIndex = (r.z > 0) ? 4 : 5;
	}
}




/* We'll keep it for now as a refernce */
void Mesh::convert_xyz_to_cube_uv(float x, float y, float z, int *index, float *u, float *v)
{
  float absX = fabs(x);
  float absY = fabs(y);
  float absZ = fabs(z);

  int isXPositive = x > 0 ? 1 : 0;
  int isYPositive = y > 0 ? 1 : 0;
  int isZPositive = z > 0 ? 1 : 0;

  float maxAxis, uc, vc;

  // POSITIVE X
  if (isXPositive && absX >= absY && absX >= absZ) {
    // u (0 to 1) goes from +z to -z
    // v (0 to 1) goes from -y to +y
    maxAxis = absX;
    uc = -z;
    vc = y;
    *index = 0;
  }
  // NEGATIVE X
  if (!isXPositive && absX >= absY && absX >= absZ) {
    // u (0 to 1) goes from -z to +z
    // v (0 to 1) goes from -y to +y
    maxAxis = absX;
    uc = z;
    vc = y;
    *index = 1;
  }
  // POSITIVE Y
  if (isYPositive && absY >= absX && absY >= absZ) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from +z to -z
    maxAxis = absY;
    uc = x;
    vc = -z;
    *index = 2;
  }
  // NEGATIVE Y
  if (!isYPositive && absY >= absX && absY >= absZ) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from -z to +z
    maxAxis = absY;
    uc = x;
    vc = z;
    *index = 3;
  }
  // POSITIVE Z
  if (isZPositive && absZ >= absX && absZ >= absY) {
    // u (0 to 1) goes from -x to +x
    // v (0 to 1) goes from -y to +y
    maxAxis = absZ;
    uc = x;
    vc = y;
    *index = 4;
  }
  // NEGATIVE Z
  if (!isZPositive && absZ >= absX && absZ >= absY) {
    // u (0 to 1) goes from +x to -x
    // v (0 to 1) goes from -y to +y
    maxAxis = absZ;
    uc = -x;
    vc = y;
    *index = 5;
  }

  // Convert range from -1 to 1 to 0 to 1
  *u = 0.5f * (uc / maxAxis + 1.0f);
  *v = 0.5f * (vc / maxAxis + 1.0f);
}




void Mesh::print() const
{
	super::print();
//	printVec3("Ambient", _properties._ambient);
//	printVec3("_emission", _properties._emission);
//	printVec3("_diffuse", _properties._diffuse);
//	printVec3("_specular", _properties._specular);
//	cout << "Shininess: " << _properties._shininess << endl;
}

