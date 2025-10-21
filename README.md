# ImageProcessor

A lightweight C++ image processing library. Supports basic editing operations (invert, grayscale, brightness, filters, blending, kernels, etc.) with a simple object-oriented interface.

---

## Features

- Load and save images (PNG, JPG, BMP)
- Pixel-level access via `ImageData`
- Image operations:
  - Grayscale, thresholding, inversion
  - Brightness, contrast, tint, noise
- Geometry:
  - Flip, rotate, resize, crop
- Compositing:
  - Blend, multiply, screen, overlay, add/subtract/difference
- Convolution filters:
  - Box blur, Gaussian blur, Sobel, Laplacian, sharpen, emboss
- Custom kernel support (fixed-size or dynamic)

---

## A Few Example Outputs

### Unedited input images
<p float="left">
  <img src="input/cat.png" width="30%">
  <img src="input/dog.png" width="30%">
</p>

### Invert
<p float="left">
  <img src="testOutputs/cat_invert.png" width="30%">
  <img src="testOutputs/dog_invert.png" width="30%">
</p>

### Noise
<p float="left">
  <img src="testOutputs/cat_noise.png" width="30%">
  <img src="testOutputs/dog_noise.png" width="30%">
</p>

### Threshold
<p float="left">
  <img src="testOutputs/cat_threshold.png" width="30%">
  <img src="testOutputs/dog_threshold.png" width="30%">
</p>

### Emboss Kernel
<p float="left">
  <img src="testOutputs/cat_emboss.png" width="30%">
  <img src="testOutputs/dog_emboss.png" width="30%">
</p>

### Sobel Kernel
<p float="left">
  <img src="testOutputs/cat_sobelX.png" width="30%">
  <img src="testOutputs/dog_sobelX.png" width="30%">
</p>

### Max and Min Composition
<p float="left">
  <img src="testOutputs/max_dog_cat.png" width="30%">
  <img src="testOutputs/min_dog_cat.png" width="30%">
</p>

### Add and Subtract Composition
<p float="left">
  <img src="testOutputs/add_dog_cat.png" width="30%">
  <img src="testOutputs/subtract_dog_cat.png" width="30%">
</p>

---

## Project Structure

```
ImageProcessor/
├── CMakeLists.txt
├── inc/
│   ├── ImageData.h
│   └── stb/
│       ├── stb_image.h
│       └── stb_image_write.h
├── src/
│   ├── main.cpp
│   └── stb/
│       ├── stb_image.cpp
│       └── stb_image_write.cpp
├── input/
│   ├── cat.jpg
│   └── dog.jpg
└── output/
```

---

### Prerequisites
- CMake ≥ 3.16  
- C++20-compatible compiler

### Build Instructions

```bash
# Clone the repo
git clone https://github.com/<yourusername>/ImageProcessor.git
cd ImageProcessor

# Configure the build
cmake -B build -S .

# Build the executable
cmake --build build --config Release

# Run
./bin/ImageProcessor
```

---

## Example Usage

```cpp
#include "ImageData.h"

int main() {
    ImageData img;
    img.loadImage("input/dog.jpg");

    img.invert();
    img.saveImage("output/dog_inverted.png", ImageFormat::PNG);

    return 0;
}
```

---

## CMake Notes

The provided `CMakeLists.txt`:
- Builds the project as a standalone executable.
- Includes both the `inc/` and `inc/stb/` directories.
- Automatically creates the `output/` directory after build.

---

## Dependencies

- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)  
- [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)

Both are included locally in `inc/stb/`.

---
