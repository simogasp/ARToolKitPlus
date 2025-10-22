#include "ARToolKitPlus/matrix.h"
#include <gtest/gtest.h>
#include <vector>
#include <span>
#include <ranges>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <memory>
#include <random>
#include <cstddef>
#include <initializer_list>


using namespace ARToolKitPlus;

namespace {

// RAII wrapper for ARMat
struct MatDeleter {
    void operator()(ARMat* m) const noexcept {
        if (m) Matrix::free(m);
    }
};
using MatPtr = std::unique_ptr<ARMat, MatDeleter>;

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

inline ARFloat eps() { return static_cast<ARFloat>(1e-6); }

} // namespace

// -------------------------------
// alloc / free
// -------------------------------

TEST(MatrixAllocFree, AllocatesCorrectly) {
    const auto m = Matrix::alloc(3, 4);
    EXPECT_EQ(m->row, 3);
    EXPECT_EQ(m->clm, 4);
    ASSERT_NE(m->m, nullptr);
}

// note that a malloc(0) is implementation dependent, it may return nullptr, a unique pointer, or even a valid but unusable block.
TEST(MatrixAlloc, ZeroSizedMatrixDoesNotCrash) {
    auto m = Matrix::alloc(0, 0);
    ASSERT_NE(m, nullptr);  // If implementation returns non-null
    EXPECT_EQ(m->row, 0);
    EXPECT_EQ(m->clm, 0);
    // m->m may be nullptr or not — don't assert either way
    Matrix::free(m);
}

TEST(MatrixAlloc, ZeroRowsOrColsHandledGracefully) {
    auto m1 = Matrix::alloc(0, 5);
    ASSERT_NE(m1, nullptr);
    EXPECT_EQ(m1->row, 0);
    EXPECT_EQ(m1->clm, 5);
    Matrix::free(m1);

    auto m2 = Matrix::alloc(5, 0);
    ASSERT_NE(m2, nullptr);
    EXPECT_EQ(m2->row, 5);
    EXPECT_EQ(m2->clm, 0);
    Matrix::free(m2);
}

// -------------------------------
// dup / allocDup
// -------------------------------

TEST(MatrixDup, CopiesElements) {
    constexpr auto rows{6u};
    constexpr auto cols{6u};
    for(auto n_rows = 1u; n_rows < rows; ++n_rows) {
        for (auto n_cols = 1u; n_cols < cols; ++n_cols) {
            const auto mat_size = n_rows * n_cols;

            // create a vector of floats of mat_size elements from -(mat_size-1)/2 to +(mat_size-1)/2
            std::vector<ARFloat> values(mat_size);
            // use std::iota to fill the vector
            std::iota(values.begin(), values.end(), -(static_cast<ARFloat>(mat_size) - 1) / 2);

            const auto src = makeMat(static_cast<int>(n_rows), static_cast<int>(n_cols), values);
            const auto dest = makeMat(static_cast<int>(n_rows), static_cast<int>(n_cols));

            const auto ret = Matrix::dup(dest.get(), src.get());
            EXPECT_EQ(ret, 0);

            for (auto i = 0u; i < mat_size; ++i) {
                EXPECT_NEAR(dest->m[i], src->m[i], eps());
            }
            // test the macro ARELEM0
            for(auto i{0u}; i < n_rows; ++i) {
                for(auto j = 0u; j < n_cols; ++j) {
                    EXPECT_NEAR(ARELEM0(dest.get(), i, j), ARELEM0(src.get(), i, j), eps());
                }
            }

        }
    }
}

TEST(MatrixAllocDup, CreatesExactCopy) {
    constexpr auto rows{2u};
    constexpr auto cols{3u};
    const auto src = makeMat(rows, cols, {1, 2, 3, 4, 5, 6});
    MatPtr dup(Matrix::allocDup(src.get()));
    ASSERT_NE(dup, nullptr);

    EXPECT_EQ(dup->row, src->row);
    EXPECT_EQ(dup->clm, src->clm);
    for (int i = 0; i < src->row * src->clm; ++i) {
        EXPECT_NEAR(dup->m[i], src->m[i], eps());
    }
    for(auto i{0u}; i < rows; ++i) {
        for(auto j = 0u; j < cols; ++j) {
            EXPECT_NEAR(ARELEM0(dup.get(), i, j), ARELEM0(src.get(), i, j), eps());
        }
    }
}

// -------------------------------
// mul
// -------------------------------

struct MulCase {
    int r1;
    int c1;
    int r2;
    int c2;
    std::vector<ARFloat> a;
    std::vector<ARFloat> b;
    std::vector<ARFloat> expected;
};

