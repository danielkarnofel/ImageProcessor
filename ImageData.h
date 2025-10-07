#pragma once
#pragma warning(push)
#pragma warning(disable : 4365 4820 5219 4388 4514 5045)

#include "stb_image.h"
#include "stb_image_write.h"

#include <vector>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <iostream>

enum class ImageFormat { PNG, JPG, BMP };
enum class ColorChannel { Invalid = 0, G = 1, GA = 2, RGB = 3, RGBA = 4 };

// Pixel structs will always have RGBA (4) channels, regardless of image input/output formats
constexpr int pixelChannels = static_cast<int>(ColorChannel::RGBA);
struct Pixel
{
	unsigned char r{};
	unsigned char g{};
	unsigned char b{};
	unsigned char a{};
};

template <typename T>
using Vector2D = std::vector<std::vector<T>>;

class ImageData
{

private:
	int width{};
	int height{};
	int channels{};
	Vector2D<Pixel> pixels{};

public:
	ImageData() = default;
	~ImageData() = default;

	// Image IO functions
	// --------------------------------------------------------------------------------

	void loadImage(const char* inputFile);

	void saveImage(const char* outputFile, ImageFormat format, int quality = 90);


	// Getters/Setters
	// --------------------------------------------------------------------------------

	int getWidth() const { return width; }
	
	int getHeight() const { return height; }
	
	Pixel& getPixel(int y, int x) { return pixels[y][x]; }
	
	const Pixel& getPixel(int y, int x) const { return pixels[y][x]; }

	void setPixel(int y, int x, const Pixel& pixel) { pixels[y][x] = pixel; }


	// Monochrome and Tone
	// --------------------------------------------------------------------------------

	void grayscale();

	void threshold(unsigned char thresholdValue = 128);

	void invert();


	// Color Adjustments
	// --------------------------------------------------------------------------------

	void brightness(int offset);

	void contrast(float factor);

	void tint(const Pixel& color, float strength);

	void noise(float intensity);


	// Geometry
	// --------------------------------------------------------------------------------

	void flipH();

	void flipV();

	void rotateR();

	void rotateL();

	void resize(int w, int h);

	void crop(int x, int y, int w, int h);


	// Compositing
	// --------------------------------------------------------------------------------

	void blend(const ImageData& other, float alpha);

	void compositeOver(const ImageData& background);

	void applyAlphaMask(const ImageData& mask);

	void multiply(const ImageData& other);

	void screen(const ImageData& other);

	void overlay(const ImageData& other);

	void darken(const ImageData& other);

	void lighten(const ImageData& other);

	void add(const ImageData& other, float scale = 1.0f);

	void subtract(const ImageData& other);

	void difference(const ImageData& other);

	void average(const ImageData& other);

	void max(const ImageData& other);

	void min(const ImageData& other);


	// Convolution Kernels
	// --------------------------------------------------------------------------------

	template<size_t N>
	using Kernel = std::array<std::array<float, N>, N>;

	template<size_t N>
	void applyKernel(const Kernel<N>& kernel);

	/*template<size_t N>
	constexpr Kernel<N> scaleKernel(const Kernel<N>& kernel, float scale);

	template<size_t N>
	constexpr Kernel<N> normalizeKernel(const Kernel<N>& kernel);

	template<size_t N, size_t M>
	constexpr Kernel<N + M - 1> convolveKernels(const Kernel<N>& k1, const Kernel<M>& k2);*/

	// Predefined kernels (3x3)
	enum class KernelType {
		Default,
		BoxBlur,
		GaussianBlur,
		SobelX,
		SobelY,
		Laplacian,
		Sharpen,
		Emboss
	};
	constexpr Kernel<3> getKernel(KernelType type);
	constexpr const char* toString(KernelType type);

	/*constexpr Kernel<3> getDefaultKernel();
	constexpr Kernel<3> getBoxBlurKernel();
	constexpr Kernel<3> getGaussianBlurKernel();
	constexpr Kernel<3> getSobelXKernel();
	constexpr Kernel<3> getSobelYKernel();
	constexpr Kernel<3> getLaplacianKernel();
	constexpr Kernel<3> getSharpenKernel();
	constexpr Kernel<3> getEmbossKernel();*/

