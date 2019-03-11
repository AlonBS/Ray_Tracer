

#include "Mesh.h"


ObjectProperties operator*(const MeshProperties& mp, const ObjectProperties& op)
{
	ObjectProperties ret =
	{
			._ambient   = mp._ambient   * op._ambient,
			._emission  = mp._emission  * op._emission,
			._diffuse   = mp._diffuse   * op._diffuse,
			._specular  = mp._specular  * op._specular,
			._shininess = mp._shininess * op._shininess
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
		   MeshProperties& properties,
		   Image *ambientTexture,
		   Image *diffuseTexture,
		   Image *specularTexture)

:_properties(properties),
 _ambientTexture(ambientTexture),
 _diffuseTexture(diffuseTexture),
 _specularTexture(specularTexture)
{
	__triangulate(vertices, indices);
	__computeBoundingBox(vertices);
	__computeBoundingVolume(vertices);
}


Mesh::~Mesh()
{
	for (Triangle *t : triangles) {
		delete(t);
		t = nullptr;
	}

	triangles.clear();

	if (boundingBox) {
		delete(boundingBox);
		boundingBox = nullptr;
	}

	if (boundingVolume) {
		delete(boundingVolume);
		boundingVolume = nullptr;
	}
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

void
Mesh::__computeBoundingBox(vector<Vertex>& vertices)
{
	vec3 minBound = vec3(INFINITY, INFINITY, INFINITY);
	vec3 maxBound = vec3(-INFINITY, -INFINITY, -INFINITY);

	for (Vertex v : vertices) {

		// Min Bound Calc
		if (v.Position.x < minBound.x)
			minBound.x = v.Position.x;
		if (v.Position.y < minBound.y)
			minBound.y = v.Position.y;
		if (v.Position.z < minBound.z)
			minBound.z = v.Position.z;

		// Max Bound Calc
		if (v.Position.x > maxBound.x)
			maxBound.x = v.Position.x;
		if (v.Position.y > maxBound.y)
			maxBound.y = v.Position.y;
		if (v.Position.z > maxBound.z)
			maxBound.z = v.Position.z;

	}

	boundingBox = new Box(minBound, maxBound);
	boundingBox->print();
}


void
Mesh::__computeBoundingVolume(vector<Vertex>& vertices)
{
	boundingVolume = new BoundingVolume();

	boundingVolume->computeBounds(vertices);
}


bool
Mesh::intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, vec2* texCoords, MeshProperties* properties)
{
	GLfloat minDist = INFINITY;

	GLfloat tDist;
	vec3 tP, tN;
	vec2 ttC;

	// If we don't pass the bounding box test - we don't test each triangle of this mesh
//	if (!boundingBox->intersectsRay(r, &tDist, nullptr, nullptr, nullptr, nullptr)) {
//		return false;
//	}

	if (!boundingVolume->intersectRay(r)) {
		return false;
	}

	for (Triangle *t : triangles) {

		/* When we iterate over triangles as part of mesh - we take the properties of the mesh
		 * and not the triangle. In fact, this triangle doesn't have other but default properties
		 */
		if (t->intersectsRayM(r, &tDist, &tP, &tN, &ttC)) {

			if (tDist < minDist) {

				*dist = minDist = tDist;
				if (point) *point = tP;
				if (normal) *normal = tN;
				if (texCoords) *texCoords = ttC;
				if (properties) *properties = this->_properties;
			}
		}
	}

	if (minDist == INFINITY) {
		return false;
	}

	return true;
}


vec3 Mesh::__getTextureColor(Image* texture, vec2& uv)
{
	if (!texture) {
		return COLOR_WHITE;
	}

	uv = glm::clamp(uv, 0.f + EPSILON, 1.f - EPSILON); // Textures at the edges tend to be not accurate

	int w = texture->getWidth();
	int h = texture->getHeight();

	/* TODO - consider interpolation for better effects (average near by pixels) */
	return texture->getPixel((int)(uv.x * w), (int) (uv.y * h));
}


vec3 Mesh::getAmbientTextureColor(vec2& uv)
{
	return this->__getTextureColor(_ambientTexture, uv);
}

vec3 Mesh::getDiffuseTextureColor(vec2& uv)
{
	return this->__getTextureColor(_diffuseTexture, uv);
}

vec3 Mesh::getSpecularTextureColor(vec2& uv)
{
	return this->__getTextureColor(_specularTexture, uv);
}