TEST(MatrixMul, BasicCases) {
    const std::vector<MulCase> cases = {
        // Identity * A = A
        {2,2, 2,2,
         {1,0,
          0,1},
         {5,6,
          7,8},
         {5,6,
          7,8}},
        // 2x3 * 3x2
        {2,3, 3,2,
         {1,2,3,
          4,5,6},
         {7,8,
          9,10,
          11,12},
         {58,64,
          139,154}},
        // 1x3 * 3x1 = scalar
        {1,3, 3,1,
         {2,3,4},
         {5,
          6,
          7},
         {56}}
    };

    for (const auto& c : cases) {
        auto A = makeMat(c.r1, c.c1, c.a);
        auto B = makeMat(c.r2, c.c2, c.b);
        auto Dest = makeMat(c.r1, c.c2);

        int ret = Matrix::mul(Dest.get(), A.get(), B.get());
        EXPECT_EQ(ret, 0);

        ASSERT_EQ(static_cast<int>(c.expected.size()), Dest->row * Dest->clm);
        const auto sz = static_cast<std::size_t>(Dest->row * Dest->clm);
        for (auto i = 0u; i < sz; ++i) {
            EXPECT_NEAR(Dest->m[i], c.expected[i], eps());
        }
    }
}

// -------------------------------
// selfInv
// -------------------------------

struct InvCase {
    int n;
    std::vector<ARFloat> input;
    std::vector<ARFloat> expected;
    int expectedRet;
};

TEST(MatrixSelfInv, InvertibleMatrices) {
    std::vector<InvCase> cases = {
        // 2x2 invertible
        {2,
         {4,7,
          2,6},
         {0.6f, -0.7f,
          -0.2f, 0.4f},
         0},
        // Identity
        {3,
         {1,0,0,
          0,1,0,
          0,0,1},
         {1,0,0,
          0,1,0,
          0,0,1},
         0}
    };

    for (auto& c : cases) {
        auto M = makeMat(c.n, c.n, c.input);
        int ret = Matrix::selfInv(M.get());
        EXPECT_EQ(ret, c.expectedRet);

        const auto sz = static_cast<std::size_t>(c.n * c.n);
        for (auto i = 0u; i < sz; ++i) {
            EXPECT_NEAR(M->m[i], c.expected[i], 1e-5);
        }
    }
}

TEST(MatrixSelfInv, SingularMatrix) {
    // Determinant = 0
    const auto M = makeMat(2, 2, {1,2,
                            2,4});
    const int ret = Matrix::selfInv(M.get());
    EXPECT_EQ(ret, -1);
}

// Utility: multiply two matrices and return a std::vector
std::vector<ARFloat> multiplyRaw(const ARMat* A, const ARMat* B) {
    const auto sz = static_cast<std::size_t>(A->row * A->clm);
    std::vector<ARFloat> result(sz, 0.0f);
    const auto a_rows = static_cast<std::size_t>(A->row);
    for (auto i = 0u; i < a_rows; ++i) {
        const auto b_cols = static_cast<std::size_t>(B->clm);
        for (auto j = 0u; j < b_cols; ++j) {
            ARFloat sum = 0;
            const auto a_cols = static_cast<std::size_t>(A->clm);
            for (auto k = 0u; k < a_cols; ++k) {
                sum += A->m[i * a_cols + k] * B->m[k * b_cols + j];
            }
            result[i * b_cols + j] = sum;
        }
    }
    return result;
}

TEST(MatrixSelfInv, InversePropertyCheck) {
    // Deterministic seed for reproducibility
    std::mt19937 rng(42);
    std::uniform_real_distribution<ARFloat> dist(-5.0, 5.0);

    for (int trial = 0; trial < 5; ++trial) {
        constexpr int N = 3;
        auto M = makeMat(N, N);
        // Fill with random values
        for (int i = 0; i < N * N; ++i) {
            M->m[i] = dist(rng);
        }

        // To avoid singular matrices, add identity scaling
        for (int i = 0; i < N; ++i) {
            M->m[i * N + i] += static_cast<ARFloat>(N);
        }

        // Make a copy for later multiplication
        auto Mcopy = makeMat(N, N);
        std::copy_n(M->m, N * N, Mcopy->m);

        const auto ret = Matrix::selfInv(M.get());
        ASSERT_EQ(ret, 0) << "Matrix inversion failed on trial " << trial;

        // Multiply M⁻¹ * M_original ≈ I
        const auto prod = multiplyRaw(M.get(), Mcopy.get());

        for (auto i = 0u; i < N; ++i) {
            for (auto j = 0u; j < N; ++j) {
                const ARFloat expected = (i == j) ? 1.0f : 0.0f;
                EXPECT_NEAR(prod[i * N + j], expected, 1e-4)
                    << "Mismatch at (" << i << "," << j << ") in trial " << trial;
            }
        }
    }
}