	// Custom kernels

	using DynamicKernel = std::vector<std::vector<float>>;

	void applyKernel(const DynamicKernel& kernel);

	DynamicKernel scaleKernel(const DynamicKernel& kernel, float scale);

	DynamicKernel normalizeKernel(const DynamicKernel& kernel);

	DynamicKernel createCustomKernel(const std::vector<std::vector<float>>& values);

	DynamicKernel convolveKernels(const DynamicKernel& k1, const DynamicKernel& k2);
};

// Image IO Functions
// ------------------------------------------------------------------------------------

void ImageData::loadImage(const char* inputFile)
{
	unsigned char* data = stbi_load(inputFile, &width, &height, &channels, pixelChannels);
	if (!data)
	{
		throw std::runtime_error("Failed to load image");
	}
	pixels.resize(height, std::vector<Pixel>(width));
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = (y * width + x) * pixelChannels;
			pixels[y][x] = { data[index], data[index + 1], data[index + 2], data[index + 3] };
		}
	}
	stbi_image_free(data);
}

constexpr ColorChannel channelCount(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::PNG:
		return ColorChannel::RGBA;
	case ImageFormat::JPG:
		return ColorChannel::RGB;
	case ImageFormat::BMP:
		return ColorChannel::RGB;
	default:
		return ColorChannel::Invalid;
	}
}

std::vector<unsigned char> packPixelData(const Vector2D<Pixel>& pixels, int width, int height, ImageFormat format)
{
	ColorChannel colorChannel = channelCount(format);
	std::vector<unsigned char> rawData;
	rawData.reserve(width * height * static_cast<int>(colorChannel));

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const Pixel& pixel = pixels[y][x];
			if (colorChannel == ColorChannel::RGB) {
				rawData.push_back(pixel.r);
				rawData.push_back(pixel.g);
				rawData.push_back(pixel.b);
			}
			if (colorChannel == ColorChannel::RGBA) {
				rawData.push_back(pixel.r);
				rawData.push_back(pixel.g);
				rawData.push_back(pixel.b);
				rawData.push_back(pixel.a);
			}
		}
	}
	return rawData;
}

void ImageData::saveImage(const char* outputFile, ImageFormat format, int quality)
{
	int outputChannels = static_cast<int>(channelCount(format));
	auto rawData = packPixelData(pixels, width, height, format);

	int stride = width * pixelChannels;

	bool success = false;
	switch (format)
	{
	case ImageFormat::PNG:
		std::cout << "Saving PNG image...\n";
		success = static_cast<bool>(stbi_write_png(outputFile, width, height, outputChannels, rawData.data(), stride));
		break;
	case ImageFormat::JPG:
		std::cout << "Saving JPG image...\n";
		success = static_cast<bool>(stbi_write_jpg(outputFile, width, height, outputChannels, rawData.data(), quality));
		break;
	case ImageFormat::BMP:
		std::cout << "Saving BMP image...\n";
		success = static_cast<bool>(stbi_write_bmp(outputFile, width, height, outputChannels, rawData.data()));
		break;
	}

	if (!success) {
		throw std::runtime_error("Failed to save image");
	}
}

// Monochrome and Tone functions
// ------------------------------------------------------------------------------------

void ImageData::grayscale()
{
	unsigned char grayValue{};
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			grayValue = static_cast<unsigned char>(0.299f * pixel.r + 0.587f * pixel.g + 0.114f * pixel.b);
			pixel.r = grayValue;
			pixel.g = grayValue;
			pixel.b = grayValue;
		}
	}
}

void ImageData::threshold(unsigned char thresholdValue)
{
	unsigned char grayValue{};
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			grayValue = static_cast<unsigned char>(0.299f * pixel.r + 0.587f * pixel.g + 0.114f * pixel.b);
			if (grayValue < thresholdValue)
			{
				pixel.r = pixel.g = pixel.b = 0;
			}
			else
			{
				pixel.r = pixel.g = pixel.b = 255;
			}
		}
	}
}

