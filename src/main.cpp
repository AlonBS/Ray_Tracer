//
//
#include <iostream>
#include <FreeImage.h>
#include <fstream>
#include <omp.h>
#include <sys/time.h>
#include <iomanip>

#include "Camera.hpp"
#include "Object.h"
#include "SceneParser.h"
#include "Image.h"
#include "RayTracer.h"

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#define FILES_EXTENSION ".rt"
#define DEFUALT_RESULT_FORMAT ".png"

#define CURRENT_VERSION "1.5"

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std;

namespace fs = ::boost::filesystem;
namespace po = ::boost::program_options;

bool singleThreaded = false;
string resultFormat;
fs::path outputDirectory;


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


static void parse_args(int argc, char *argv[], vector<fs::path>& scenes)
{
	try {
		// Declare the supported options.
		po::options_description desc("Ray-Tracer parameters");
		desc.add_options()
			("help,h", "show this message")
			("version,v", "Current Version of the program")
			("input,i", po::value<string>(), "The path to a scene file, or scenes directory following the syntax specific within README file. Applies only to direct directory. No recursive sub-directories search is performed.")
			("output,o", po::value<string>(), "Optional - The directory where to save the results. If not specified, results will be saved in the same directory of input files.")
			("format", po::value<string>()->default_value(DEFUALT_RESULT_FORMAT), "Optional - The file format in which scenes result will be saved. Currently supported types are: png, jpeg, jpg, bmp, tiff.")
			("single-thread,s", po::bool_switch(&singleThreaded), "Flag to force single thread rendering. Default behavior is multi-threaded.")
		;


		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			exit(0);
		}
		if (vm.count("version")) {
			cout << "Version: " << CURRENT_VERSION << std::endl;
			exit(0);
		}

		if (vm.count("input") == 0) {
			cout << "No input scene or scenes directory was given." << endl;
			std::cout << "Try --help for usage" << std::endl;
			exit(-1);
		}

		fs::path input{vm["input"].as<string>()};
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

		if (vm.count("output") == 0) {
			outputDirectory = input.parent_path();
		}
		else {
			outputDirectory = vm["output"].as<string>();
		}

		if (!fs::exists(outputDirectory)) {
			if (!fs::create_directory(outputDirectory)) {

				std::cout << "Could not create directory for the results." << std::endl;
				exit(-1);
			}
		}


		if (vm.count("format")) {
			string form = vm["format"].as<string>();
			// For the incompetent user
			if (form[0] == '.') {
				resultFormat = form;
			}
			else {
				resultFormat = "." + form;
			}
		}
	}

	catch (boost::program_options::unknown_option& e) {
		std::cerr << e.what() << std::endl;
		std::cout << "Try --help for usage" << std::endl;
		exit(-1);
	}

	catch (boost::program_options::invalid_command_line_syntax& e) {

		std::cout << "Invalid Command line syntax" << std::endl;
		std::cout << e.what() << std::endl;
		exit(-3);
	}
}



static void render_scene(string fileName)
{

	Scene *scene = SceneParser::readFile(fileName.c_str());
	if (!scene) {
		cerr << "Scene: " << fileName << " was not rendered." << endl;
		return;
	}

	RayTracer rayTracer;
	Image *img = nullptr;

	if (singleThreaded) {
		img = rayTracer.rayTraceST(*scene);
	}
	else {
		img = rayTracer.rayTraceMT(*scene);
	}


	string output = outputDirectory.generic_string() + "/"	+
					fs::path{fileName}.stem().generic_string() +
					"_result" + resultFormat;

	img->saveImage(output);

	delete img;
	delete scene;
}


int main(int argc, char *argv[])
{

	vector<fs::path> files{};
	parse_args(argc, argv, files);

	struct timeval start, end;
	GLdouble time, totalTime;

	FreeImage_Initialise();

	cout << "Ray Tracer working:" << endl;
	cout << "=====================\n" << endl;


	for (fs::path p : files) {

		gettimeofday(&start, NULL);

		{
			std::cout << "\tRendering " << p.filename() << "..." << endl;
			render_scene(p.generic_string());
			std::cout << "\tFinished rendering " << p.filename() << "..." << endl;
		}

		gettimeofday(&end, NULL);

		time = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
		cout << "\tTime took: " << setprecision(1) << time  <<  " seconds. \n" << endl;
		totalTime += time;

	}

	GLuint minutes = (GLuint)(totalTime / 60);
	GLuint seconds = (GLuint)totalTime % 60;
	cout << "Finished working.\nTotal Time: " << minutes << " minutes and " << seconds << " seconds." << endl;

	std::cout << "Results are saved at: " << outputDirectory << std::endl;

	FreeImage_DeInitialise();
	return 0;
}






