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
    BBox()
	{
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


bool BBox::intersect(const vec3& orig, const vec3& invDir, const bvec3& sign, float& tHit) const
{

    //numRayBBoxTests++; - TODO - add
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin  = (bounds[sign[0]    ].x - orig.x) * invDir.x;
    tmax  = (bounds[1 - sign[0]].x - orig.x) * invDir.x;
    tymin = (bounds[sign[1]    ].y - orig.y) * invDir.y;
    tymax = (bounds[1 - sign[1]].y - orig.y) * invDir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[sign[2]    ].z - orig.z) * invDir.z;
    tzmax = (bounds[1 - sign[2]].z - orig.z) * invDir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
    	tmin = tzmin;
    if (tzmax < tmax)
    	tmax = tzmax;

    tHit = tmin;

    return true;
}


class AccelerationStructure
{
public:
    // [comment]
    // We transfer owner ship of the mesh list to the acceleration structure. This makes
    // more sense from a functional/structure stand point because the objects/meshes themselves
    // should be destroyed/deleted when the acceleration structure is being deleted
    // Ideally this means the render function() itself should be bounded (in terms of lifespan)
    // to the lifespan of the acceleration structure (aka we should wrap the accel struc instance
    // and the render method() within the same object, so that when this object is deleted,
    // the render function can't be called anymore.
    // [/comment]
    AccelerationStructure(std::vector<std::unique_ptr<const Mesh>>& m) : meshes(std::move(m)) {}
    virtual ~AccelerationStructure() {}
    virtual bool intersect(const vec3& orig, const vec3& dir, const uint32_t& rayId, float& tHit) const
    {
        // [comment]
        // Because we don't want to change the content of the mesh itself, just get a point to it so
        // it's safer to make it const (which doesn't mean we can't change its assignment just that
        // we can't do something like intersectedMesh->color = blue. You would get something like:
        // "read-only variable is not assignable" error message at compile time)
        // [/comment]
        const Mesh* intersectedMesh = nullptr;
        float t = INFINITY;
        for (const auto& mesh: meshes) {
//            if (mesh->intersect(orig, dir, t) && t < tHit) {
//                intersectedMesh = mesh.get();
//                tHit = t;
//            }
        }

        return (intersectedMesh != nullptr);
    }
protected:
    const std::vector<std::unique_ptr<const Mesh>> meshes;
};

// [comment]
// Implementation of the ray-bbox method. If the ray intersects the bbox of a mesh then
// we test if the ray intersects the mesh contained by the bbox itself.
// [/comment]
class BBoxAcceleration : public AccelerationStructure
{
public:
    BBoxAcceleration(std::vector<std::unique_ptr<const Mesh>>& m) : AccelerationStructure(m) {}

    // [comment]
    // Implement the ray-bbox acceleration method. The method consist of intersecting the
    // ray against the bbox of the mesh first, and if the ray inteesects the boudning box
    // then test if the ray intersects the mesh itsefl. It is obvious that the ray can't
    // intersect the mesh if it doesn't intersect its boudning volume (a box in this case)
    // [/comment]
    virtual bool intersect(const Vec3f& orig, const Vec3f& dir, const uint32_t& rayId, float& tHit) const
    {
        const Mesh* intersectedMesh = nullptr;
        const Vec3f invDir = 1 / dir;
        const Vec3b sign(dir.x < 0, dir.y < 0, dir.z < 0);
        float t = kInfinity;
        for (const auto& mesh : meshes) {
            // If you intersect the box
            if (mesh->bbox.intersect(orig, invDir, sign, t)) {
                // Then test if the ray intersects the mesh and if does then first check
                // if the intersection distance is the nearest and if we pass that test as well
                // then update tNear variable with t and keep a pointer to the intersected mesh
                if (mesh->intersect(orig, dir, t) && t < tHit) {
                    tHit = t;
                    intersectedMesh = mesh.get();
                }
            }
        }

        // Return true if the variable intersectedMesh is not null, false otherwise
        return (intersectedMesh != nullptr);
    }
};



class BVH : public AccelerationStructure
{
private:
    static const uint8_t kNumPlaneSetNormals = 7;
    static const vec3 planeSetNormals[kNumPlaneSetNormals];
    struct Extents
    {
        Extents()
        :mesh(nullptr)
        {
            for (uint8_t i = 0;  i < kNumPlaneSetNormals; ++i)
                d[i][0] = INFINITY, d[i][1] = -INFINITY;
        }

