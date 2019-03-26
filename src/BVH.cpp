
#include "BVH.h"


//
//
///***************************BBOX ****************************************/
//
//
///******************************
////       Public Methods
///*****************************/
//
//
//
//bool BBox::intersect(const vec3& orig, const vec3& invDir, const bvec3& sign, float& tHit) const
//{
//
//	//numRayBBoxTests++; - TODO - add
//	float tmin, tmax, tymin, tymax, tzmin, tzmax;
//
//	tmin  = (bounds[sign[0]    ].x - orig.x) * invDir.x;
//	tmax  = (bounds[1 - sign[0]].x - orig.x) * invDir.x;
//	tymin = (bounds[sign[1]    ].y - orig.y) * invDir.y;
//	tymax = (bounds[1 - sign[1]].y - orig.y) * invDir.y;
//
//	if ((tmin > tymax) || (tymin > tmax))
//		return false;
//
//	if (tymin > tmin)
//		tmin = tymin;
//	if (tymax < tmax)
//		tmax = tymax;
//
//	tzmin = (bounds[sign[2]    ].z - orig.z) * invDir.z;
//	tzmax = (bounds[1 - sign[2]].z - orig.z) * invDir.z;
//
//	if ((tmin > tzmax) || (tzmin > tmax))
//		return false;
//
//	if (tzmin > tmin)
//		tmin = tzmin;
//	if (tzmax < tmax)
//		tmax = tzmax;
//
//	tHit = tmin;
//
//	return true;
//}


/***************************BVH ****************************************/


/******************************
//       Public Methods
/*****************************/

