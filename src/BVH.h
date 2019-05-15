/*
 * BVH.h
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
#include "Box.h"

using namespace glm;



class BVH
{

public:
	BVH(vector<shared_ptr<const Mesh>>& meshes);
	~BVH();

	bool intersectsRay(const Ray &r,
					   const GLfloat &minDist,
					   GLfloat* dist,
					   vec3* point,
					   vec3* normal,
					   ObjectTexColors* texColors,
					   ObjectProperties* properties,
					   bool shadowRay = false);

	bool intersectsRay(const Ray &r,
					   const GLfloat &minDist,
					   GLfloat* dist,
					   vec3* point,
					   vec3* normal,
					   ObjectTexColors* texColors,
					   ObjectProperties* properties,
					   bool shadowRay = false) const;



private:

	static const GLuint NUM_OF_SET_NORMALS = 7;
	static const vec3 PLANE_SET_NORMALS[NUM_OF_SET_NORMALS];

	struct Extents
	{
		typedef struct SlabDist {

			GLfloat dNear;
			GLfloat dFar;

		}SlabDist;


		Extents()
		{
			mesh.reset();
			for (uint8_t i = 0;  i < NUM_OF_SET_NORMALS; ++i) {
				dists[i].dNear = INFINITY, dists[i].dFar = -INFINITY;
			}
		}

		void build(shared_ptr<const Mesh>& meshA)
		{
			for (uint8_t i = 0; i < NUM_OF_SET_NORMALS; ++i)
			{
				dists[i].dNear = INFINITY, dists[i].dFar = -INFINITY;
				for (auto& t : meshA->getTriangles())
				{
					for (auto& v : t->getVerticesPos())
					{
						GLfloat d = dot(PLANE_SET_NORMALS[i], v);
						dists[i].dNear = glm::min(dists[i].dNear, d);
						dists[i].dFar  = glm::max(dists[i].dFar, d);
					}

				}
			}
			//this->mesh.reset();
			//this->mesh = make_shared<const Mesh>(*mesh.get());
			this->mesh = meshA;
		}

		void extendBy(const Extents& other)
		{

			for (uint8_t i = 0;  i < NUM_OF_SET_NORMALS; ++i) {

				dists[i].dNear = glm::min(dists[i].dNear, other.dists[i].dNear);
				dists[i].dFar  = glm::max(dists[i].dFar,  other.dists[i].dFar);
			}
		}

		vec3 centroid() const
		{
			// With respect to XYZ axis - simple average
			return vec3( 0.5 * (dists[0].dNear + dists[0].dFar),
						 0.5 * (dists[1].dNear + dists[1].dFar),
						 0.5 * (dists[2].dNear + dists[2].dFar));
		}

		bool intersect(const GLfloat[], const GLfloat[], GLfloat*, GLfloat*) const;

		SlabDist dists [NUM_OF_SET_NORMALS];
		shared_ptr<const Mesh> mesh;
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

		// For priority queue sorting
		struct QueueElement
		{
			const OctreeNode *node; // octree node held by this element in the queue
			GLfloat t; // distance from the ray origin to the extents of the node
			QueueElement(const OctreeNode *n, GLfloat tn) : node(n), t(tn) {}
			friend bool operator < (const QueueElement &a, const QueueElement &b) { return a.t > b.t; }
		};

		OctreeNode* root = nullptr;
		AABB bbox;

		static GLuint maxDepth;

	private:



		void deleteOctreeNode(OctreeNode*& node);

		void insert(OctreeNode*& node, const Extents* extents, const AABB& bbox, uint32_t depth);

		void build(OctreeNode*& node, const AABB& bbox);


		void computeChildIndexAndBbox(const vec3& extentsCentroid,
				   	   	   	   	   	  const vec3& nodeCentroid,
									  AABB* childBbox,
									  GLuint* childIndex);
	};


	Extents __buildSceneExtents(vector<shared_ptr<const Mesh>>& meshes);


	vector<Extents> extentsList;
	unique_ptr<Octree> octree = nullptr;
};



#endif /* BVH_H */
