# 🖼️ ImageProcessor

A lightweight **C++ image manipulation library and CLI tool** powered by [stb_image](https://github.com/nothings/stb) and [stb_image_write](https://github.com/nothings/stb).  
Supports basic editing operations (invert, grayscale, brightness, filters, blending, kernels, etc.) with a simple object-oriented interface.

---

## 📁 Project Structure

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

## ⚙️ Features

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

## 🚀 Getting Started

### Prerequisites
- CMake ≥ 3.16  
- C++20-compatible compiler:
  - MSVC (Visual Studio 2019+)
  - GCC ≥ 9
  - Clang ≥ 9

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

For **VS Code users**:
- Install the **CMake Tools** extension.
- Use the "Build" and "Run" buttons in the status bar.
- Ensure your launch configuration sets:
  ```json
  "cwd": "${workspaceFolder}"
  ```

---

## 🧪 Example Usage

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

## 🧰 CMake Notes

The provided `CMakeLists.txt`:
- Builds the project as a standalone executable.
- Includes both the `inc/` and `inc/stb/` directories.
- Automatically creates the `output/` directory after build.
- You can optionally copy the `input/` folder into the build directory by adding:
  ```cmake
  add_custom_command(TARGET ImageProcessor POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/input
          $<TARGET_FILE_DIR:ImageProcessor>/input
  )
  ```

---

## 🧩 Troubleshooting

| Problem | Cause | Fix |
|----------|--------|-----|
| `Failed to load image` | Working directory not set correctly | Set `"cwd": "${workspaceFolder}"` in `launch.json` |
| No output file created | `output/` folder missing | CMake auto-creates it, but verify manually |
| `std::clamp` not found | Old compiler or C++ standard | Ensure `CMAKE_CXX_STANDARD 20` is set |

---

## 📚 Dependencies

- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)  
- [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)

Both are included locally in `inc/stb/`.

---

## 🧠 Future Improvements

- Add bilinear/bicubic resize  
- Add gamma correction and tone mapping  
- Add alpha-aware blending (normalized alpha)  
- Add CLI arguments (e.g., `ImageProcessor input.jpg --invert --save output.png`)  
- Split implementation into `.cpp` files for faster rebuilds  

---

## 📄 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
