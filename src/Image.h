/*
 * Image.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <cstdio>
#include <iostream>
#include <FreeImage.h>
#include <cassert>

#include <glm/glm.hpp>

class Pixel {

public:

	BYTE r;
	BYTE g;
	BYTE b;

	Pixel() : r(0), g(0), b(0) {}

	Pixel(BYTE r, BYTE g, BYTE b) : r(r), g(g), b(b) {}

	inline friend std::ostream& operator<< (std::ostream &out, const Pixel &pixel);

};

inline std::ostream& operator<< (std::ostream &out, const Pixel &pixel)
{
	out << "(" << pixel.r << "," << pixel.g << "," << pixel.b << ")";
	return out;
}


class Image {
private:


	int width;
	int height;

	FIBITMAP* bitmap;

public:


	Image (int width, int height);


	~Image();

	void setPixel(int col, int row, glm::vec3& rgbColors);

	glm::vec3 getPixel(int col, int row);
	glm::vec3 getPixel(int col, int row) const;

	int getWidth() { return this->width; }
	int getHeight() { return this->height; }

	int getWidth() const { return this->width; }
	int getHeight() const { return this->height; }


	void saveImage(std::string& fileName) const;
	void loadImage(std::string& fileName);
	void loadImage(const char * fileName);


};


#endif /* IMAGE_H_ */
