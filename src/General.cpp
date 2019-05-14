

#include "General.h"




bool generateStats = false;
RayTracerStats rayTracerStats;

void updateStats(StatsType st)
{
	if (!generateStats) return;

	switch (st)
	{

		case INCREMENT_RAY_COUNT:
			++rayTracerStats.numOfRays;
			break;

		case INCREMENT_INTERSECTION_TESTS_COUNT:
			++rayTracerStats.numOfIntersectTests;
			break;

		case INCREMENT_HITS_COUNT:
			++rayTracerStats.numOfHits;
			break;

	}

}


RayTracerStats& getStats()
{
	return rayTracerStats;
}

void clearStats()
{
	rayTracerStats.numOfRays = {0};
	rayTracerStats.numOfIntersectTests = {0};
	rayTracerStats.numOfHits = {0};

}


//bool faceNormals = false;