void ImageData::invert()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			pixel.r = 255 - pixel.r;
			pixel.g = 255 - pixel.g;
			pixel.b = 255 - pixel.b;
		}
	}
}

// Color Adjustment Functions
// ------------------------------------------------------------------------------------

void ImageData::brightness(int offset)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			pixel.r = static_cast<unsigned char>(std::clamp(pixel.r + offset, 0, 255));
			pixel.g = static_cast<unsigned char>(std::clamp(pixel.g + offset, 0, 255));
			pixel.b = static_cast<unsigned char>(std::clamp(pixel.b + offset, 0, 255));
		}
	}
}

void ImageData::contrast(float factor)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			pixel.r = static_cast<unsigned char>(std::clamp(static_cast<int>(pixel.r * factor), 0, 255));
			pixel.g = static_cast<unsigned char>(std::clamp(static_cast<int>(pixel.g * factor), 0, 255));
			pixel.b = static_cast<unsigned char>(std::clamp(static_cast<int>(pixel.b * factor), 0, 255));
		}
	}
}

void ImageData::tint(const Pixel& color, float strength)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			pixel.r = static_cast<unsigned char>(pixel.r * (1 - strength) + color.r * strength);
			pixel.g = static_cast<unsigned char>(pixel.g * (1 - strength) + color.g * strength);
			pixel.b = static_cast<unsigned char>(pixel.b * (1 - strength) + color.b * strength);
		}
	}
}

void ImageData::noise(float intensity)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			pixel.r = static_cast<unsigned char>(std::clamp(static_cast<int>(pixel.r + (rand() % 256 - 128) * intensity), 0, 255));
			pixel.g = static_cast<unsigned char>(std::clamp(static_cast<int>(pixel.g + (rand() % 256 - 128) * intensity), 0, 255));
			pixel.b = static_cast<unsigned char>(std::clamp(static_cast<int>(pixel.b + (rand() % 256 - 128) * intensity), 0, 255));
		}
	}
}

// Geomoetry Functions
// ------------------------------------------------------------------------------------

void ImageData::flipH()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width / 2; ++x)
		{
			std::swap(pixels[y][x], pixels[y][width - x - 1]);
		}
	}
}

void ImageData::flipV()
{
	for (int y = 0; y < height / 2; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			std::swap(pixels[y][x], pixels[height - y - 1][x]);
		}
	}
}

void ImageData::rotateR()
{
	Vector2D<Pixel> rotatedPixels(width, std::vector<Pixel>(height));
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			rotatedPixels[x][height - y - 1] = pixels[y][x];
		}
	}
	pixels = std::move(rotatedPixels);
	std::swap(width, height);
}

void ImageData::rotateL()
{
	Vector2D<Pixel> rotatedPixels(width, std::vector<Pixel>(height));
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			rotatedPixels[width - x - 1][y] = pixels[y][x];
		}
	}
	pixels = std::move(rotatedPixels);
	std::swap(width, height);
}

constexpr int maxSize{ 4096 };
void ImageData::resize(int w, int h)
{
	if (w <= 0 || w > maxSize || h <= 0 || h > maxSize) {
		throw std::invalid_argument("Invalid input");
	}
	Vector2D<Pixel> resizedPixels(h, std::vector<Pixel>(w));
	float xRatio = static_cast<float>(width) / w;
	float yRatio = static_cast<float>(height) / h;
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int srcX = static_cast<int>(x * xRatio);
			int srcY = static_cast<int>(y * yRatio);
			resizedPixels[y][x] = pixels[srcY][srcX];
		}
	}
	pixels = std::move(resizedPixels);
	width = w;
	height = h;
}

