#include <ARToolKitPlus/TrackerSingleMarker.h>
#include <ARToolKitPlus/version.h>
#include <cstdlib>
#include <iostream>

int main(int , char**)
{
    constexpr int imWidth{640};
    constexpr int imHeight{480};
    constexpr int maxImagePatterns{8};
    constexpr int pattWidth{6};
    constexpr int pattHeight{6};
    constexpr int pattSamples{6};
    constexpr int maxLoadPatterns{0};
    ARToolKitPlus::TrackerSingleMarker tracker(imWidth, imHeight, maxImagePatterns, pattWidth, pattHeight, pattSamples, maxLoadPatterns);
    std::cout << "ARToolKitPlus version: " << ARToolKitPlus::version_string << std::endl;
    return EXIT_SUCCESS;
}