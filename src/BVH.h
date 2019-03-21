/*
 * Box.h
 *
 *  Created on: Mar 10, 2019
 *      Author: alonbs
 */

#ifndef BVH_H
#define BVH_H

#include <atomic>
#include <memory>
#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <cmath>
#include <chrono>
#include <queue>

#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <glm/glm.hpp>

#include "Mesh.h"

using namespace glm;


class BBox
{
public:
	BBox() {
		bounds[0] = vec3(-INFINITY, -INFINITY, -INFINITY);
		bounds[1] = vec3(+INFINITY, +INFINITY, +INFINITY);
	}

	BBox(vec3 min_, vec3 max_)
	{
		bounds[0] = min_;
		bounds[1] = max_;
	}
	BBox& extendBy(const vec3& p)
	{
		if (p.x < bounds[0].x) bounds[0].x = p.x;
		if (p.y < bounds[0].y) bounds[0].y = p.y;
		if (p.z < bounds[0].z) bounds[0].z = p.z;
		if (p.x > bounds[1].x) bounds[1].x = p.x;
		if (p.y > bounds[1].y) bounds[1].y = p.y;
		if (p.z > bounds[1].z) bounds[1].z = p.z;

		return *this;
	}
	vec3 centroid() const { return (bounds[0] + bounds[1]) * 0.5f; }
	//vec3& operator [] (bool i) { return bounds[i]; }
	//const vec3 operator [] (bool i) const { return bounds[i]; }
	bool intersect(const vec3&, const vec3&, const bvec3&, float&) const;
	vec3 bounds[2];
};


//
//class AccelerationStructure
//{
//public:
//	// [comment]
//	// We transfer owner ship of the mesh list to the acceleration structure. This makes
//	// more sense from a functional/structure stand point because the objects/meshes themselves
//	// should be destroyed/deleted when the acceleration structure is being deleted
//	// Ideally this means the render function() itself should be bounded (in terms of lifespan)
//	// to the lifespan of the acceleration structure (aka we should wrap the accel struc instance
//	// and the render method() within the same object, so that when this object is deleted,
//	// the render function can't be called anymore.
//	// [/comment]
//	AccelerationStructure(std::vector<std::unique_ptr<const Mesh>>& m) : meshes(std::move(m)) {}
//	virtual ~AccelerationStructure() {}
//	virtual bool intersect(const vec3& orig, const vec3& dir, const uint32_t& rayId, float& tHit) const
//	{
//		// [comment]
//		// Because we don't want to change the content of the mesh itself, just get a point to it so
//		// it's safer to make it const (which doesn't mean we can't change its assignment just that
//		// we can't do something like intersectedMesh->color = blue. You would get something like:
//		// "read-only variable is not assignable" error message at compile time)
//		// [/comment]
//		const Mesh* intersectedMesh = nullptr;
//		float t = INFINITY;
//		for (const auto& mesh: meshes) {
//			//            if (mesh->intersect(orig, dir, t) && t < tHit) {
//			//                intersectedMesh = mesh.get();
//			//                tHit = t;
//			//            }
//		}
//
//		return (intersectedMesh != nullptr);
//	}
//protected:
//	const std::vector<std::unique_ptr<const Mesh>> meshes;
//};