void ImageData::crop(int x, int y, int w, int h)
{
	if (x < 0 || x > width) {
		throw std::invalid_argument("Invalid input");
	}
	if (y < 0 || y > height) {
		throw std::invalid_argument("Invalid input");
	}
	if (x + w > width) {
		w = width - x;
	}
	if (y + h > height) {
		h = height - y;
	}
	Vector2D<Pixel> croppedPixels(h, std::vector<Pixel>(w));
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			croppedPixels[i][j] = pixels[y + i][x + j];
		}
	}
	pixels = std::move(croppedPixels);
	width = w;
	height = h;
}

// Compositing Functions
// ------------------------------------------------------------------------------------

void ImageData::blend(const ImageData& other, float alpha)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to blend");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pixels[y][x].r = static_cast<unsigned char>(alpha * pixels[y][x].r + (1 - alpha) * other.pixels[y][x].r);
			pixels[y][x].g = static_cast<unsigned char>(alpha * pixels[y][x].g + (1 - alpha) * other.pixels[y][x].g);
			pixels[y][x].b = static_cast<unsigned char>(alpha * pixels[y][x].b + (1 - alpha) * other.pixels[y][x].b);
		}
	}
}

void ImageData::compositeOver(const ImageData& background)
{
	if (width != background.width || height != background.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to composite");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& bgPixel = background.pixels[y][x];
			pixel.r = static_cast<unsigned char>(pixel.r * pixel.a + bgPixel.r * (1 - pixel.a));
			pixel.g = static_cast<unsigned char>(pixel.g * pixel.a + bgPixel.g * (1 - pixel.a));
			pixel.b = static_cast<unsigned char>(pixel.b * pixel.a + bgPixel.b * (1 - pixel.a));
		}
	}
}

void ImageData::applyAlphaMask(const ImageData& mask)
{
	if (width != mask.width || height != mask.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to apply alpha mask");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& maskPixel = mask.pixels[y][x];
			pixel.a = static_cast<unsigned char>(maskPixel.r);
		}
	}
}

void ImageData::multiply(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to multiply");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = static_cast<unsigned char>(pixel.r * otherPixel.r / 255);
			pixel.g = static_cast<unsigned char>(pixel.g * otherPixel.g / 255);
			pixel.b = static_cast<unsigned char>(pixel.b * otherPixel.b / 255);
		}
	}
}

void ImageData::screen(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to screen");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = static_cast<unsigned char>(255 - ((255 - pixel.r) * (255 - otherPixel.r)) / 255);
			pixel.g = static_cast<unsigned char>(255 - ((255 - pixel.g) * (255 - otherPixel.g)) / 255);
			pixel.b = static_cast<unsigned char>(255 - ((255 - pixel.b) * (255 - otherPixel.b)) / 255);
		}
	}
}

void ImageData::overlay(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to overlay");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			if (pixel.r < 128)
			{
				pixel.r = static_cast<unsigned char>(pixel.r * otherPixel.r / 255);
			}
			else
			{
				pixel.r = static_cast<unsigned char>(255 - ((255 - pixel.r) * (255 - otherPixel.r)) / 255);
			}
			if (pixel.g < 128)
			{
				pixel.g = static_cast<unsigned char>(pixel.g * otherPixel.g / 255);
			}
			else
			{
				pixel.g = static_cast<unsigned char>(255 - ((255 - pixel.g) * (255 - otherPixel.g)) / 255);
			}
			if (pixel.b < 128)
			{
				pixel.b = static_cast<unsigned char>(pixel.b * otherPixel.b / 255);
			}
			else
			{
				pixel.b = static_cast<unsigned char>(255 - ((255 - pixel.b) * (255 - otherPixel.b)) / 255);
			}
		}
	}
}

void ImageData::darken(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to darken");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = std::min(pixel.r, otherPixel.r);
			pixel.g = std::min(pixel.g, otherPixel.g);
			pixel.b = std::min(pixel.b, otherPixel.b);
		}
	}
}

void ImageData::lighten(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to lighten");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = std::max(pixel.r, otherPixel.r);
			pixel.g = std::max(pixel.g, otherPixel.g);
			pixel.b = std::max(pixel.b, otherPixel.b);
		}
	}
}

