#include "ImageData.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <stdexcept>

namespace fs = std::filesystem;

void ensureOutputDir(const std::string& dir) {
    if (!fs::exists(dir)) fs::create_directory(dir);
}

void saveSafe(ImageData& img, const std::string& filename) {
    try {
        img.saveImage(filename.c_str(), ImageFormat::PNG);
        std::cout << "✅ Saved: " << filename << "\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to save " << filename << ": " << e.what() << "\n";
    }
}

void loadSafe(ImageData& img, const std::string& path) {
    try {
        img.loadImage(path.c_str());
        std::cout << "Loaded: " << path << "\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to load " << path << ": " << e.what() << "\n";
    }
}

int main() {
    try {
        const std::string baseDir = PROJECT_SOURCE_DIR;
        const std::string inputDog = baseDir + "/input/dog.png";
        const std::string inputCat = baseDir + "/input/cat.png";
        const std::string outputDir = baseDir + "/output";
        ensureOutputDir(outputDir);

        // === Load Images ===
        ImageData dog, cat;
        loadSafe(dog, inputDog);
        loadSafe(cat, inputCat);

        // === Tone / Monochrome ===
        {
            ImageData d = dog, c = cat;
            d.invert(); saveSafe(d, outputDir + "/dog_invert.png");
            c.invert(); saveSafe(c, outputDir + "/cat_invert.png");

            d = dog; c = cat;
            d.grayscale(); saveSafe(d, outputDir + "/dog_grayscale.png");
            c.grayscale(); saveSafe(c, outputDir + "/cat_grayscale.png");

            d = dog; c = cat;
            d.threshold(128); saveSafe(d, outputDir + "/dog_threshold.png");
            c.threshold(128); saveSafe(c, outputDir + "/cat_threshold.png");
        }

        // === Color Adjustments ===
        {
            ImageData d = dog, c = cat;
            d.brightness(40); saveSafe(d, outputDir + "/dog_brightness.png");
            c.brightness(-40); saveSafe(c, outputDir + "/cat_brightness.png");

            d = dog; c = cat;
            d.contrast(1.5f); saveSafe(d, outputDir + "/dog_contrast.png");
            c.contrast(0.75f); saveSafe(c, outputDir + "/cat_contrast.png");

            d = dog; c = cat;
            Pixel redTint{255, 50, 50, 255};
            d.tint(redTint, 0.4f); saveSafe(d, outputDir + "/dog_tint.png");
            c.tint(redTint, 0.4f); saveSafe(c, outputDir + "/cat_tint.png");

            d = dog; c = cat;
            d.noise(0.3f); saveSafe(d, outputDir + "/dog_noise.png");
            c.noise(0.3f); saveSafe(c, outputDir + "/cat_noise.png");
        }

        // === Geometry ===
        {
            ImageData d = dog, c = cat;
            d.flipH(); saveSafe(d, outputDir + "/dog_flipH.png");
            c.flipV(); saveSafe(c, outputDir + "/cat_flipV.png");

            d = dog; c = cat;
            d.rotateR(); saveSafe(d, outputDir + "/dog_rotateR.png");
            c.rotateL(); saveSafe(c, outputDir + "/cat_rotateL.png");

            d = dog; c = cat;
            d.resize(d.getWidth() / 2, d.getHeight() / 2); saveSafe(d, outputDir + "/dog_resize.png");
            c.crop(50, 50, c.getWidth() / 2, c.getHeight() / 2); saveSafe(c, outputDir + "/cat_crop.png");
        }

        // === Compositing (dog + cat) ===
        {
            ImageData d = dog, c = cat;
            d.blend(c, 0.5f); saveSafe(d, outputDir + "/blend_dog_cat.png");

            d = dog; d.multiply(c); saveSafe(d, outputDir + "/multiply_dog_cat.png");
            d = dog; d.screen(c); saveSafe(d, outputDir + "/screen_dog_cat.png");
            d = dog; d.overlay(c); saveSafe(d, outputDir + "/overlay_dog_cat.png");
            d = dog; d.darken(c); saveSafe(d, outputDir + "/darken_dog_cat.png");
            d = dog; d.lighten(c); saveSafe(d, outputDir + "/lighten_dog_cat.png");
            d = dog; d.add(c, 0.5f); saveSafe(d, outputDir + "/add_dog_cat.png");
            d = dog; d.subtract(c); saveSafe(d, outputDir + "/subtract_dog_cat.png");
            d = dog; d.difference(c); saveSafe(d, outputDir + "/difference_dog_cat.png");
            d = dog; d.average(c); saveSafe(d, outputDir + "/average_dog_cat.png");
            d = dog; d.max(c); saveSafe(d, outputDir + "/max_dog_cat.png");
            d = dog; d.min(c); saveSafe(d, outputDir + "/min_dog_cat.png");
        }

        // === Convolution / Kernels ===
        {
            ImageData d = dog;
            using KT = ImageData::KernelType;

            const std::vector<std::pair<KT, std::string>> kernels = {
                {KT::BoxBlur, "boxblur"},
                {KT::GaussianBlur, "gaussian"},
                {KT::SobelX, "sobelX"},
                {KT::SobelY, "sobelY"},
                {KT::Laplacian, "laplacian"},
                {KT::Sharpen, "sharpen"},
                {KT::Emboss, "emboss"}
            };

            for (auto [type, name] : kernels) {
                ImageData copy = dog;
                copy.applyKernel(getKernel(type));
                saveSafe(copy, outputDir + "/dog_" + name + ".png");
            }

            for (auto [type, name] : kernels) {
                ImageData copy = cat;
                copy.applyKernel(getKernel(type));
                saveSafe(copy, outputDir + "/cat_" + name + ".png");
            }
        }

        std::cout << "\n🎉 All image operations completed successfully!\n";
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
