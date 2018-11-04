/*
 * Image.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */


#include <cstdio>
#include <iostream>
#include <cassert>
#include "Image.h"
#include "General.h"
#include "glm/glm.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

#define RGBSIZE 24


glm::vec3 Image::clamp (glm::vec3& v)
{
	glm::vec3 clampedV = v;

	(clampedV.r > 1.0f) ? clampedV.r = 1.0f : (clampedV.r < 0.0f) ? clampedV.r = 0.0f : clampedV.r;
	(clampedV.g > 1.0f) ? clampedV.g = 1.0f : (clampedV.g < 0.0f) ? clampedV.g = 0.0f : clampedV.g;
	(clampedV.b > 1.0f) ? clampedV.b = 1.0f : (clampedV.b < 0.0f) ? clampedV.b = 0.0f : clampedV.b;

	return clampedV;
}


Image::Image(int width, int height)
:width(width), height(height)
{
	this->bitmap = FreeImage_Allocate(width, height, RGBSIZE);
}



Image::~Image() {

	FreeImage_Unload(this->bitmap);
}

void Image::setPixel(int col, int row, glm::vec3& rgbColors)
{
///	assert(row >= 0 && row < height);
///	assert(col >= 0 && col < width);

	RGBQUAD color;

	glm::vec3 clampedValue = this->clamp(rgbColors);
	color.rgbRed = clampedValue.r * 255;
	color.rgbGreen = clampedValue.g * 255;
	color.rgbBlue = clampedValue.b * 255;

	//FreeImage_SetPixelColor(this->bitmap, col, row, &color);
	FreeImage_SetPixelColor(this->bitmap, col, height - row, &color);
}

vec3 Image::getPixel(int col, int row)
{
	vec3 color;
	RGBQUAD pixel;

	FreeImage_GetPixelColor(this->bitmap, col, row, &pixel);
	color.r = pixel.rgbRed / 255.0f;
	color.g = pixel.rgbGreen / 255.0f;
	color.b = pixel.rgbBlue / 255.0f;

	return this->clamp(color);
}


void Image::saveImage(std::string& fileName) const
{
	string extension = boost::filesystem::extension(fileName);
	if ( boost::iequals(extension, ".jpg") || boost::iequals(extension, ".jpeg") ) {

		FreeImage_Save(FIF_JPEG, bitmap, fileName.c_str(), 0);
	}
	else if ( boost::iequals(extension, ".png") ) {

		FreeImage_Save(FIF_PNG, bitmap, fileName.c_str(), 0);
	}

	std::cout << "\tImage saved to: " << fileName << std::endl;
}

void
Image::loadImage(std::string& fileName)
{
	this->loadImage(fileName.c_str());
}


// Method to load an image into a texture using the freeimageplus library. Returns the texture ID or dies trying.
void Image::loadImage(const char* filename)
{

    // Determine the format of the image.
    // Note: The second paramter ('size') is currently unused, and we should use 0 for it.
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename , 0);

    // Image not found? Abort! Without this section we get a 0 by 0 image with 0 bits-per-pixel but we don't abort, which
    // you might find preferable to dumping the user back to the desktop.
    if (format == -1)
    {
        cout << "Could not find image: " << filename << " - Aborting." << endl;
    }

    // Found image, but couldn't determine the file format? Try again...
    if (format == FIF_UNKNOWN)
    {
        cout << "Couldn't determine file format - attempting to get from file extension..." << endl;
        format = FreeImage_GetFIFFromFilename(filename);
        // Check that the plugin has reading capabilities for this format (if it's FIF_UNKNOWN,
        // for example, then it won't have) - if we can't read the file, then we bail out =(
        if ( !FreeImage_FIFSupportsReading(format) )
        {
            cout << "Detected image format cannot be read!" << endl;
            exit(-1);
        }
    }

    // If we're here we have a known image format, so load the image into a bitmap
    FIBITMAP* tmpBitmap = FreeImage_Load(format, filename);
    int bitsPerPixel =  FreeImage_GetBPP(tmpBitmap);

    if (bitsPerPixel == 32)
    {
    	this->bitmap = tmpBitmap;
    }
    else
    {
    	this->bitmap = FreeImage_ConvertTo32Bits(tmpBitmap);
    }

    // Some basic image info - strip it out if you don't care
    this->width  = FreeImage_GetWidth(this->bitmap);
    this->height = FreeImage_GetHeight(this->bitmap);

    // Unload the 32-bit colour bitmap
    if (bitsPerPixel != 32)
    {
        FreeImage_Unload(tmpBitmap);
    }
}