        void extendBy(const Extents& e)
        {

            for (uint8_t i = 0;  i < kNumPlaneSetNormals; ++i) {
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
        float d[kNumPlaneSetNormals][2];
        const Mesh* mesh;
    };

    struct Octree
    {
        Octree(const Extents& sceneExtents)
        {
            float xDiff = sceneExtents.d[0][1] - sceneExtents.d[0][0];
            float yDiff = sceneExtents.d[1][1] - sceneExtents.d[1][0];
            float zDiff = sceneExtents.d[2][1] - sceneExtents.d[2][0];
            float maxDiff = std::max(xDiff, std::max(yDiff, zDiff));

            vec3 minPlusMax(sceneExtents.d[0][0] + sceneExtents.d[0][1],
            				sceneExtents.d[1][0] + sceneExtents.d[1][1],
							sceneExtents.d[2][0] + sceneExtents.d[2][1]);

            bbox.bounds[0] = (minPlusMax - maxDiff) * 0.5f;
            bbox.bounds[1] = (minPlusMax + maxDiff) * 0.5f;

            root = new OctreeNode;
        }

        ~Octree() { deleteOctreeNode(root); }

        void insert(const Extents* extents) { insert(root, extents, bbox, 0); }
        void build() { build(root, bbox); };

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

        void deleteOctreeNode(OctreeNode*& node)
        {
            for (uint8_t i = 0; i < 8; i++) {
                if (node->child[i] != nullptr) {
                    deleteOctreeNode(node->child[i]);
                }
            }
            delete node;
        }

        void insert(OctreeNode*& node, const Extents* extents, const BBox& bbox, uint32_t depth)
        {
            if (node->isLeaf) {
                if (node->nodeExtentsList.size() == 0 || depth == 16) {
                    node->nodeExtentsList.push_back(extents);
                }
                else {
                    node->isLeaf = false;
                    // Re-insert extents held by this node
                    while (node->nodeExtentsList.size()) {
                        insert(node, node->nodeExtentsList.back(), bbox, depth);
                        node->nodeExtentsList.pop_back();
                    }
                    // Insert new extent
                    insert(node, extents, bbox, depth);
                }
            }
            else {
                // Need to compute in which child of the current node this extents should
                // be inserted into
                vec3 extentsCentroid = extents->centroid();
                vec3 nodeCentroid = (bbox.bounds[0] + bbox.bounds[1]) * 0.5f;
                BBox childBBox;
                uint8_t childIndex = 0;
                // x-axis
                if (extentsCentroid.x > nodeCentroid.x) {
                    childIndex = 4;
                    childBBox.bounds[0].x = nodeCentroid.x;
                    childBBox.bounds[1].x = bbox.bounds[1].x;
                }
                else {
                    childBBox.bounds[0].x = bbox.bounds[0].x;
                    childBBox.bounds[1].x = nodeCentroid.x;
                }
                // y-axis
                if (extentsCentroid.y > nodeCentroid.y) {
                    childIndex += 2;
                    childBBox.bounds[0].y = nodeCentroid.y;
                    childBBox.bounds[1].y = bbox.bounds[1].y;
                }
                else {
                    childBBox.bounds[0].y = bbox.bounds[0].y;
                    childBBox.bounds[1].y = nodeCentroid.y;
                }
                // z-axis
                if (extentsCentroid.z > nodeCentroid.z) {
                    childIndex += 1;
                    childBBox.bounds[0].z = nodeCentroid.z;
                    childBBox.bounds[1].z = bbox.bounds[1].z;
                }
                else {
                    childBBox.bounds[0].z = bbox.bounds[0].z;
                    childBBox.bounds[1].z = nodeCentroid.z;
                }

                // Create the child node if it doesn't exsit yet and then insert the extents in it
                if (node->child[childIndex] == nullptr)
                    node->child[childIndex] = new OctreeNode;
                insert(node->child[childIndex], extents, childBBox, depth + 1);
            }
        }

        void build(OctreeNode*& node, const BBox& bbox)
        {
            if (node->isLeaf) {
                for (const auto& e: node->nodeExtentsList) {
                    node->nodeExtents.extendBy(*e);
                }
            }
            else {
                for (uint8_t i = 0; i < 8; ++i) {
                        if (node->child[i]) {
                        BBox childBBox;
                        vec3 centroid = bbox.centroid();
                        // x-axis
                        childBBox.bounds[0].x = (i & 4) ? centroid.x : bbox.bounds[0].x;
                        childBBox.bounds[1].x = (i & 4) ? bbox.bounds[1].x : centroid.x;
                        // y-axis
                        childBBox.bounds[0].y = (i & 2) ? centroid.y : bbox.bounds[0].y;
                        childBBox.bounds[1].y = (i & 2) ? bbox.bounds[1].y : centroid.y;
                        // z-axis
                        childBBox.bounds[0].z = (i & 1) ? centroid.z : bbox.bounds[0].z;
                        childBBox.bounds[1].z = (i & 1) ? bbox.bounds[1].z : centroid.z;

                        // Inspect child
                        build(node->child[i], childBBox);

                        // Expand extents with extents of child
                        node->nodeExtents.extendBy(node->child[i]->nodeExtents);
                    }
                }
            }
        }
    };

    std::vector<Extents> extentsList;
    Octree* octree = nullptr;
public:
    BVH(std::vector<std::unique_ptr<const Mesh>>& m);
    bool intersect(const vec3&, const vec3&, const uint32_t&, float&) const;
    ~BVH() { delete octree; }
}


const vec3 BVH::planeSetNormals[BVH::kNumPlaneSetNormals] = {
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1),
    vec3( sqrtf(3) / 3.f,  sqrtf(3) / 3.f, sqrtf(3) / 3.f),
    vec3(-sqrtf(3) / 3.f,  sqrtf(3) / 3.f, sqrtf(3) / 3.f),
    vec3(-sqrtf(3) / 3.f, -sqrtf(3) / 3.f, sqrtf(3) / 3.f),
    vec3( sqrtf(3) / 3.f, -sqrtf(3) / 3.f, sqrtf(3) / 3.f)
};

BVH::BVH(std::vector<std::unique_ptr<const Mesh>>& m) /*: AccelerationStructure(m)*/
{
    Extents sceneExtents; // that's the extent of the entire scene which we need to compute for the octree
    extentsList.reserve(meshes.size());
    for (uint32_t i = 0; i < meshes.size(); ++i) {
        for (uint8_t j = 0; j < kNumPlaneSetNormals; ++j) {
            for (const auto vtx : meshes[i]->vertexPool) {
                float d = dot(planeSetNormals[j], vtx);
                // set dNEar and dFar
                if (d < extentsList[i].d[j][0]) extentsList[i].d[j][0] = d;
                if (d > extentsList[i].d[j][1]) extentsList[i].d[j][1] = d;
            }
        }
        sceneExtents.extendBy(extentsList[i]); // expand the scene extent of this object's extent
        extentsList[i].mesh = meshes[i].get(); // the extent itself needs to keep a pointer to the object its holds
    }

    // Now that we have the extent of the scene we can start building our octree
    // Using C++ make_unique function here but you don't need to, just to learn something...
    octree = new Octree(sceneExtents);

    for (uint32_t i = 0; i < meshes.size(); ++i) {
        octree->insert(&extentsList[i]);
    }

    // Build from bottom up
    octree->build();
}

bool BVH::Extents::intersect(
    const float* precomputedNumerator,
    const float* precomputedDenominator,
    float& tNear,   // tn and tf in this method need to be contained
    float& tFar,    // within the range [tNear:tFar]
    uint8_t& planeIndex) const
{
    //numRayBoundingVolumeTests++; TODO
    for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i) {
        float tNearExtents = (d[i][0] - precomputedNumerator[i]) / precomputedDenominator[i];
        float tFarExtents = (d[i][1] - precomputedNumerator[i]) / precomputedDenominator[i];
        if (precomputedDenominator[i] < 0) std::swap(tNearExtents, tFarExtents);
        if (tNearExtents > tNear) tNear = tNearExtents, planeIndex = i;
        if (tFarExtents < tFar) tFar = tFarExtents;
        if (tNear > tFar) return false;
    }