// [comment]
// Implementation of the ray-bbox method. If the ray intersects the bbox of a mesh then
// we test if the ray intersects the mesh contained by the bbox itself.
// [/comment]
//class BBoxAcceleration : public AccelerationStructure
//{
//public:
//	BBoxAcceleration(std::vector<std::unique_ptr<const Mesh>>& m) : AccelerationStructure(m) {}
//
//	// [comment]
//	// Implement the ray-bbox acceleration method. The method consist of intersecting the
//	// ray against the bbox of the mesh first, and if the ray inteesects the boudning box
//	// then test if the ray intersects the mesh itsefl. It is obvious that the ray can't
//	// intersect the mesh if it doesn't intersect its boudning volume (a box in this case)
//	// [/comment]
//	virtual bool intersect(const vec3& orig, const vec3& dir, const uint32_t& rayId, float& tHit) const
//	{
//		const Mesh* intersectedMesh = nullptr;
//		const vec3 invDir = 1.f / dir;
//		const bvec3 sign(dir.x < 0, dir.y < 0, dir.z < 0);
//		float t = INFINITY;
//		for (const auto& mesh : meshes) {
//			//            // If you intersect the box
//			//            if (mesh->bbox.intersect(orig, invDir, sign, t)) {
//			//                // Then test if the ray intersects the mesh and if does then first check
//			//                // if the intersection distance is the nearest and if we pass that test as well
//			//                // then update tNear variable with t and keep a pointer to the intersected mesh
//			//                if (mesh->intersect(orig, dir, t) && t < tHit) {
//			//                    tHit = t;
//			//                    intersectedMesh = mesh.get();
//			//                }
//			//            }
//		}
//
//		// Return true if the variable intersectedMesh is not null, false otherwise
//		return (intersectedMesh != nullptr);
//	}
//};


class BVH /*: AccelerationStructure */
{

public:
	BVH(vector<Mesh*>& meshes);
	~BVH();
	bool intersect(const vec3&, const vec3&, const uint32_t&, float&) const;

	bool intersectsRay(const Ray &r,
					   GLfloat &minDist,
					   GLfloat* dist,
					   vec3* point,
					   vec3* normal,
					   ObjectTexColors* texColors,
					   ObjectProperties* properties);



private:
	static const GLuint NUM_OF_SET_NORMALS = 7;
	static const vec3 planeSetNormals[NUM_OF_SET_NORMALS];

	vector<Mesh*> meshes;


	struct Extents
	{
		Extents()
		:mesh(nullptr)
		{
			for (uint8_t i = 0;  i < NUM_OF_SET_NORMALS; ++i)
				d[i][0] = INFINITY, d[i][1] = -INFINITY;
		}

		void extendBy(const Extents& e)
		{

			for (uint8_t i = 0;  i < NUM_OF_SET_NORMALS; ++i) {
				if (e.d[i][0] < d[i][0]) d[i][0] = e.d[i][0];
				if (e.d[i][1] > d[i][1]) d[i][1] = e.d[i][1];
			}
		}
		/* inline */
		vec3 centroid() const
		{
			return vec3(d[0][0] + d[0][1] * 0.5,
					d[1][0] + d[1][1] * 0.5,
					d[2][0] + d[2][1] * 0.5);
		}

		bool intersect(const float*, const float*, float&, float&, uint8_t&) const;
		float d[NUM_OF_SET_NORMALS][2];
		const Mesh* mesh;
	};


	class Octree
	{
	public:
		Octree(const Extents& sceneExtents);

		~Octree();

		void insert(const Extents* extents);
		void build();

		struct OctreeNode
		{
			OctreeNode* child[8] = { nullptr };
			std::vector<const Extents *> nodeExtentsList; // pointer to the objects extents
			Extents nodeExtents; // extents of the octree node itself
			bool isLeaf = true;
		};

		struct QueueElement
		{
			const OctreeNode *node; // octree node held by this element in the queue
			float t; // distance from the ray origin to the extents of the node
			QueueElement(const OctreeNode *n, float tn) : node(n), t(tn) {}
			// priority_queue behaves like a min-heap
			friend bool operator < (const QueueElement &a, const QueueElement &b) { return a.t > b.t; }
		};

		OctreeNode* root = nullptr; // make unique son don't have to manage deallocation
		BBox bbox;

	private:

		void deleteOctreeNode(OctreeNode*& node);

		void insert(OctreeNode*& node, const Extents* extents, const BBox& bbox, uint32_t depth);

		void build(OctreeNode*& node, const BBox& bbox);
	};


	std::vector<Extents> extentsList;
	Octree* octree = nullptr;
};



#endif /* BVH_H */
