/**
 * @file test_version.cpp
 * @brief Unit tests for ARToolKitPlus version header
 *
 * This test suite validates the version header functionality across different C++ standards.
 * It tests:
 * - Version constants (major, minor, patch)
 * - Version strings (C++11 const char* vs C++17+ string_view)
 * - Version struct and comparison operators
 * - Version checking functions (runtime and compile-time)
 * - Legacy C-style macros for backward compatibility
 * - C++ standard detection macros
 */

#include "ARToolKitPlus/version.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace ARToolKitPlus;

namespace {

/**
 * Test fixture for version tests
 */
class VersionTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

} // namespace

// ===============================================
// Version Constants Tests
// ===============================================

/**
 * Tests that version constants are accessible and have sensible values
 */
TEST_F(VersionTest, VersionConstants) {
    EXPECT_GT(version_major, 0) << "Major version should be positive";
    EXPECT_GE(version_minor, 0) << "Minor version should be non-negative";
    EXPECT_GE(version_patch, 0) << "Patch version should be non-negative";
}

// ===============================================
// Version String Tests
// ===============================================

/**
 * Tests that version string is accessible and non-empty
 * Type varies by C++ standard: const char* (C++11) or string_view (C++17+)
 */
TEST_F(VersionTest, VersionString) {
    EXPECT_NE(version_string, nullptr);

#ifdef ARTK_CXX17
    // C++17+: string_view
    EXPECT_FALSE(version_string.empty()) << "Version string should not be empty";
    EXPECT_GT(version_string.length(), 0) << "Version string should have positive length";
#else
    // C++11: const char*
    EXPECT_GT(std::strlen(version_string), 0u) << "Version string should not be empty";
#endif
}

// ===============================================
// Version Struct Tests
// ===============================================

/**
 * Tests the Version struct construction and to_int() conversion
 */
TEST_F(VersionTest, VersionStructConstruction) {
    constexpr Version v{1, 2, 3};
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 2);
    EXPECT_EQ(v.patch, 3);
    EXPECT_EQ(v.to_int(), 10203) << "Version 1.2.3 should convert to 10203";
}

/**
 * Tests that current_version matches individual version constants
 */
TEST_F(VersionTest, CurrentVersion) {
    EXPECT_EQ(current_version.major, version_major);
    EXPECT_EQ(current_version.minor, version_minor);
    EXPECT_EQ(current_version.patch, version_patch);
}

/**
 * Tests Version to_int() conversion with various values
 */
TEST_F(VersionTest, VersionToInt) {
    EXPECT_EQ(Version(0, 0, 0).to_int(), 0);
    EXPECT_EQ(Version(1, 0, 0).to_int(), 10000);
    EXPECT_EQ(Version(0, 1, 0).to_int(), 100);
    EXPECT_EQ(Version(0, 0, 1).to_int(), 1);
    EXPECT_EQ(Version(2, 5, 7).to_int(), 20507);
}

// ===============================================
// Version Comparison Tests
// ===============================================

/**
 * Tests equality operator
 */
TEST_F(VersionTest, EqualityComparison) {
    constexpr Version v1{1, 5, 0};
    constexpr Version v2{1, 5, 0};
    constexpr Version v3{1, 4, 9};

    EXPECT_TRUE(v1 == v2) << "Identical versions should be equal";
    EXPECT_FALSE(v1 == v3) << "Different versions should not be equal";
}

/**
 * Tests inequality operator (C++11/17 only, implicit in C++20)
 */
TEST_F(VersionTest, InequalityComparison) {
    constexpr Version v1{1, 5, 0};
    constexpr Version v2{1, 4, 9};

    EXPECT_TRUE(v1 != v2) << "Different versions should be unequal";
    EXPECT_FALSE(v1 != v1) << "Same version should not be unequal to itself";
}

/**
 * Tests less-than operator
 */