const vec3 BVH::PLANE_SET_NORMALS[BVH::NUM_OF_SET_NORMALS] = {
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

	Extents sceneExtents = __buildSceneExtents(meshes); // that's the extent of the entire scene which we need to compute for the octree

	// Now that we have the extents of the scene we can start building our octree
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


//
//bool
//BVH::intersect(const vec3& orig, const vec3& dir, const uint32_t& rayId, float& tHit) const
//{
//	tHit = INFINITY;
//	const Mesh* intersectedMesh = nullptr;
//	float precomputedNumerator[BVH::NUM_OF_SET_NORMALS];
//	float precomputedDenominator[BVH::NUM_OF_SET_NORMALS];
//	for (uint8_t i = 0; i < NUM_OF_SET_NORMALS; ++i) {
//		precomputedNumerator[i] = dot(planeSetNormals[i], orig);
//		precomputedDenominator[i] = dot(planeSetNormals[i], dir);
//	}
//
//	/*
//    tNear = kInfinity; // set
//    for (uint32_t i = 0; i < meshes.size(); ++i) {
//        numRayVolumeTests++;
//        float tn = -kInfinity, tf = kInfinity;
//        uint8_t planeIndex;
//        if (extents[i].intersect(precomputedNumerator, precomputedDenominator, tn, tf, planeIndex)) {
//            if (tn < tNear) {
//                intersectedMesh = meshes[i].get();
//                tNear = tn;
//                // normal = planeSetNormals[planeIndex];
//            }
//        }
//    }
//	 */
//
//	uint8_t planeIndex;
//	float tNear = 0, tFar = INFINITY; // tNear, tFar for the intersected extents
//	if (!octree->root->nodeExtents.intersect(precomputedNumerator, precomputedDenominator, tNear, tFar, planeIndex) || tFar < 0)
//		return false;
//	tHit = tFar;
//	std::priority_queue<BVH::Octree::QueueElement> queue;
//	queue.push(BVH::Octree::QueueElement(octree->root, 0));
//	while (!queue.empty() && queue.top().t < tHit) {
//		const Octree::OctreeNode *node = queue.top().node;
//		queue.pop();
//		if (node->isLeaf) {
//			for (const auto& e: node->nodeExtentsList) {
//				float t = INFINITY;
//				//                if (e->mesh->intersect(orig, dir, t) && t < tHit) {
//				//                    tHit = t;
//				//                    intersectedMesh = e->mesh;
//				//                }
//			}
//		}
//		else {
//			for (uint8_t i = 0; i < 8; ++i) {
//				if (node->child[i] != nullptr) {
//					float tNearChild = 0, tFarChild = tFar;
//					if (node->child[i]->nodeExtents.intersect(precomputedNumerator, precomputedDenominator, tNearChild, tFarChild, planeIndex)) {
//						float t = (tNearChild < 0 && tFarChild >= 0) ? tFarChild : tNearChild;
//						queue.push(BVH::Octree::QueueElement(node->child[i], t));
//					}
//				}
//			}
//		}
//	}
//
//	return (intersectedMesh != nullptr);
//}




bool BVH::intersectsRay(const Ray &r,
				   	    const GLfloat &minDist,
						GLfloat* dist,
						vec3* point,
						vec3* normal,
						ObjectTexColors* texColors,
						ObjectProperties* properties)
{
	bool intersected = false;
	GLfloat closestHit = minDist;

	//tHit = INFINITY;
	//const Mesh* intersectedMesh = nullptr;

	GLfloat precomputedNumerators[BVH::NUM_OF_SET_NORMALS];
	GLfloat precomputedDenominators[BVH::NUM_OF_SET_NORMALS];
	for (uint8_t i = 0; i < NUM_OF_SET_NORMALS; ++i) {
		precomputedNumerators[i] = dot(PLANE_SET_NORMALS[i], r.origin);
		precomputedDenominators[i] = dot(PLANE_SET_NORMALS[i], r.direction);
	}


	GLfloat tNear, tFar;
	bool extentsIntersected = octree->root->nodeExtents.intersect(precomputedNumerators, precomputedDenominators, &tNear, &tFar);
	if (!extentsIntersected || tFar < 0) {
		return false;
	}


	GLfloat closestT = (tNear < 0 && tFar >= 0) ? tFar : tNear;
	// We've already found closer intersection
	if (minDist < closestT) {
		return false;
	}

//	closestHit = tNear; // TODO - was tFar;
//	closestHit = tFar; // TODO - was tFar;

	priority_queue<BVH::Octree::QueueElement> queue;
	queue.push(BVH::Octree::QueueElement(octree->root, 0));

	while (!queue.empty() && queue.top().t < closestHit) {
		const Octree::OctreeNode *node = queue.top().node;
		queue.pop();
		if (node->isLeaf) {
			for (const auto& e: node->nodeExtentsList) {

//				GLfloat t = INFINITY;
				if (e->mesh->intersectsRay(r, dist, point, normal, texColors, properties)) {

					if (*dist < closestHit) {
						// Notice we only update closestHit upon intersection with object (mesh), NOT extents
						closestHit = *dist;
						intersected = true;
					}
				}
			}
		}
		else {
			for (GLuint i = 0; i < 8; ++i) {
				if (node->child[i] != nullptr) {
					GLfloat tNearChild = 0, tFarChild = tFar;
					if (node->child[i]->nodeExtents.intersect(precomputedNumerators, precomputedDenominators, &tNearChild, &tFarChild)) {
						GLfloat t = (tNearChild < 0 && tFarChild >= 0) ? tFarChild : tNearChild;
						queue.push(BVH::Octree::QueueElement(node->child[i], t)); // Using our supplied comparator
					}
				}
			}
		}
	}

	return intersected;
}


BVH::Extents BVH::__buildSceneExtents(std::vector<Mesh*>& meshes)
{
	Extents sceneExtents;

	extentsList.reserve(meshes.size());
	for (uint32_t i = 0; i < meshes.size(); ++i) {

		extentsList[i].build(meshes[i]);
		sceneExtents.extendBy(extentsList[i]); // expand the scene extent of this object's extent
	}

	return sceneExtents;
}







/***************************Octree ****************************************/


/******************************
//       Public Methods
/*****************************/

BVH::Octree::Octree(const Extents& sceneExtents)
{
	GLfloat xDiff = sceneExtents.dists[0].dFar - sceneExtents.dists[0].dNear;
	GLfloat yDiff = sceneExtents.dists[1].dFar - sceneExtents.dists[1].dNear;
	GLfloat zDiff = sceneExtents.dists[2].dFar - sceneExtents.dists[2].dNear;
	GLfloat maxDiff = glm::max(xDiff, glm::max(yDiff, zDiff));

	vec3 minPlusMax(sceneExtents.dists[0].dNear + sceneExtents.dists[0].dFar,
					sceneExtents.dists[1].dNear + sceneExtents.dists[1].dFar,
					sceneExtents.dists[2].dNear + sceneExtents.dists[2].dFar);

	bbox.bounds[0] = (minPlusMax - maxDiff) * 0.5f;
	bbox.bounds[1] = (minPlusMax + maxDiff) * 0.5f;

	this->root = new OctreeNode;
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


void BVH::Octree::insert(OctreeNode*& node, const Extents* extents, const AABB& bbox, uint32_t depth)
{
	if (node->isLeaf) {
		if (node->nodeExtentsList.size() == 0 || depth == 16) {
			node->nodeExtentsList.push_back(extents);
		}
		else {
			node->isLeaf = false;
			// Re-insert extents held by this node
			while (!node->nodeExtentsList.empty()) {
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

		AABB childBBox;
		GLuint childIndex = 0;
		computeChildIndexAndBbox(extentsCentroid, nodeCentroid, &childBBox, &childIndex);

		// Create the child node if it doesn't exist yet and then insert the extents in it
		if (node->child[childIndex] == nullptr) {
			node->child[childIndex] = new OctreeNode();
		}
		insert(node->child[childIndex], extents, childBBox, depth + 1);
	}
}


void BVH::Octree::build(OctreeNode*& node, const AABB& bbox)
{
	if (node->isLeaf) {
		for (const auto& e: node->nodeExtentsList) {
			node->nodeExtents.extendBy(*e);
		}
	}
	else {
		for (uint8_t i = 0; i < 8; ++i) {
			if (node->child[i]) {

				vec3 minBound, maxBound;
				vec3 centroid = bbox.centroid();
				// x-axis
				minBound.x = (i & 4) ? centroid.x : bbox.bounds[0].x;
				maxBound.x = (i & 4) ? bbox.bounds[1].x : centroid.x;
				// y-axis
				minBound.y = (i & 2) ? centroid.y : bbox.bounds[0].y;
				maxBound.y = (i & 2) ? bbox.bounds[1].y : centroid.y;
				// z-axis
				minBound.z = (i & 1) ? centroid.z : bbox.bounds[0].z;
				maxBound.z = (i & 1) ? bbox.bounds[1].z : centroid.z;


				AABB childBBox{minBound, maxBound};

				// Inspect child
				build(node->child[i], childBBox);

				// Expand extents with extents of child
				node->nodeExtents.extendBy(node->child[i]->nodeExtents);
			}
		}
	}
}



void BVH::Octree::computeChildIndexAndBbox(const vec3& extentsCentroid,
										   const vec3& nodeCentroid,
										   AABB* childBbox,
										   GLuint* childIndex)
{
	GLuint index = 0;
	vec3 minBound, maxBound;


	// x-axis
	if (extentsCentroid.x > nodeCentroid.x) {
		index = 4;
		minBound.x = nodeCentroid.x;
		maxBound.x = bbox.bounds[1].x;
	}
	else {
		minBound.x = bbox.bounds[0].x;
		maxBound.x = nodeCentroid.x;
	}
	// y-axis
	if (extentsCentroid.y > nodeCentroid.y) {
		index += 2;
		minBound.y = nodeCentroid.y;
		maxBound.y = bbox.bounds[1].y;
	}
	else {
		minBound.y = bbox.bounds[0].y;
		maxBound.y = nodeCentroid.y;
	}
	// z-axis
	if (extentsCentroid.z > nodeCentroid.z) {
		index += 1;
		minBound.z = nodeCentroid.z;
		maxBound.z = bbox.bounds[1].z;
	}
	else {
		minBound.z = bbox.bounds[0].z;
		maxBound.z = nodeCentroid.z;
	}

	*childIndex = index;
	*childBbox = AABB(minBound, maxBound);
}








bool BVH::Extents::intersect(const GLfloat precomputedNumerator[],
							 const GLfloat precomputedDenominator[],
							 GLfloat* tNear,   // tn and tf in this method need to be contained
							 GLfloat* tFar    // within the range [tNear:tFar]
						    ) const
{
	GLfloat final_t_Near = -INFINITY;
	GLfloat final_t_far  = +INFINITY;

	//numRayBoundingVolumeTests++; TODO
	for (uint8_t i = 0; i < NUM_OF_SET_NORMALS; ++i)
	{
		GLfloat tNearExtents = (dists[i].dNear - precomputedNumerator[i]) / precomputedDenominator[i];
		GLfloat tFarExtents = (dists[i].dFar - precomputedNumerator[i]) / precomputedDenominator[i];

		if (precomputedDenominator[i] < 0)
			std::swap(tNearExtents, tFarExtents);

		final_t_Near = glm::max(tNearExtents, final_t_Near);
		final_t_far = glm::min(tFarExtents, final_t_far);

	}

	if (final_t_Near > final_t_far) return false;

	*tNear = final_t_Near;
	*tFar = final_t_far;
	return true;
}