void ImageData::add(const ImageData& other, float scale)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to add");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = static_cast<unsigned char>(std::clamp(pixel.r + scale * otherPixel.r, 0.0f, 255.0f));
			pixel.g = static_cast<unsigned char>(std::clamp(pixel.g + scale * otherPixel.g, 0.0f, 255.0f));
			pixel.b = static_cast<unsigned char>(std::clamp(pixel.b + scale * otherPixel.b, 0.0f, 255.0f));
		}
	}
}

void ImageData::subtract(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to subtract");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = static_cast<unsigned char>(std::clamp(pixel.r - otherPixel.r, 0, 255));
			pixel.g = static_cast<unsigned char>(std::clamp(pixel.g - otherPixel.g, 0, 255));
			pixel.b = static_cast<unsigned char>(std::clamp(pixel.b - otherPixel.b, 0, 255));
		}
	}
}

void ImageData::difference(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to find difference");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = static_cast<unsigned char>(std::abs(pixel.r - otherPixel.r));
			pixel.g = static_cast<unsigned char>(std::abs(pixel.g - otherPixel.g));
			pixel.b = static_cast<unsigned char>(std::abs(pixel.b - otherPixel.b));
		}
	}
}

void ImageData::average(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to average");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = static_cast<unsigned char>((pixel.r + otherPixel.r) / 2);
			pixel.g = static_cast<unsigned char>((pixel.g + otherPixel.g) / 2);
			pixel.b = static_cast<unsigned char>((pixel.b + otherPixel.b) / 2);
		}
	}
}

void ImageData::max(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to find max");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = std::max(pixel.r, otherPixel.r);
			pixel.g = std::max(pixel.g, otherPixel.g);
			pixel.b = std::max(pixel.b, otherPixel.b);
		}
	}
}

void ImageData::min(const ImageData& other)
{
	if (width != other.width || height != other.height)
	{
		throw std::invalid_argument("Images must be the same dimensions to find min");
	}
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Pixel& pixel = pixels[y][x];
			const Pixel& otherPixel = other.pixels[y][x];
			pixel.r = std::min(pixel.r, otherPixel.r);
			pixel.g = std::min(pixel.g, otherPixel.g);
			pixel.b = std::min(pixel.b, otherPixel.b);
		}
	}
}

// Convolution Functions
// ------------------------------------------------------------------------------------

template<size_t N>
void ImageData::applyKernel(const Kernel<N>& kernel) 
{
	std::vector<std::vector<Pixel>> result{ pixels };

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;

			for (int i = 0; i < static_cast<int>(N); ++i)
			{
				for (int j = 0; j < static_cast<int>(N); ++j)
				{
					int newY = y + i - N / 2;
					int newX = x + j - N / 2;

					Pixel sample{ 0, 0, 0, 255 };
					if (newY >= 0 && newY < height && newX >= 0 && newX < width)
					{
						sample = pixels[newY][newX];
					}
					else
					{
						// Handle out-of-bounds pixels with zero padding
						sample = { 0, 0, 0, 255 };
					}

					float weight = kernel[i][j];
					r += weight * sample.r;
					g += weight * sample.g;
					b += weight * sample.b;
				}
			}
			result[y][x].r = static_cast<unsigned char>(std::clamp(r, 0.0f, 255.0f));
			result[y][x].g = static_cast<unsigned char>(std::clamp(g, 0.0f, 255.0f));
			result[y][x].b = static_cast<unsigned char>(std::clamp(b, 0.0f, 255.0f));
			result[y][x].a = pixels[y][x].a;
		}
	}
	pixels = std::move(result);
}

template<size_t N>
static constexpr ImageData::Kernel<N> scaleKernel(const ImageData::Kernel<N>& kernel, float scale)
{
	ImageData::Kernel<N> result = kernel;
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			result[i][j] *= scale;
		}
	}
	return result;
}

template<size_t N>
static constexpr ImageData::Kernel<N> normalizeKernel(const ImageData::Kernel<N>& kernel)
{
	float sum = 0.0f;
	for (const auto& row : kernel)
	{
		for (float val : row)
		{
			sum += val;
		}
	}

	ImageData::Kernel<N> result = kernel;

	if (sum != 0.0f)
	{
		for (auto& row : result)
		{
			for (float& val : row)
			{
				val /= sum;
			}
		}
	}
	return result;
}

