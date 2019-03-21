
#include "BVH.h"




/***************************BBOX ****************************************/


/******************************
//       Public Methods
/*****************************/



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


/***************************BVH ****************************************/


/******************************
//       Public Methods
/*****************************/

const vec3 BVH::planeSetNormals[BVH::NUM_OF_SET_NORMALS] = {
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1),
    vec3( sqrtf(3) / 3.f,  sqrtf(3) / 3.f, sqrtf(3) / 3.f),
    vec3(-sqrtf(3) / 3.f,  sqrtf(3) / 3.f, sqrtf(3) / 3.f),
    vec3(-sqrtf(3) / 3.f, -sqrtf(3) / 3.f, sqrtf(3) / 3.f),
    vec3( sqrtf(3) / 3.f, -sqrtf(3) / 3.f, sqrtf(3) / 3.f)
};


//BVH::BVH(std::vector<std::unique_ptr<const Mesh>>& m) : AccelerationStructure(m)
BVH::BVH(std::vector<Mesh*>& meshes) /*: AccelerationStructure(meshes)*/
: meshes(meshes)
{

	Extents sceneExtents; // that's the extent of the entire scene which we need to compute for the octree
	extentsList.reserve(meshes.size());
	for (uint32_t i = 0; i < meshes.size(); ++i) {
		for (uint8_t j = 0; j < NUM_OF_SET_NORMALS; ++j) {
			for (const auto vtx : meshes[i]->getVertices()) {
				float d = glm::dot(planeSetNormals[j], vtx.Position);
				// set dNEar and dFar
				if (d < extentsList[i].d[j][0]) extentsList[i].d[j][0] = d;
				if (d > extentsList[i].d[j][1]) extentsList[i].d[j][1] = d;
			}
		}
		sceneExtents.extendBy(extentsList[i]); // expand the scene extent of this object's extent
		extentsList[i].mesh = meshes[i]; // the extent itself needs to keep a pointer to the object its holds
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


BVH::~BVH()
{
	delete octree;
}



bool
BVH::intersect(const vec3& orig, const vec3& dir, const uint32_t& rayId, float& tHit) const
{
	tHit = INFINITY;
	const Mesh* intersectedMesh = nullptr;
	float precomputedNumerator[BVH::NUM_OF_SET_NORMALS];
	float precomputedDenominator[BVH::NUM_OF_SET_NORMALS];
	for (uint8_t i = 0; i < NUM_OF_SET_NORMALS; ++i) {
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
	float tNear = 0, tFar = INFINITY; // tNear, tFar for the intersected extents
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
				float t = INFINITY;
				//                if (e->mesh->intersect(orig, dir, t) && t < tHit) {
				//                    tHit = t;
				//                    intersectedMesh = e->mesh;
				//                }
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







/***************************Octree ****************************************/


/******************************
//       Public Methods
/*****************************/

BVH::Octree::Octree(const Extents& sceneExtents)
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


BVH::Octree::~Octree()
{
	deleteOctreeNode(root);
}


void BVH::Octree::insert(const Extents* extents)
{
	insert(root, extents, bbox, 0);
}

void BVH::Octree::build()
{
	build(root, bbox);
};


/******************************
//       Private Methods
/*****************************/
void BVH::Octree::deleteOctreeNode(OctreeNode*& node)
{
	for (uint8_t i = 0; i < 8; i++) {
		if (node->child[i] != nullptr) {
			deleteOctreeNode(node->child[i]);
		}
	}
	delete node;
}


void BVH::Octree::insert(OctreeNode*& node, const Extents* extents, const BBox& bbox, uint32_t depth)
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


void BVH::Octree::build(OctreeNode*& node, const BBox& bbox)
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


bool BVH::Extents::intersect(
		const float* precomputedNumerator,
		const float* precomputedDenominator,
		float& tNear,   // tn and tf in this method need to be contained
		float& tFar,    // within the range [tNear:tFar]
		uint8_t& planeIndex) const
{
	//numRayBoundingVolumeTests++; TODO
	for (uint8_t i = 0; i < NUM_OF_SET_NORMALS; ++i) {
		float tNearExtents = (d[i][0] - precomputedNumerator[i]) / precomputedDenominator[i];
		float tFarExtents = (d[i][1] - precomputedNumerator[i]) / precomputedDenominator[i];
		if (precomputedDenominator[i] < 0) std::swap(tNearExtents, tFarExtents);
		if (tNearExtents > tNear) tNear = tNearExtents, planeIndex = i;
		if (tFarExtents < tFar) tFar = tFarExtents;
		if (tNear > tFar) return false;
	}

	return true;
}