    return true;
}

bool BVH::intersect(const vec3& orig, const vec3& dir, const uint32_t& rayId, float& tHit) const
{
    tHit = INFINITY;
    const Mesh* intersectedMesh = nullptr;
    float precomputedNumerator[BVH::kNumPlaneSetNormals];
    float precomputedDenominator[BVH::kNumPlaneSetNormals];
    for (uint8_t i = 0; i < kNumPlaneSetNormals; ++i) {
        precomputedNumerator[i] = dot(planeSetNormals[i], orig);
        precomputedDenominator[i] = dot(planeSetNormals[i], dir);
    }

    /*
    tNear = kInfinity; // set
    for (uint32_t i = 0; i < meshes.size(); ++i) {
        numRayVolumeTests++;
        float tn = -kInfinity, tf = kInfinity;
        uint8_t planeIndex;
        if (extents[i].intersect(precomputedNumerator, precomputedDenominator, tn, tf, planeIndex)) {
            if (tn < tNear) {
                intersectedMesh = meshes[i].get();
                tNear = tn;
                // normal = planeSetNormals[planeIndex];
            }
        }
    }
    */

    uint8_t planeIndex;
    float tNear = 0, tFar = kInfinity; // tNear, tFar for the intersected extents
    if (!octree->root->nodeExtents.intersect(precomputedNumerator, precomputedDenominator, tNear, tFar, planeIndex) || tFar < 0)
        return false;
    tHit = tFar;
    std::priority_queue<BVH::Octree::QueueElement> queue;
    queue.push(BVH::Octree::QueueElement(octree->root, 0));
    while (!queue.empty() && queue.top().t < tHit) {
        const Octree::OctreeNode *node = queue.top().node;
        queue.pop();
        if (node->isLeaf) {
            for (const auto& e: node->nodeExtentsList) {
                float t = kInfinity;
                if (e->mesh->intersect(orig, dir, t) && t < tHit) {
                    tHit = t;
                    intersectedMesh = e->mesh;
                }
            }
        }
        else {
            for (uint8_t i = 0; i < 8; ++i) {
                if (node->child[i] != nullptr) {
                    float tNearChild = 0, tFarChild = tFar;
                    if (node->child[i]->nodeExtents.intersect(precomputedNumerator, precomputedDenominator, tNearChild, tFarChild, planeIndex)) {
                        float t = (tNearChild < 0 && tFarChild >= 0) ? tFarChild : tNearChild;
                        queue.push(BVH::Octree::QueueElement(node->child[i], t));
                    }
                }
            }
        }
    }

    return (intersectedMesh != nullptr);
}

// [comment]
// Implementation of the Grid acceleration structure



#endif /* BVH_H */