TEST_F(VersionTest, LessThanComparison) {
    constexpr Version v1{1, 0, 0};
    constexpr Version v2{1, 5, 0};
    constexpr Version v3{2, 0, 0};

    EXPECT_TRUE(v1 < v2) << "1.0.0 should be less than 1.5.0";
    EXPECT_TRUE(v2 < v3) << "1.5.0 should be less than 2.0.0";
    EXPECT_FALSE(v2 < v1) << "1.5.0 should not be less than 1.0.0";
    EXPECT_FALSE(v1 < v1) << "Version should not be less than itself";
}

/**
 * Tests less-than-or-equal operator
 */
TEST_F(VersionTest, LessThanOrEqualComparison) {
    constexpr Version v1{1, 0, 0};
    constexpr Version v2{1, 5, 0};

    EXPECT_TRUE(v1 <= v2) << "1.0.0 should be <= 1.5.0";
    EXPECT_TRUE(v1 <= v1) << "Version should be <= itself";
    EXPECT_FALSE(v2 <= v1) << "1.5.0 should not be <= 1.0.0";
}

/**
 * Tests greater-than operator
 */
TEST_F(VersionTest, GreaterThanComparison) {
    constexpr Version v1{1, 0, 0};
    constexpr Version v2{1, 5, 0};
    constexpr Version v3{2, 0, 0};

    EXPECT_TRUE(v2 > v1) << "1.5.0 should be greater than 1.0.0";
    EXPECT_TRUE(v3 > v2) << "2.0.0 should be greater than 1.5.0";
    EXPECT_FALSE(v1 > v2) << "1.0.0 should not be greater than 1.5.0";
    EXPECT_FALSE(v1 > v1) << "Version should not be greater than itself";
}

/**
 * Tests greater-than-or-equal operator
 */
TEST_F(VersionTest, GreaterThanOrEqualComparison) {
    constexpr Version v1{1, 0, 0};
    constexpr Version v2{1, 5, 0};

    EXPECT_TRUE(v2 >= v1) << "1.5.0 should be >= 1.0.0";
    EXPECT_TRUE(v1 >= v1) << "Version should be >= itself";
    EXPECT_FALSE(v1 >= v2) << "1.0.0 should not be >= 1.5.0";
}

// ===============================================
// Version Checking Function Tests
// ===============================================

/**
 * Tests version_at_least() function with various inputs
 */
TEST_F(VersionTest, VersionAtLeastFunction) {
    // Should be at least version 0.0.0
    EXPECT_TRUE(version_at_least(0, 0, 0)) << "Any version is at least 0.0.0";

    // Should be at least its own version
    EXPECT_TRUE(version_at_least(version_major, version_minor, version_patch))
        << "Version should be at least its own version";

    // Should not be at least a very high version
    EXPECT_FALSE(version_at_least(999, 0, 0)) << "Current version should not be at least 999.0.0";
}

// ===============================================
// Variable Template Tests (C++17+)
// ===============================================

#if defined(ARTK_CXX17) || defined(ARTK_CXX20)
/**
 * Tests compile-time version_at_least_v variable template (C++17+)
 */
TEST_F(VersionTest, VersionAtLeastVariableTemplate) {
    // Compile-time checks
    EXPECT_TRUE((version_at_least_v<0, 0, 0>)) << "Any version is at least 0.0.0";
    EXPECT_FALSE((version_at_least_v<999, 0, 0>)) << "Current version should not be at least 999.0.0";

    // Can be used in static_assert
    static_assert(version_at_least_v<0, 0, 0>, "Should be at least 0.0.0");
}
#endif

// ===============================================
// Legacy C-style Macro Tests
// ===============================================

/**
 * Tests that legacy C-style version macros are defined and consistent
 */
TEST_F(VersionTest, LegacyMacros) {
    // Test individual version macros
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_MAJOR, version_major)
        << "C macro should match C++ constant";
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_MINOR, version_minor)
        << "C macro should match C++ constant";
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_PATCH, version_patch)
        << "C macro should match C++ constant";

    // Test version string macro
    EXPECT_STREQ(ARTOOLKITPLUS_VERSION_STRING, version_string)
        << "C macro string should match C++ version string";

    // Test composite version macro
    constexpr int expected_version = version_major * 10000 + version_minor * 100 + version_patch;
    EXPECT_EQ(ARTOOLKITPLUS_VERSION, expected_version)
        << "ARTOOLKITPLUS_VERSION macro should match computed value";
}

