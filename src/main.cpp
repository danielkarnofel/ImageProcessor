#include "ImageData.h"
#include <filesystem>

ImageData* img{ new ImageData() };

std::string inputFile = std::string(PROJECT_SOURCE_DIR) + "/input/dog.jpg";
std::string outputFile = std::string(PROJECT_SOURCE_DIR) + "/output/dog.jpg";

int main()
{

	img->loadImage(inputFile.c_str());
	
	img->invert();

	img->saveImage(outputFile.c_str(), ImageFormat::JPG);

	return 0;
}