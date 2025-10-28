/**
 * @file vector.h
 * @brief Vector operations for ARToolKitPlus
 *
 * This file provides vector data structures and operations including
 * allocation, mathematical operations, and matrix transformations.
 *
 * Copyright (C) 2010  ARToolkitPlus Authors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *  Daniel Wagner
 */

#ifndef __ARTOOLKITVECTOR_HEADERFILE__
#define __ARTOOLKITVECTOR_HEADERFILE__

#include <ARToolKitPlus/config.h>

namespace ARToolKitPlus {

struct ARMat;

/**
 * @struct ARVec
 * @brief Vector structure for ARToolKitPlus
 *
 * A dynamic vector structure that holds floating-point values.
 */
struct AR_EXPORT ARVec {
	ARFloat *v;   ///< Pointer to the vector data array
	int clm;      ///< Number of columns (elements) in the vector
};

/**
 * @namespace Vector
 * @brief Namespace containing vector operations
 */
namespace Vector {

/**
 * @brief Allocates memory for a new vector
 *
 * Creates a new ARVec structure and allocates memory for the specified
 * number of elements.
 *
 * @param clm Number of columns (elements) in the vector
 * @return Pointer to the newly allocated ARVec, or NULL if allocation fails
 */
AR_EXPORT ARVec * alloc(int clm);

/**
 * @brief Frees memory allocated for a vector
 *
 * Deallocates memory for both the vector data array and the ARVec structure.
 *
 * @param v Pointer to the vector to be freed
 * @return 0 on success
 */
AR_EXPORT int free(ARVec *v);

/**
 * @brief Computes a Householder vector for reflection
 *
 * Generates a normalized Householder vector that can be used to create a
 * Householder reflection matrix. The function modifies the input vector in-place:
 * - Computes the L2 norm s of the vector
 * - Adds s to the first element (with appropriate sign)
 * - Normalizes so that the resulting vector has norm sqrt(2)
 *
 * This is a preparatory step used in QR decomposition and matrix tridiagonalization.
 * The resulting vector v satisfies: H = I - v*v^T, where H is the Householder matrix.
 *
 * @param x Pointer to the vector to transform (modified in-place to become the Householder vector)
 * @return The computed scale factor (-s), representing the magnitude to reflect across
 * @note If the input vector has zero norm, it remains unchanged and returns 0
 */
AR_EXPORT ARFloat household(const ARVec *x);

/**
 * @brief Computes the inner product (dot product) of two vectors
 *
 * Calculates the sum of element-wise products of two vectors.
 * The vectors must have the same dimension.
 *
 * @param x Pointer to the first vector
 * @param y Pointer to the second vector
 * @return The inner product of the two vectors
 * @pre x->clm == y->clm (vectors must have same dimension)
 */
AR_EXPORT ARFloat innerproduct(const ARVec *x, const ARVec *y);

/**
 * @brief Tridiagonalizes a symmetric matrix using Householder transformations
 *
 * Reduces a symmetric matrix to tridiagonal form using Householder
 * transformations. The transformation matrix is stored back in matrix 'a',
 * while the diagonal and sub-diagonal elements are stored in vectors 'd' and 'e'.
 *
 * @param a Pointer to the square symmetric matrix to tridiagonalize (modified in-place)
 * @param d Pointer to vector that will store the diagonal elements
 * @param e Pointer to vector that will store the sub-diagonal elements
 * @return 0 on success, -1 if dimensions are invalid
 * @pre a->clm == a->row (matrix must be square)
 * @pre a->clm == d->clm (diagonal vector must match matrix dimension)
 * @pre a->clm == e->clm + 1 (sub-diagonal vector dimension must be matrix dimension - 1)
 */
AR_EXPORT int tridiagonalize(const ARMat *a, const ARVec *d, const ARVec *e);

} // namespace Vector

} // namespace ARToolKitPlus


#endif // __ARTOOLKITVECTOR_HEADERFILE__