template<size_t N, size_t M>
static constexpr ImageData::Kernel<N + M - 1> convolveKernels(const ImageData::Kernel<N>& k1, const ImageData::Kernel<M>& k2)
{
	constexpr size_t newSize = N + M - 1;
	ImageData::Kernel<newSize> result{};

	for (size_t i = 0; i < newSize; ++i) {
		for (size_t j = 0; j < newSize; ++j) {
			float sum = 0.0f;

			for (size_t u = 0; u < N; ++u) {
				for (size_t v = 0; v < N; ++v) {
					int k2_i = static_cast<int>(i) - static_cast<int>(u);
					int k2_j = static_cast<int>(j) - static_cast<int>(v);

					if (k2_i >= 0 && k2_i < static_cast<int>(M) &&
						k2_j >= 0 && k2_j < static_cast<int>(M)) {
						sum += k1[u][v] * k2[k2_i][k2_j];
					}
				}
			}
			result[i][j] = sum;
		}
	}

	return result;
}

constexpr const char* ImageData::toString(KernelType type)
{
	switch (type) {
	case KernelType::Default:
		return "Default";
	case KernelType::BoxBlur:
		return "Box Blur";
	case KernelType::GaussianBlur:
		return "Gaussian Blur";
	case KernelType::SobelX:
		return "Sobel X";
	case KernelType::SobelY:
		return "Sobel Y";
	case KernelType::Laplacian:
		return "Laplacian";
	case KernelType::Sharpen:
		return "Sharpen";
	case KernelType::Emboss:
		return "Emboss";
	default:
		return "Unknown";
	}
}

constexpr ImageData::Kernel<3> getDefaultKernel()
{
	return { {
		{{  0,  0,  0 }},
		{{  0,  1,  0 }},
		{{  0,  0,  0 }}
	} };
}
constexpr ImageData::Kernel<3> getBoxBlurKernel()
{
	return { {
		{{  1,  1,  1 }},
		{{  1,  1,  1 }},
		{{  1,  1,  1 }}
	} };
}
constexpr ImageData::Kernel<3> getGaussianBlurKernel()
{
	return { {
		{{  1,  2,  1 }},
		{{  2,  4,  2 }},
		{{  1,  2,  1 }}
	} };
}
constexpr ImageData::Kernel<3> getSobelXKernel()
{
	return { {
		{{ -1,  0,  1 }},
		{{ -2,  0,  2 }},
		{{ -1,  0,  1 }}
	} };
}
constexpr ImageData::Kernel<3> getSobelYKernel()
{
	return { {
		{{ -1, -2, -1 }},
		{{  0,  0,  0 }},
		{{  1,  2,  1 }}
	} };
}
constexpr ImageData::Kernel<3> getLaplacianKernel()
{
	return { {
		{{  0, -1,  0 }},
		{{ -1,  4, -1 }},
		{{  0, -1,  0 }}
	} };
}
constexpr ImageData::Kernel<3> getSharpenKernel()
{
	return { {
		{{  0, -1,  0 }},
		{{ -1,  5, -1 }},
		{{  0, -1,  0 }}
	} };
}
constexpr ImageData::Kernel<3> getEmbossKernel()
{
	return { {
		{{ -2, -1,  0 }},
		{{ -1,  1,  1 }},
		{{  0,  1,  2 }}
	} };
}

static constexpr ImageData::Kernel<3> getKernel(ImageData::KernelType type)
{
	switch (type) {
	case ImageData::KernelType::Default:
		return getDefaultKernel();
	case ImageData::KernelType::BoxBlur:
		return getBoxBlurKernel();
	case ImageData::KernelType::GaussianBlur:
		return getGaussianBlurKernel();
	case ImageData::KernelType::SobelX:
		return getSobelXKernel();
	case ImageData::KernelType::SobelY:
		return getSobelYKernel();
	case ImageData::KernelType::Laplacian:
		return getLaplacianKernel();
	case ImageData::KernelType::Sharpen:
		return getSharpenKernel();
	case ImageData::KernelType::Emboss:
		return getEmbossKernel();
	default:
		return getDefaultKernel();
	}
}

