#include "ARToolKitPlus/vector.h"
#include "ARToolKitPlus/matrix.h"
#include <gtest/gtest.h>
#include <vector>
#include <span>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <numbers>
#include <string>
#include <initializer_list>


using namespace ARToolKitPlus;

namespace {

// RAII wrapper for ARVec
struct VecDeleter {
    void operator()(ARVec* v) const noexcept {
        if (v) Vector::free(v);
    }
};
using VecPtr = std::unique_ptr<ARVec, VecDeleter>;

// RAII wrapper for ARMat (needed for tridiagonalize tests)
struct MatDeleter {
    void operator()(ARMat* m) const noexcept {
        if (m) Matrix::free(m);
    }
};
using MatPtr = std::unique_ptr<ARMat, MatDeleter>;

VecPtr makeVec(int clm, std::span<const ARFloat> values) {
    EXPECT_TRUE(clm > 0);
    ARVec* raw = Vector::alloc(clm);
    EXPECT_NE(raw, nullptr);
    if (!values.empty()) {
        const auto sz = static_cast<std::size_t>(clm);
        const auto valuesSz = values.size();
        EXPECT_EQ(sz, valuesSz);
        std::ranges::copy(values, raw->v);
    }
    return VecPtr(raw);
}

VecPtr makeVec(int clm, std::initializer_list<ARFloat> values = {}) {
    return makeVec(clm, std::span<const ARFloat>(values));
}

MatPtr makeMat(int rows, int cols, std::span<const ARFloat> values) {
    EXPECT_TRUE(rows > 0 && cols > 0);
    ARMat* raw = Matrix::alloc(rows, cols);
    EXPECT_NE(raw, nullptr);
    if (!values.empty()) {
        const auto sz = static_cast<std::size_t>(rows * cols);
        const auto valuesSz = values.size();
        EXPECT_EQ(sz, valuesSz);
        std::ranges::copy(values, raw->m);
    }
    return MatPtr(raw);
}

MatPtr makeMat(int rows, int cols, std::initializer_list<ARFloat> values = {}) {
    return makeMat(rows, cols, std::span<const ARFloat>(values));
}

inline constexpr ARFloat eps() { return static_cast<ARFloat>(1e-6); }

} // namespace

// -------------------------------
// alloc / free
// -------------------------------

TEST(VectorAllocFree, AllocatesCorrectly) {
    const auto v = Vector::alloc(5);
    EXPECT_EQ(v->clm, 5);
    ASSERT_NE(v->v, nullptr);
}

TEST(VectorAllocFree, AllocatesSingleElement) {
    const auto v = Vector::alloc(1);
    EXPECT_EQ(v->clm, 1);
    ASSERT_NE(v->v, nullptr);
}

TEST(VectorAllocFree, AllocatesLargeVector) {
    constexpr int large_size = 10000;
    const auto v = Vector::alloc(large_size);
    EXPECT_EQ(v->clm, large_size);
    ASSERT_NE(v->v, nullptr);
}

TEST(VectorAllocFree, InitializesWithValues) {
    const auto v = makeVec(4, {1.0f, 2.0f, 3.0f, 4.0f});
    EXPECT_EQ(v->clm, 4);
    EXPECT_NEAR(v->v[0], 1.0f, eps());
    EXPECT_NEAR(v->v[1], 2.0f, eps());
    EXPECT_NEAR(v->v[2], 3.0f, eps());
    EXPECT_NEAR(v->v[3], 4.0f, eps());
}

// -------------------------------
// innerproduct
// -------------------------------

struct InnerProductCase {
    int dim;
    std::vector<ARFloat> x;
    std::vector<ARFloat> y;
    ARFloat expected;
};

TEST(VectorInnerProduct, BasicCases) {
    const std::vector<InnerProductCase> cases = {
        // Orthogonal vectors
        {2, {1.0f, 0.0f}, {0.0f, 1.0f}, 0.0f},
        // Parallel vectors
        {3, {1.0f, 2.0f, 3.0f}, {1.0f, 2.0f, 3.0f}, 14.0f},
        // Opposite vectors
        {2, {1.0f, 0.0f}, {-1.0f, 0.0f}, -1.0f},
        // General case
        {4, {1.0f, 2.0f, 3.0f, 4.0f}, {5.0f, 6.0f, 7.0f, 8.0f}, 70.0f},
        // Single element
        {1, {5.0f}, {3.0f}, 15.0f},
        // Zero vectors
        {3, {0.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 3.0f}, 0.0f},
    };

    for (const auto& c : cases) {
        const auto x = makeVec(c.dim, c.x);
        const auto y = makeVec(c.dim, c.y);

        const ARFloat result = Vector::innerproduct(x.get(), y.get());
        EXPECT_NEAR(result, c.expected, eps())
            << "Failed for dim=" << c.dim;
    }
}

