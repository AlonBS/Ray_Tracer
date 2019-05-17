

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
		   vector<GLuint>& indices,
		   ObjectProperties& properties,
		   MeshTextures& meshTextures)

: Object()
{

	super::setTextures(meshTextures.globalTexture, &meshTextures.globalNormalMap);
	_properties = properties;


	_textures.ambientTexture = meshTextures.ambientTexture;
	_textures.diffuseTexture = meshTextures.diffuseTexture;
	_textures.specularTexture = meshTextures.specularTexture;
	_textures.normalMap = meshTextures.normalMap;


	_textures.envMaps = meshTextures.envMaps;
	_envMapped = meshTextures.envMaps.maps.size() > 0;

	__triangulate(vertices, indices);
}



Mesh::~Mesh()
{
	_triangles.clear();
}


const vector<unique_ptr<const Triangle>>& Mesh::getTriangles() const
{
	return _triangles;
}

const vector<unique_ptr<const Triangle>>& Mesh::getTriangles()
{
	return const_cast<vector<unique_ptr<const Triangle>>&>(static_cast<const Mesh &>(*this).getTriangles());
}



void
Mesh::__triangulate(vector<Vertex>& vertices, vector<GLuint>& indices)
{
	for (GLuint i = 0 ; i < indices.size() ; i+=3)
	{
		this->_triangles.push_back(make_unique<const Triangle>(vertices[indices[i  ]],
											    			   vertices[indices[i+1]],
															   vertices[indices[i+2]]));
	}
}


bool
Mesh::intersectsRay(
		const Ray &r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties) const
{
	GLfloat minDist = INFINITY;

	GLfloat tDist;
	vec3 tP, tN;
	vec2 ttC;

	mat3 TBN;

	for (auto & t : _triangles) {

		/* When we iterate over triangles as part of mesh - we take the properties of the mesh
		 * and not the triangle. In fact, this triangle doesn't have other but default properties
		 */

		if (t->intersectsRayM(r, &tDist, &tP, &tN, &ttC, &TBN)) {

			if (tDist < minDist) {

				*dist = minDist = tDist;
				if (point) *point = tP;
				if (normal) {
					*normal = tN;

					if (!_objectGlobalProperties.no_bump_maps) {
						if (_textures.normalMap || super::_normalMap) {
							*normal = normalize(2.f*getNormalFromMap(ttC) - 1.0f);
							*normal = normalize(TBN * (*normal));
						}
					}
				}

				if (texColors) {

					texColors->_ambientTexColor  = this->getAmbientTextureColor(ttC);
					texColors->_diffuseTexColor  = this->getDiffuseTextureColor(ttC);
					texColors->_specularTexColor = this->getSpecularTextureColor(ttC);

					if (_envMapped) {

						GLuint index;
						vec2 uv;
						vec3 refDir;

						GLfloat nDotD = dot(r.direction, tN);
												vec3 dir;
												vec3 norm;
												GLfloat eta;

						if (_textures.envMaps.refractiveMapping) { //

							if (nDotD > EPSILON) { // Going out of the object
								dir = r.direction;
								norm = -tN;
								eta = _textures.envMaps.refractiveIndex / VOID_INDEX;
							}
							else {
								dir = r.direction; // TODO - consider -rayDir
								norm = tN;
								eta = VOID_INDEX / _textures.envMaps.refractiveIndex;
							}

							refDir = normalize(glm::refract(dir, norm, eta));
						}

						else { // Reflective Mapping

							refDir = normalize(glm::reflect(r.direction, tN));
						}

						if (vec3IsValid(refDir)) {

							cubeMap(refDir, &index, &uv);
							vec3 envColor = getTextureColor(_textures.envMaps.maps[index].get(), uv);

							texColors->_ambientTexColor  *= envColor;
							texColors->_diffuseTexColor  *= envColor;
							texColors->_specularTexColor *= envColor;
						}
					}
				}

				if (properties) *properties = this->_properties;
			}
		}
	}

	return minDist != INFINITY;
}



bool
Mesh::intersectsRay(
		const Ray &r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties)
{
	return static_cast<const Mesh&>(*this).intersectsRay(r, dist, point, normal, texColors, properties);
}



vec3 Mesh::getAmbientTextureColor(const vec2& uv) const
{
	return getTextureColor(_textures.ambientTexture.get(), uv) * super::getAmbientTextureColor(uv);
}

vec3 Mesh::getDiffuseTextureColor(const vec2& uv) const
{
	return getTextureColor(_textures.diffuseTexture.get(), uv) * super::getDiffuseTextureColor(uv);
}

vec3 Mesh::getSpecularTextureColor(const vec2& uv) const
{
	return getTextureColor(_textures.specularTexture.get(), uv) * super::getSpecularTextureColor(uv);
}

vec3 Mesh::getNormalFromMap(const vec2& uv) const
{
	return getTextureColor(_textures.normalMap.get(), uv) * super::getNormalFromMap(uv);
}


void Mesh::print() const
{
	super::print();
}

