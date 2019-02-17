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
#define FILES_EXTENSION ".rt"

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std;

namespace fs = ::boost::filesystem;
namespace po = ::boost::program_options;

static void print_usage()
{
	std::cout << "Usage: ray_tracer <input-file | input-dir> [output-dir]" << std::endl;
	std::cout << "Where:" << std::endl;
	std::cout << "\t<input-file | input-dir> - Is the path to a scene file, or scenes directory following the syntax specific within README file" << std::endl;
	std::cout << "\t[output-dir] - Optional - is the directory where to save the results. If not specified, results will be saved in the same directory of input files." << std::endl;

}

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


static void parse_args(int argc, char *argv[], vector<fs::path>& scenes, string& output_dir)
{
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "show this message")
		("version,v", "Current Version of the program")
	    ("input", po::value<string>(), "The path to a scene file, or scenes directory following the syntax specific within README file")
		("output", po::value<string>(), "The directory where to save the results. If not specified, results will be saved in the same directory of input files.")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help")) {
	    cout << desc << "\n";
	    exit(1);
	}

	std::cout << vm["input"].as<string>() << std::endl;
	std::cout << vm["output"].as<string>() << std::endl;

	exit(0);

	if (vm.count("compression")) {
	    cout << "Compression level was set to "	 << vm["compression"].as<int>() << ".\n";
	} else {
	    cout << "Compression level was not set.\n";
	}

}


static void parse_args2(int argc, char *argv[], vector<fs::path>& scenes, string& output_dir)
{
	if (argc <= 1) {
		std::cout << "Wrong number of arguments.\n" << std::endl;
		print_usage();
		exit(-1);
	}

	fs::path input{argv[1]};
	if (!fs::exists(input)) {
		std::cout << "No such file or directory: " << input << std::endl;
		exit(-1);
	}
	if (fs::is_directory(input)) {
		string ext = FILES_EXTENSION;
		get_all_scenes(input, ext, scenes);
	}
	else {
		scenes.push_back(input);
	}

	fs::path output;
	if (argc <= 2) { // No output directory
		output = input.parent_path();
	}
	else { // output directory given -
		output = argv[2];
	}


	if (!fs::exists(output)) {
		if (!fs::create_directory(output)) {

			std::cout << "Could not create directory for the results." << std::endl;
			exit(-1);
		}
	}

	std::cout << "Results are saved at: " << output << std::endl;


}



static void render_scene(string &fileName, string& outputDirectory)
{

	RenderInfo *renderInfo = SceneParser::readFile(fileName.c_str());

	RayTracer rayTracer;
	Image *img = rayTracer.rayTrace(fileName, renderInfo->camera, renderInfo->scene, renderInfo->width, renderInfo->height, renderInfo->maxDepth);

	fs::path p = fileName;

	string output = outputDirectory + "/" + fileName;
//	if (renderInfo->outputFile.empty()) {
//		// No output file was given
//		output = fileName + "_result.png";
//	}
//	else {
//		output = p.parent_path().generic_string() + "/" + renderInfo->outputFile;
//	}

	img->saveImage(output);

	delete img;
	delete renderInfo;
}


int main(int argc, char *argv[])
{

	vector<fs::path> files;
	string outputDirectory;
	parse_args(argc, argv, files, outputDirectory);


	struct timeval start, end;
	GLdouble time, totalTime;


	FreeImage_Initialise();

	//fs::path directory = "./scenes/b";
	//string ext = ".test";

	//get_all_scenes(directory, ext, files);

	string fileName;

	cout << "Ray Tracer working..." << endl;


	for (fs::path p : files) {

		gettimeofday(&start, NULL);


		std::cout << "\tRendering " << p.filename() << "..." << endl;
		string file = p.generic_string();
		render_scene(file, outputDirectory);

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




