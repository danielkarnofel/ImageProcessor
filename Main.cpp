#include "ImageData.h"
#include <iostream>
#include <cassert>

//ImageData* xp{ new ImageData() };
//ImageData* xn{ new ImageData() };
//ImageData* yp{ new ImageData() };
//ImageData* yn{ new ImageData() };
//ImageData* zp{ new ImageData() };
//ImageData* zn{ new ImageData() };

ImageData* img1{ new ImageData() };
ImageData* img2{ new ImageData() };
ImageData* img3{ new ImageData() };
ImageData* img4{ new ImageData() };
ImageData* img5{ new ImageData() };
ImageData* img6{ new ImageData() };

const char* inputFile1{ "input/combined.jpg" };

int main()
{

	img1->loadImage(inputFile1);
	img2->loadImage(inputFile1);
	img3->loadImage(inputFile1);
	img4->loadImage(inputFile1);
	img5->loadImage(inputFile1);
	img6->loadImage(inputFile1);
	
	img1->crop(768, 384, 384, 384); // +X (right)
	img2->crop(0, 384, 384, 384); // -X (left)
	img3->crop(384, 0, 384, 384); // +Y (top)
	img4->crop(384, 768, 384, 384); // -Y (bottom)
	img5->crop(384, 384, 384, 384); // +Z (front)
	img6->crop(1152, 384, 384, 384); // -Z (back)

	img1->resize(512, 512); // +X (right)
	img2->resize(512, 512); // +X (right)
	img3->resize(512, 512); // +X (right)
	img4->resize(512, 512); // +X (right)
	img5->resize(512, 512); // +X (right)
	img6->resize(512, 512); // +X (right)

	img1->saveImage("output/px.jpg", ImageFormat::JPG);
	img2->saveImage("output/nx.jpg", ImageFormat::JPG);
	img3->saveImage("output/py.jpg", ImageFormat::JPG);
	img4->saveImage("output/ny.jpg", ImageFormat::JPG);
	img5->saveImage("output/pz.jpg", ImageFormat::JPG);
	img6->saveImage("output/nz.jpg", ImageFormat::JPG);

	return 0;
}