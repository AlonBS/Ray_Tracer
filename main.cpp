//
//
#include <iostream>
#include <FreeImage.h>
#include <fstream>
#include <omp.h>
#include <sys/time.h>
#include <iomanip>

#include "Object.h"
#include "SceneParser.h"
#include "Camera.h"
#include "RenderInfo.h"
#include "Image.h"
#include "RayTracer.h"

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/filesystem.hpp>

using namespace std;

namespace fs = ::boost::filesystem;

// return the filenames of all files that have the specified extension
static void get_all_scenes(const fs::path& root, const string& ext, vector<fs::path>& ret)
{
    if(!fs::exists(root) || !fs::is_directory(root)) return;

    fs::directory_iterator it(root);
    fs::directory_iterator endit;
    //fs::recursive_directory_iterator it(root);
    //fs::recursive_directory_iterator endit;

    while(it != endit)
    {
        if(fs::is_regular_file(*it) && it->path().extension() == ext)
        	ret.push_back(it->path());
        ++it;
    }

}

static void render_scene(string &fileName)
{

	RenderInfo *renderInfo = SceneParser::readFile(fileName.c_str());

	RayTracer rayTracer;
	Image *img = rayTracer.rayTrace(fileName, renderInfo->camera, renderInfo->scene, renderInfo->width, renderInfo->height, renderInfo->maxDepth);

	fs::path p = fileName;

	string output;
	if (renderInfo->outputFile.empty()) {
		// No output file was given
		output = fileName + "_result.png";
	}
	else {
		output = p.parent_path().generic_string() + "/" + renderInfo->outputFile;
	}

	img->saveImage(output);

	delete img;
	delete renderInfo;
}

#include <future>
#include <thread>
#include <chrono>
#include <vector>
//
//int main()
//{
//	vector<future<int>> future_vector;
//
//	future_vector.push_back(
//			std::async(std::launch::async, []()
//			{
//        		std::this_thread::sleep_for(std::chrono::seconds(3));
//        		return 3;
//			}
//	));
//
//	future_vector.push_back(
//			std::async(std::launch::async, []()
//			{
//				std::this_thread::sleep_for(std::chrono::seconds(5));
//				return 5;
//			}
//	));
//
////    std::cout << "waiting...\n";
////    std::future_status status;
//    while (!future_vector.empty()) {
////
//    	std::future<int>& future = future_vector.pop();
////
////    	do {
////    		status = future.wait_for(std::chrono::seconds(1));
////    		if (status == std::future_status::deferred) {
////    			std::cout << "deferred\n";
////    		} else if (status == std::future_status::timeout) {
////    			std::cout << "timeout\n";
////    		} else if (status == std::future_status::ready) {
////    			std::cout << "ready!\n";
////    		}
////    	} while (status != std::future_status::ready);
//    }
//
//
//    std::cout << "result is " << future.get() << '\n';
//}
//


int main()
{

//	size_t max = 10000;


//	for (int i = 0 ; i < max ; ++i) {
//
//		for (int j = 0 ; j < 1000 ; ++j) {
//
//			std::cout << "Progress: [ "<< setprecision(1) << fixed << (i / (GLfloat)max) * 100.0 << "% ] \r";
//			std::cout.flush();
//
//		}
//
//
//
//	}
//
//	return 0;





	struct timeval start, end;
	GLdouble time, totalTime;


	FreeImage_Initialise();

	fs::path directory = "./scenes/b";
	string ext = ".test";
	vector<fs::path> files;
	get_all_scenes(directory, ext, files);

	Image *img = nullptr;
	string fileName;

	cout << "Ray Tracer working..." << endl;

	int tid;

	for (fs::path p : files) {

		gettimeofday(&start, NULL);


		std::cout << "\tRendering " << p.filename() << "..." << endl;
		string file = p.generic_string();
		render_scene(file);

		std::cout << "\tFinished rendering " << p.filename() << "..." << endl;

		gettimeofday(&end, NULL);

		time = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		cout << "\tTime took: " << setprecision(1) << time  <<  " seconds. \n" << endl;
		totalTime += time;

	}

	GLuint minutes = (GLuint)(totalTime / 60);
	GLuint seconds = (GLuint)totalTime % 60;
	cout << "Finished working.\nTotal Time: " << minutes << " minutes and " << seconds << " seconds." << endl;

	FreeImage_DeInitialise();
	return 0;
}