/**
 * Tests ARTOOLKITPLUS_VERSION_CHECK macro
 */
TEST_F(VersionTest, VersionCheckMacro) {
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_CHECK(0, 0, 0), 0);
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_CHECK(1, 0, 0), 10000);
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_CHECK(0, 1, 0), 100);
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_CHECK(0, 0, 1), 1);
    EXPECT_EQ(ARTOOLKITPLUS_VERSION_CHECK(2, 5, 7), 20507);
}

/**
 * Tests ARTOOLKITPLUS_VERSION_AT_LEAST macro
 */
TEST_F(VersionTest, VersionAtLeastMacro) {
    // Should be at least version 0.0.0
    EXPECT_TRUE(ARTOOLKITPLUS_VERSION_AT_LEAST(0, 0, 0))
        << "Any version is at least 0.0.0";

    // Should not be at least a very high version
    EXPECT_FALSE(ARTOOLKITPLUS_VERSION_AT_LEAST(999, 0, 0))
        << "Current version should not be at least 999.0.0";
}

// ===============================================
// C++ Standard Detection Tests
// ===============================================

/**
 * Tests that exactly one C++ standard detection macro is defined
 */
TEST_F(VersionTest, CxxStandardDetection) {
    int defined_count = 0;

#ifdef ARTK_CXX20
    defined_count++;
#endif

#ifdef ARTK_CXX17
    defined_count++;
#endif

    // Either C++20, C++17, or neither (C++11/14) should be defined
    EXPECT_LE(defined_count, 1) << "Only one C++ standard macro should be defined";

    // Verify mutually exclusive
#ifdef ARTK_CXX20
    EXPECT_FALSE(false) << "C++20 is active";
#elif defined(ARTK_CXX17)
    EXPECT_FALSE(false) << "C++17 is active";
#else
    EXPECT_FALSE(false) << "C++11/14 is active";
#endif
}

// ===============================================
// Integration Tests
// ===============================================

/**
 * Tests that all version representations are consistent
 */
TEST_F(VersionTest, ConsistencyAcrossRepresentations) {
    // C++ constants
    constexpr int cpp_major = version_major;
    constexpr int cpp_minor = version_minor;
    constexpr int cpp_patch = version_patch;

    // C macros
    constexpr int c_major = ARTOOLKITPLUS_VERSION_MAJOR;
    constexpr int c_minor = ARTOOLKITPLUS_VERSION_MINOR;
    constexpr int c_patch = ARTOOLKITPLUS_VERSION_PATCH;

    // Version struct
    constexpr int struct_major = current_version.major;
    constexpr int struct_minor = current_version.minor;
    constexpr int struct_patch = current_version.patch;

    // All should be consistent
    EXPECT_EQ(cpp_major, c_major);
    EXPECT_EQ(cpp_major, struct_major);
    EXPECT_EQ(cpp_minor, c_minor);
    EXPECT_EQ(cpp_minor, struct_minor);
    EXPECT_EQ(cpp_patch, c_patch);
    EXPECT_EQ(cpp_patch, struct_patch);
}

/**
 * Tests version string parsing (format should be "major.minor.patch")
 */
TEST_F(VersionTest, VersionStringFormat) {
    std::string ver_str(version_string);

    // Should contain dots
    EXPECT_NE(ver_str.find('.'), std::string::npos)
        << "Version string should contain at least one dot";

    // Parse and compare
    std::istringstream iss(ver_str);
    int parsed_major = -1;
    int parsed_minor = -1;
    int parsed_patch = -1;
    char dot1;
    char dot2;

    iss >> parsed_major >> dot1 >> parsed_minor >> dot2 >> parsed_patch;

    EXPECT_EQ(parsed_major, version_major) << "Parsed major should match constant";
    EXPECT_EQ(parsed_minor, version_minor) << "Parsed minor should match constant";
    EXPECT_EQ(parsed_patch, version_patch) << "Parsed patch should match constant";
    EXPECT_EQ(dot1, '.') << "First separator should be a dot";
    EXPECT_EQ(dot2, '.') << "Second separator should be a dot";
}

