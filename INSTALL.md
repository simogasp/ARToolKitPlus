# Building instructions

ARToolKitPlus requires:

- A C++ compiler with at least C++11 support
- CMake 3.20 or higher
- [optional] Boost fo OpenCV calibration file support
- [optional] GTest for unit tests

You can build the project using the provided CMake presets.
From the root of the ARToolKitPlus source directory:

```bash
 cmake --preset release
 # or
 cmake --preset debug
```

followed by

```bash
 cmake --build --preset release
 # or
 cmake --build --preset debug
```

This will generate the library files in the `build/<preset name>` directory.
If you need to install the library you can run:

```bash
 cmake --build --preset release -t install
# or
 cmake --build --preset debug -t install
 ```
This will install the library and headers in in `build/install/<preset name>`.
If you need to install to a specific location you can change the `installDir` in the CMakePresets.json file.

## ArtoolKitPlus as a 3rd party library

ARToolKitPlus can be used as a 3rd party library in your CMake project using the `find_package` command or the `FetchContent` module if you want to build it as part of your project.
In both cases, a target named `ARToolKitPlus::ARToolKitPlus` will be created that you can link against.

Check the [`sample/as3rdparty/CMakeLists.txt`](sample/as3rdparty/CMakeLists.txt) file for an example of how to do this.