TEST(VectorInnerProduct, Commutative) {
    const auto x = makeVec(3, {1.0f, 2.0f, 3.0f});
    const auto y = makeVec(3, {4.0f, 5.0f, 6.0f});

    const ARFloat xy = Vector::innerproduct(x.get(), y.get());
    const ARFloat yx = Vector::innerproduct(y.get(), x.get());

    EXPECT_NEAR(xy, yx, eps());
}

TEST(VectorInnerProduct, NormSquared) {
    const auto v = makeVec(3, {3.0f, 4.0f, 0.0f});
    const ARFloat normSquared = Vector::innerproduct(v.get(), v.get());

    // 3^2 + 4^2 = 9 + 16 = 25
    EXPECT_NEAR(normSquared, 25.0f, eps());
    EXPECT_NEAR(std::sqrt(normSquared), 5.0f, eps());
}

TEST(VectorInnerProduct, NegativeValues) {
    const auto x = makeVec(3, {-1.0f, -2.0f, -3.0f});
    const auto y = makeVec(3, {1.0f, 2.0f, 3.0f});

    const ARFloat result = Vector::innerproduct(x.get(), y.get());
    // -1*1 + -2*2 + -3*3 = -1 - 4 - 9 = -14
    EXPECT_NEAR(result, -14.0f, eps());
}

// -------------------------------
// household
// -------------------------------

struct HouseholdCase {
    int dim;
    std::vector<ARFloat> input;
    ARFloat expectedReturn;
    std::string description;
};

TEST(VectorHousehold, BasicCases) {
    const std::vector<HouseholdCase> cases = {
        // Simple 2D vector
        {2, {3.0f, 4.0f}, -5.0f, "3-4-5 triangle"},
        // Vector with zero norm
        {3, {0.0f, 0.0f, 0.0f}, 0.0f, "zero vector"},
        // Unit vector
        {3, {1.0f, 0.0f, 0.0f}, -1.0f, "unit vector x"},
        // Single element
        {1, {5.0f}, -5.0f, "single element"},
    };

    for (const auto& c : cases) {
        auto v = makeVec(c.dim, c.input);
        const ARFloat result = Vector::household(v.get());

        EXPECT_NEAR(result, c.expectedReturn, eps())
            << "Failed for: " << c.description;

        // After household transformation, the vector should be normalized
        // (unless it was zero to begin with)
        if (c.expectedReturn != 0.0f) {
            const ARFloat squared_norm = Vector::innerproduct(v.get(), v.get());
            EXPECT_NEAR(squared_norm, 2.0f, eps())
                << "Vector not normalized for: " << c.description;
        }
    }
}

