#include "ARToolKitPlus/version.h"
#include <iostream>

int main() {
    std::cout << "=== ARToolKitPlus Version Header Test ===\n\n";

    // Test version constants
    std::cout << "Version constants:\n";
    std::cout << "  Major: " << ARToolKitPlus::version_major << '\n';
    std::cout << "  Minor: " << ARToolKitPlus::version_minor << '\n';
    std::cout << "  Patch: " << ARToolKitPlus::version_patch << '\n';

    // Test version string
    std::cout << "\nVersion string:\n";
#ifdef ARTK_CXX17
    std::cout << "  " << ARToolKitPlus::version_string << " (std::string_view)\n";
#else
    std::cout << "  " << ARToolKitPlus::version_string << " (const char*)\n";
#endif

    // Test Version struct
    std::cout << "\nVersion struct:\n";
    std::cout << "  current_version.major: " << ARToolKitPlus::current_version.major << '\n';
    std::cout << "  current_version.minor: " << ARToolKitPlus::current_version.minor << '\n';
    std::cout << "  current_version.patch: " << ARToolKitPlus::current_version.patch << '\n';
    std::cout << "  current_version.to_int(): " << ARToolKitPlus::current_version.to_int() << '\n';

    // Test version comparisons
    std::cout << "\nVersion comparisons:\n";
    ARToolKitPlus::Version v1{1, 0, 0};
    ARToolKitPlus::Version v2{1, 5, 0};
    ARToolKitPlus::Version v3{1, 5, 0};

    std::cout << "  v1(1.0.0) == v2(1.5.0): " << (v1 == v2) << '\n';
    std::cout << "  v2(1.5.0) == v3(1.5.0): " << (v2 == v3) << '\n';
    std::cout << "  v1(1.0.0) < v2(1.5.0): " << (v1 < v2) << '\n';
    std::cout << "  v2(1.5.0) >= v3(1.5.0): " << (v2 >= v3) << '\n';

#ifdef ARTK_CXX20
    std::cout << "  Using C++20 spaceship operator\n";
#else
    std::cout << "  Using C++11/17 explicit comparisons\n";
#endif

    // Test version_at_least function
    std::cout << "\nversion_at_least() tests:\n";
    std::cout << "  version_at_least(0, 0, 0): " << ARToolKitPlus::version_at_least(0, 0, 0) << '\n';
    std::cout << "  version_at_least(1, 0, 0): " << ARToolKitPlus::version_at_least(1, 0, 0) << '\n';
    std::cout << "  version_at_least(99, 0, 0): " << ARToolKitPlus::version_at_least(99, 0, 0) << '\n';

    // Test variable template (C++17+)
#if defined(ARTK_CXX17) || defined(ARTK_CXX20)
    std::cout << "\nversion_at_least_v template:\n";
    std::cout << "  version_at_least_v<0, 0, 0>: " << ARToolKitPlus::version_at_least_v<0, 0, 0> << '\n';
    std::cout << "  version_at_least_v<1, 0, 0>: " << ARToolKitPlus::version_at_least_v<1, 0, 0> << '\n';
    std::cout << "  version_at_least_v<99, 0, 0>: " << ARToolKitPlus::version_at_least_v<99, 0, 0> << '\n';
#endif

    // Test legacy C-style macros
    std::cout << "\nLegacy C-style macros:\n";
    std::cout << "  ARTOOLKITPLUS_VERSION_MAJOR: " << ARTOOLKITPLUS_VERSION_MAJOR << '\n';
    std::cout << "  ARTOOLKITPLUS_VERSION_MINOR: " << ARTOOLKITPLUS_VERSION_MINOR << '\n';
    std::cout << "  ARTOOLKITPLUS_VERSION_PATCH: " << ARTOOLKITPLUS_VERSION_PATCH << '\n';
    std::cout << "  ARTOOLKITPLUS_VERSION_STRING: " << ARTOOLKITPLUS_VERSION_STRING << '\n';
    std::cout << "  ARTOOLKITPLUS_VERSION: " << ARTOOLKITPLUS_VERSION << '\n';
    std::cout << "  ARTOOLKITPLUS_VERSION_AT_LEAST(1, 0, 0): " << ARTOOLKITPLUS_VERSION_AT_LEAST(1, 0, 0) << '\n';
    std::cout << "  ARTOOLKITPLUS_VERSION_AT_LEAST(99, 0, 0): " << ARTOOLKITPLUS_VERSION_AT_LEAST(99, 0, 0) << '\n';

    // C++ standard detection
    std::cout << "\nC++ standard detection:\n";
#ifdef ARTK_CXX20
    std::cout << "  C++20 or later\n";
#elif defined(ARTK_CXX17)
    std::cout << "  C++17\n";
#else
    std::cout << "  C++11/14\n";
#endif

    std::cout << "\n=== All tests completed successfully ===\n";
    return EXIT_SUCCESS;
}