void ImageData::applyKernel(const ImageData::DynamicKernel& kernel)
{
	int N = static_cast<int>(kernel.size());
	if (N == 0) throw std::invalid_argument("Invalid kernel size");

	int M = static_cast<int>(kernel[0].size());
	if (M == 0) throw std::invalid_argument("Invalid kernel size");

	std::vector<std::vector<Pixel>> result{ pixels };

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;

			for (int i = 0; i < static_cast<int>(N); ++i)
			{
				for (int j = 0; j < static_cast<int>(N); ++j)
				{
					int newY = y + i - N / 2;
					int newX = x + j - M / 2;

					Pixel sample{ 0, 0, 0, 255 };
					if (newY >= 0 && newY < height && newX >= 0 && newX < width)
					{
						sample = pixels[newY][newX];
					}
					else
					{
						// Handle out-of-bounds pixels with zero padding
						sample = { 0, 0, 0, 255 };
					}

					float weight = kernel[i][j];
					r += weight * sample.r;
					g += weight * sample.g;
					b += weight * sample.b;
				}
			}
			result[y][x].r = static_cast<unsigned char>(std::clamp(r, 0.0f, 255.0f));
			result[y][x].g = static_cast<unsigned char>(std::clamp(g, 0.0f, 255.0f));
			result[y][x].b = static_cast<unsigned char>(std::clamp(b, 0.0f, 255.0f));
			result[y][x].a = pixels[y][x].a;
		}
	}
	pixels = std::move(result);
}

ImageData::DynamicKernel ImageData::scaleKernel(const DynamicKernel& kernel, float scale)
{
	DynamicKernel result = kernel;
	for (auto& row : result)
	{
		for (float& val : row)
		{
			val *= scale;
		}
	}
	return result;
}

ImageData::DynamicKernel ImageData::normalizeKernel(const ImageData::DynamicKernel& kernel)
{
	float sum = 0.0f;
	for (const auto& row : kernel)
	{
		for (float val : row)
		{
			sum += val;
		}
	}

	DynamicKernel result = kernel;

	if (sum != 0.0f)
	{
		for (auto& row : result)
		{
			for (float& val : row)
			{
				val /= sum;
			}
		}
	}
	return result;
}

ImageData::DynamicKernel ImageData::createCustomKernel(const std::vector<std::vector<float>>& values)
{
	if (values.empty() || values[0].empty())
	{
		throw std::invalid_argument("Invalid kernel size");
	}
	int rows = static_cast<int>(values.size());
	int cols = static_cast<int>(values[0].size());
	DynamicKernel kernel(rows, std::vector<float>(cols));
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			kernel[i][j] = values[i][j];
		}
	}
	return kernel;
}

ImageData::DynamicKernel ImageData::convolveKernels(const ImageData::DynamicKernel& k1, const ImageData::DynamicKernel& k2)
{
	int N = static_cast<int>(k1.size());
	int M = static_cast<int>(k2.size());

	if (N == 0 || M == 0 || k1[0].size() != N || k2[0].size() != M) {
		throw std::invalid_argument("Kernels must be square and non-empty");
	}

	int newSize = N + M - 1;
	DynamicKernel result(newSize, std::vector<float>(newSize, 0.0f));

	for (int i = 0; i < newSize; ++i) {
		for (int j = 0; j < newSize; ++j) {
			float sum = 0.0f;
			for (int u = 0; u < N; ++u) {
				for (int v = 0; v < N; ++v) {
					int ki = i - u;
					int kj = j - v;
					if (ki >= 0 && ki < M && kj >= 0 && kj < M) {
						sum += k1[u][v] * k2[ki][kj];
					}
				}
			}
			result[i][j] = sum;
		}
	}

	return result;
}

#pragma warning(pop)