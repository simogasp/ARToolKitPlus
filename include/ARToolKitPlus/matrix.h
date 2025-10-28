/**
 * @file matrix.h
 * @brief Matrix operations for ARToolKitPlus
 *
 * This file provides matrix data structures and operations including
 * allocation, duplication, multiplication, and inversion.
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

#ifndef __ARTOOLKITMATRIX_HEADERFILE__
#define __ARTOOLKITMATRIX_HEADERFILE__

#include <ARToolKitPlus/config.h>

/**
 * @brief Matrix memory layout
 *
 * Matrix data is stored in row-major order:
 * @code
 *  <---- clm --->
 *  [ 10  20  30 ] ^
 *  [ 20  10  15 ] |
 *  [ 12  23  13 ] row
 *  [ 20  10  15 ] |
 *  [ 13  14  15 ] v
 * @endcode
 */

namespace ARToolKitPlus {

/**
 * @struct ARMat
 * @brief Matrix structure for ARToolKitPlus
 *
 * A dynamic matrix structure that holds floating-point values in row-major order.
 */
struct AR_EXPORT ARMat {
    ARFloat *m;   ///< Pointer to the matrix data array (row-major order)
    int row;      ///< Number of rows in the matrix
    int clm;      ///< Number of columns in the matrix
};

/**
 * @namespace Matrix
 * @brief Namespace containing matrix operations
 */
namespace Matrix {

/**
 * @def ARELEM0(mat,r,c)
 * @brief Access matrix element using 0-based indexing
 * @param mat Pointer to the ARMat structure
 * @param r Row index (0-based)
 * @param c Column index (0-based)
 * @return Reference to the matrix element at position (r,c)
 */
#define ARELEM0(mat,r,c) ((mat)->m[(r)*((mat)->clm)+(c)])

/**
 * @def ARELEM1(mat,row,clm)
 * @brief Access matrix element using 1-based indexing
 * @param mat Pointer to the ARMat structure
 * @param row Row index (1-based)
 * @param clm Column index (1-based)
 * @return Reference to the matrix element at position (row-1,clm-1)
 */
#define ARELEM1(mat,row,clm) ARELEM0(mat,row-1,clm-1)

/**
 * @brief Allocates memory for a new matrix
 *
 * Creates a new ARMat structure and allocates memory for the specified
 * number of rows and columns.
 *
 * @param row Number of rows in the matrix
 * @param clm Number of columns in the matrix
 * @return Pointer to the newly allocated ARMat, or NULL if allocation fails
 */
AR_EXPORT ARMat *alloc(int row, int clm);

/**
 * @brief Frees memory allocated for a matrix
 *
 * Deallocates memory for both the matrix data array and the ARMat structure.
 *
 * @param m Pointer to the matrix to be freed
 * @return 0 on success
 */
AR_EXPORT int free(ARMat *m);

/**
 * @brief Duplicates a matrix into an existing matrix
 *
 * Copies all elements from the source matrix to the destination matrix.
 * Both matrices must have the same dimensions.
 *
 * @param dest Pointer to the destination matrix (must be pre-allocated)
 * @param source Pointer to the source matrix to copy from
 * @return 0 on success
 * @pre dest->row == source->row && dest->clm == source->clm
 */
AR_EXPORT int dup(ARMat *dest, ARMat *source);

/**
 * @brief Allocates and duplicates a matrix
 *
 * Creates a new matrix with the same dimensions as the source matrix
 * and copies all elements.
 *
 * @param source Pointer to the source matrix to duplicate
 * @return Pointer to the newly allocated duplicate matrix, or NULL if allocation fails
 */
AR_EXPORT ARMat *allocDup(ARMat *source);

/**
 * @brief Multiplies two matrices
 *
 * Computes the matrix product dest = a * b. The matrices must have
 * compatible dimensions for multiplication.
 *
 * @param dest Pointer to the destination matrix that will store the result
 * @param a Pointer to the first matrix (left operand)
 * @param b Pointer to the second matrix (right operand)
 * @return 0 on success
 * @pre a->clm == b->row (inner dimensions must match)
 * @pre dest->row == a->row && dest->clm == b->clm (result dimensions must match)
 */
AR_EXPORT int mul(ARMat *dest, ARMat *a, ARMat *b);

/**
 * @brief Inverts a matrix in-place
 *
 * Computes the inverse of a square matrix using Gaussian elimination with
 * partial pivoting. The original matrix is replaced with its inverse.
 * The function uses a threshold of 1.0e-10 to determine matrix singularity.
 *
 * @param m Pointer to the square matrix to invert (modified in-place)
 * @return 0 on success, -1 if the matrix is singular or cannot be inverted
 * @pre m->row == m->clm (matrix must be square)
 */
AR_EXPORT int selfInv(ARMat *m);

} // namespace Matrix


} // namespace ARToolKitPlus


#endif // __ARTOOLKITMATRIX_HEADERFILE__