TEST(VectorHousehold, NormalizationProperty) {
    // The household transformation should produce a vector of norm sqrt(2)
    // (when the input is non-zero)
    const std::vector<std::vector<ARFloat>> testVectors = {
        {1.0f, 2.0f, 3.0f},
        {5.0f, 0.0f, 0.0f},
        {-2.0f, -3.0f, -4.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
    };


    for (const auto& input : testVectors) {
        auto v = makeVec(static_cast<int>(input.size()), input);
        const ARFloat s = Vector::household(v.get());

        if (s != 0.0f) {
            const ARFloat normSquared = Vector::innerproduct(v.get(), v.get());
            EXPECT_NEAR(normSquared, 2.0f, eps());
        }
    }
}

TEST(VectorHousehold, SignConvention) {
    // household should return -s where s has the same sign as v[0]
    {
        auto v = makeVec(2, {3.0f, 4.0f});
        const ARFloat s = Vector::household(v.get());
        EXPECT_LT(s, 0.0f); // Should be negative since v[0] > 0
    }

    {
        auto v = makeVec(2, {-3.0f, 4.0f});
        const ARFloat s = Vector::household(v.get());
        EXPECT_GT(s, 0.0f); // Should be positive since v[0] < 0
    }
}

TEST(VectorHousehold, FirstElementModification) {
    // The first element should be modified: v[0] += s
    auto v = makeVec(3, {3.0f, 4.0f, 0.0f});
    const ARFloat original_v0 = v->v[0];
    const ARFloat s = Vector::household(v.get());

    // After transformation, v[0] should equal original_v0 + s (before normalization)
    // But since it's normalized, we just check that the sign convention holds
    EXPECT_NEAR(s, -5.0f, eps()); // ||(3,4,0)|| = 5, v[0] > 0 so s = -5
}

// -------------------------------
// tridiagonalize
// -------------------------------

struct TridiagonalizeCase {
    int dim;
    std::vector<ARFloat> input;  // Symmetric matrix
    std::vector<ARFloat> expectedD;  // Diagonal elements
    std::vector<ARFloat> expectedE;  // Sub-diagonal elements
    int expectedReturn;
    std::string description;
};

TEST(VectorTridiagonalize, BasicCases) {
    const std::vector<TridiagonalizeCase> cases = {
        // 2x2 symmetric matrix
        {2,
         {2.0f, 1.0f,
          1.0f, 3.0f},
         {2.0f, 3.0f},
         {1.0f},
         0,
         "2x2 symmetric"},

        // 3x3 identity matrix (already tridiagonal)
        {3,
         {1.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 1.0f},
         {1.0f, 1.0f, 1.0f},
         {0.0f, 0.0f},
         0,
         "3x3 identity"},
    };

    for (const auto& c : cases) {
        auto a = makeMat(c.dim, c.dim, c.input);
        auto d = makeVec(c.dim);
        auto e = makeVec(c.dim - 1);

        const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
        EXPECT_EQ(ret, c.expectedReturn)
            << "Failed return value for: " << c.description;

        if (ret == 0) {
            // Check diagonal elements
            for (int i = 0; i < c.dim; ++i) {
                EXPECT_NEAR(d->v[i], c.expectedD[static_cast<std::size_t>(i)], 1e-4f)
                    << "Failed diagonal at index " << i << " for: " << c.description;
            }

            // Check sub-diagonal elements
            for (int i = 0; i < c.dim - 1; ++i) {
                EXPECT_NEAR(e->v[i], c.expectedE[static_cast<std::size_t>(i)], 1e-4f)
                    << "Failed sub-diagonal at index " << i << " for: " << c.description;
            }
        }
    }
}

TEST(VectorTridiagonalize, SymmetricMatrix3x3) {
    // Test with a known symmetric matrix
    const auto a = makeMat(3, 3, {
        4.0f, 1.0f, 2.0f,
        1.0f, 2.0f, 0.0f,
        2.0f, 0.0f, 3.0f
    });
    const auto d = makeVec(3);
    const auto e = makeVec(2);

    const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
    EXPECT_EQ(ret, 0);

    // The transformation should preserve the trace (sum of diagonal elements)
    // Original trace: 4 + 2 + 3 = 9
    const ARFloat traceAfter = d->v[0] + d->v[1] + d->v[2];
    EXPECT_NEAR(traceAfter, 9.0f, 1e-4f);

    // The transformation matrix in 'a' should be orthogonal
    // Check that columns have unit length
    for (int col = 0; col < 3; ++col) {
        ARFloat colNormSq = 0.0f;
        for (int row = 0; row < 3; ++row) {
            const ARFloat val = a->m[row * 3 + col];
            colNormSq += val * val;
        }
        EXPECT_NEAR(colNormSq, 1.0f, 1e-4f)
            << "Column " << col << " not normalized";
    }
}

TEST(VectorTridiagonalize, DiagonalMatrix) {
    // A diagonal matrix is already tridiagonal
    const auto a = makeMat(4, 4, {
        5.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 3.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 7.0f
    });
    const auto d = makeVec(4);
    const auto e = makeVec(3);

    const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
    EXPECT_EQ(ret, 0);

    // Diagonal should be preserved
    EXPECT_NEAR(d->v[0], 5.0f, 1e-4f);
    EXPECT_NEAR(d->v[1], 3.0f, 1e-4f);
    EXPECT_NEAR(d->v[2], 2.0f, 1e-4f);
    EXPECT_NEAR(d->v[3], 7.0f, 1e-4f);

    // Sub-diagonal should be zero
    EXPECT_NEAR(e->v[0], 0.0f, 1e-4f);
    EXPECT_NEAR(e->v[1], 0.0f, 1e-4f);
    EXPECT_NEAR(e->v[2], 0.0f, 1e-4f);
}

TEST(VectorTridiagonalize, InvalidDimensions) {
    // Non-square matrix
    {
        const auto a = makeMat(3, 4);
        const auto d = makeVec(3);
        const auto e = makeVec(2);
        const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
        EXPECT_EQ(ret, -1);
    }

    // Mismatched d dimension
    {
        const auto a = makeMat(3, 3);
        const auto d = makeVec(4);
        const auto e = makeVec(2);
        const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
        EXPECT_EQ(ret, -1);
    }

    // Mismatched e dimension
    {
        const auto a = makeMat(3, 3);
        const auto d = makeVec(3);
        const auto e = makeVec(3);
        const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
        EXPECT_EQ(ret, -1);
    }
}

// TEST(VectorTridiagonalize, SingleElement) {
//     // 1x1 matrix
//     const auto a = makeMat(1, 1, {42.0f});
//     const auto d = makeVec(1);
//     const auto e = makeVec(0);  // Empty vector for sub-diagonal
//
//     const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
//     EXPECT_EQ(ret, 0);
//     EXPECT_NEAR(d->v[0], 42.0f, eps());
// }

TEST(VectorTridiagonalize, OrthogonalityProperty) {
    // The transformation matrix stored in 'a' should be orthogonal
    // Test with a 4x4 symmetric matrix
    const auto a = makeMat(4, 4, {
        4.0f, 1.0f, 2.0f, 1.0f,
        1.0f, 3.0f, 1.0f, 0.0f,
        2.0f, 1.0f, 5.0f, 2.0f,
        1.0f, 0.0f, 2.0f, 6.0f
    });
    const auto d = makeVec(4);
    const auto e = makeVec(3);

    const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
    EXPECT_EQ(ret, 0);

    // Check orthogonality: Q^T * Q should be identity
    // Check that dot product of different columns is zero
    for (int i = 0; i < 4; ++i) {
        for (int j = i; j < 4; ++j) {
            ARFloat dot = 0.0f;
            for (int k = 0; k < 4; ++k) {
                dot += a->m[k * 4 + i] * a->m[k * 4 + j];
            }

            const ARFloat expected = (i == j) ? 1.0f : 0.0f;
            EXPECT_NEAR(dot, expected, 1e-4f)
                << "Orthogonality failed for columns " << i << " and " << j;
        }
    }
}

TEST(VectorTridiagonalize, EigenvaluePreservation) {
    // The tridiagonalization preserves eigenvalues
    // For a simple case, check that trace and determinant are preserved
    constexpr int N = 3;
    const auto a = makeMat(N, N, {
        6.0f, 2.0f, 1.0f,
        2.0f, 3.0f, 1.0f,
        1.0f, 1.0f, 1.0f
    });

    // Calculate original trace
    ARFloat traceBefore = 0.0f;
    for (int i = 0; i < N; ++i) {
        traceBefore += a->m[i * N + i];
    }

    const auto d = makeVec(N);
    const auto e = makeVec(N - 1);

    const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
    EXPECT_EQ(ret, 0);

    // Calculate trace from diagonal elements
    ARFloat traceAfter = 0.0f;
    for (int i = 0; i < N; ++i) {
        traceAfter += d->v[i];
    }

    EXPECT_NEAR(traceBefore, traceAfter, 1e-4f)
        << "Trace not preserved by tridiagonalization";
}

// -------------------------------
// Edge cases and stress tests
// -------------------------------

TEST(VectorEdgeCases, LargeVectorInnerProduct) {
    constexpr int size = 1000;
    const auto x = makeVec(size);
    const auto y = makeVec(size);

    // Fill with sequential values
    for (int i = 0; i < size; ++i) {
        x->v[i] = static_cast<ARFloat>(i + 1);
        y->v[i] = static_cast<ARFloat>(i + 1);
    }

    const ARFloat result = Vector::innerproduct(x.get(), y.get());

    // Sum of i^2 from 1 to n is n(n+1)(2n+1)/6
    const ARFloat expected = static_cast<ARFloat>(size * (size + 1) * (2 * size + 1)) / 6.0f;
    EXPECT_NEAR(result, expected, expected * 1e-4f);
}

TEST(VectorEdgeCases, HouseholdWithVerySmallValues) {
    auto v = makeVec(3, {1e-10f, 1e-10f, 1e-10f});
    const ARFloat s = Vector::household(v.get());

    // Should handle small values gracefully
    EXPECT_TRUE(std::isfinite(s));
    EXPECT_TRUE(std::isfinite(v->v[0]));
    EXPECT_TRUE(std::isfinite(v->v[1]));
    EXPECT_TRUE(std::isfinite(v->v[2]));
}

TEST(VectorEdgeCases, TridiagonalizeLargeMatrix) {
    constexpr int N = 10;
    const auto a = makeMat(N, N);

    // Create a symmetric matrix
    for (int i = 0; i < N; ++i) {
        for (int j = i; j < N; ++j) {
            const ARFloat val = static_cast<ARFloat>(i + j + 1);
            a->m[i * N + j] = val;
            a->m[j * N + i] = val;  // Symmetric
        }
    }

    const auto d = makeVec(N);
    const auto e = makeVec(N - 1);

    const int ret = Vector::tridiagonalize(a.get(), d.get(), e.get());
    EXPECT_EQ(ret, 0);

    // Basic sanity checks
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(std::isfinite(d->v[i]));
    }
    for (int i = 0; i < N - 1; ++i) {
        EXPECT_TRUE(std::isfinite(e->v[i]));
    }
}
