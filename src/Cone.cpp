#include "Cone.h"


bool Cone::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{
	// To find intersection between Ray and canonical Cone (aligned to the y-axis), we need to solve the following equation:
	// 	Cylinder: x^2 + z^2 = y^2
	// 	Ray:    (o + d*t)
	// we must solve a quadratic equations the form of Ax^2 + Bx + C = 0, where:
	// 	A: d.x^2 + d.z^2 - d.y^2
	//  B: 2*o.x*d.x + 2*o.z*d.z - 2*o.y*d.y
	// 	C: o.x^2 + o.z^2 - o.y^2

	Ray tr = this->invTransform() * r; // Transformed ray
	GLfloat A, B, C;
	GLfloat discriminant, disc_root;
	GLfloat t1 = INFINITY, t2 = INFINITY, t3 = INFINITY, t4 = INFINITY;
	vec3    ip, ip2; // Intersection points

	bool single_intersection = false;
	bool minCapIntersection = false, maxCapIntersection = false;
	GLfloat t_min = INFINITY, t_max = INFINITY;

	A = (tr.direction.x*tr.direction.x) + (tr.direction.z*tr.direction.z) - (tr.direction.y*tr.direction.y);
	B = (2*tr.origin.x*tr.direction.x) + (2*tr.origin.z*tr.direction.z) - (2*tr.origin.y*tr.direction.y);
	C = (tr.origin.x*tr.origin.x) + (tr.origin.z*tr.origin.z) - (tr.origin.y*tr.origin.y);

	discriminant = (B*B) - 4*A*C;
	if (discriminant < EPSILON) {
		// No intersection
		return false;
	}


	disc_root = glm::sqrt(discriminant);
	t1 = (-B + disc_root) / (2*A);
	t2 = (-B - disc_root) / (2*A);

	if (abs(t1 - t2) < EPSILON) {
		// same solution
		single_intersection = true;
		t_min = t1;
	}
	else if (t1 > EPSILON && t2 > EPSILON) {
		// Both positive
		single_intersection = false;
	}
	else {
		// One positive, one negative
		// or 2 negatives
		single_intersection = true;
		t_min = glm::max(t1, t2);
	}

	if (t_min < EPSILON) {
		// If dist is a negative values (accounting for floating point errors)
		// then both solutions were negative. Meaning we have to go back from the origin of
		// the ray (against its direction) to the intersection point - which means of course that
		// there's no intersection.
		return false;
	}

	if (single_intersection) {
		ip = tr.origin + t_min * tr.direction;
		if (ip.y > maxCap || ip.y < minCap){
			return false;
		}

	}
	else { // 2 intersections

		t_min = glm::min(t1,t2);
		t_max = glm::max(t1,t2);

		ip  = tr.origin + t_min * tr.direction;
		ip2 = tr.origin + t_max * tr.direction;

		if ((ip.y > maxCap && ip2.y > maxCap) || (ip.y < minCap && ip2.y < minCap) ){ // both intersection are on infinite cone (behind min or beyond max caps)
			return false;
		}
		if ((ip.y < minCap && ip2.y > minCap) || (ip.y > minCap && ip2.y < minCap)) { // min cap intersection
			t3 = (minCap - tr.origin.y) / tr.direction.y;
		}

		if ((ip.y < maxCap && ip2.z > maxCap) || (ip.y > maxCap && ip2.y < maxCap)) { // max cap intersection
			t4 = (maxCap - tr.origin.y) / tr.direction.y;
		}

		t_min = glm::min(t_min, glm::min(t3,t4));
		/* This is so we can properly display the object at the caps, as the minimum no longer applies here */
		if (ip.y < minCap) {
			t_min = t3;
			minCapIntersection = true;
		}
		else if (ip.y > maxCap) {
			t_min = t4;
			maxCapIntersection = true;
		}
		// TODO - fix issue when rendering cylinder parallel to the camera view
		ip  = tr.origin + t_min * tr.direction;
	}

	// This is the normal at intersection point. (The Cone is aligned with the y-axis)
	vec3 n = _normalAt(ip, minCapIntersection, maxCapIntersection);
	n = normalize(vec3(mat3(this->invTransposeTrans()) * n));

	// M * p - to transform point back
	ip2 = ip; // for texture - we need the non transformed position
	ip = vec3(this->transform() * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}
	if (normal) {
		*normal = n;
	}
	if (texColors) {

		vec2 uv;
		uv.x = 0.5 + atan2(ip2.x, ip2.z) / (2*PI); // == (atan2(ip2.x, ip2.z) + PI) / (2*PI)
		uv.y = (ip2.y - minCap) / (height);

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
	}
	if (properties) {
		*properties = this->properties();
	}


	return true;
}



vec3 Cone::_normalAt(const vec3& p, bool minCapIntersect, bool maxCapIntersect)
{
	if (minCapIntersect)
		return vec3(0, -1, 0);
	else if (maxCapIntersect)
		return vec3(0, 1, 0);
	else {
		vec3 n;
		vec3 v = normalize(vec3(p.x - 0, 0, p.z-0));
		n.x = v.x * height / radius;
		n.y = radius / height;
		n.z = v.z * height / radius;
		return normalize(n);
	}
}


void Cone::print() const
{
	std::cout << "Cone Center: (" << center.x << "," << center.y << "," << center.z << ") | Radius: " << radius << " | minCap: " << minCap << " | Max Cap: " << maxCap << std::endl;
	Object::print();
}



