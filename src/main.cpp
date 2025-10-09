#include "ImageData.h"
#include <filesystem>

ImageData* img1{ new ImageData() };
ImageData* img2{ new ImageData() };

std::string inputFile1 = std::string(PROJECT_SOURCE_DIR) + "/input/dog.jpg";
std::string outputFile1 = std::string(PROJECT_SOURCE_DIR) + "/output/dog.jpg";

std::string inputFile2 = std::string(PROJECT_SOURCE_DIR) + "/input/cat.jpg";
std::string outputFile2 = std::string(PROJECT_SOURCE_DIR) + "/output/cat.jpg";

int main()
{

	img1->loadImage(inputFile1.c_str());
	img1->invert();
	img1->saveImage(outputFile1.c_str(), ImageFormat::JPG);

	img2->loadImage(inputFile2.c_str());
	img2->threshold();
	img2->saveImage(outputFile2.c_str(), ImageFormat::JPG);

	return 0;
}