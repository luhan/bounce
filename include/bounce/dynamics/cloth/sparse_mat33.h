/*
* Copyright (c) 2016-2016 Irlan Robson http://www.irlan.net
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_SPARSE_MAT_33_H
#define B3_SPARSE_MAT_33_H

#include <bounce/common/math/mat33.h>
#include <bounce/dynamics/cloth/diag_mat33.h>
#include <bounce/dynamics/cloth/dense_vec3.h>

// A static sparse matrix stored in Compressed Sparse Row (CSR) format.
// It's efficient when using in iterative solvers such as CG method, where  
// the coefficient matrix must be multiplied with a vector at each iteration.
// See https://en.wikipedia.org/wiki/Sparse_matrix
struct b3SparseMat33
{
	b3SparseMat33() { }

	b3SparseMat33(u32 _M, u32 _N,
		u32 _valueCount, b3Mat33* _values,
		u32* _row_ptrs, u32* _cols)
	{
		M = _M;
		N = _N;

		values = _values;
		valueCount = _valueCount;

		row_ptrs = _row_ptrs;
		cols = _cols;
	}

	~b3SparseMat33()
	{

	}

	// Output any given row of the original matrix.
	// The given buffer must have size of greater or equal than M.
	void AssembleRow(b3Mat33* out, u32 row) const;

	// Decompresses this matrix into its original form.
	// The output matrix is stored in row-major order.
	// The given buffer must have size of greater or equal than M * N.
	void AssembleMatrix(b3Mat33* out) const;

	// Output the block diagonal part of the original matrix.
	// This matrix must be a square matrix.
	// The given buffer must have size of greater or equal than M.
	void AssembleDiagonal(b3DiagMat33& out) const;

	// Dimensions of the original 2D matrix
	u32 M;
	u32 N;

	// Non-zero values	
	b3Mat33* values;
	u32 valueCount;

	// Sparsity structure
	u32* row_ptrs; // pointers to the first non-zero value of each row (size is M + 1)
	u32* cols; // column indices for each non-zero value (size is valueCount)
};

inline void b3SparseMat33::AssembleRow(b3Mat33* out, u32 row) const
{
	B3_ASSERT(row < M + 1);

	// Start with zero row
	for (u32 i = 0; i < N; ++i)
	{
		out[i].SetZero();
	}

	for (u32 i = row_ptrs[row]; i < row_ptrs[row + 1]; ++i)
	{
		u32 col = cols[i];

		out[col] = values[i];
	}
}

inline void b3SparseMat33::AssembleMatrix(b3Mat33* out) const
{
	for (u32 i = 0; i < M; ++i)
	{
		AssembleRow(out + i * N, i);
	}
}

inline void b3SparseMat33::AssembleDiagonal(b3DiagMat33& out) const
{
	B3_ASSERT(M == N);

	for (u32 row = 0; row < M; ++row)
	{
		out[row].SetZero();

		for (u32 row_ptr = row_ptrs[row]; row_ptr < row_ptrs[row + 1]; ++row_ptr)
		{
			if (cols[row_ptr] > row)
			{
				break;
			}

			if (cols[row_ptr] == row)
			{
				out[row] = values[row_ptr];
				break;
			}
		}
	}
}

inline void b3Mul(b3DenseVec3& out, const b3SparseMat33& A, const b3DenseVec3& v)
{
	B3_ASSERT(A.N == out.n);

	for (u32 row = 0; row < A.N; ++row)
	{
		out[row].SetZero();

		for (u32 j = A.row_ptrs[row]; j < A.row_ptrs[row + 1]; ++j)
		{
			u32 col = A.cols[j];

			out[row] += A.values[j] * v[col];
		}
	}
}

inline b3DenseVec3 operator*(const b3SparseMat33& A, const b3DenseVec3& v)
{
	b3DenseVec3 result(v.n);
	b3Mul(result, A, v);
	return result;
}

inline void b3Add(b3SparseMat33& out, b3SparseMat33& A, const b3SparseMat33& B)
{
	B3_ASSERT(A.M == B.M);
	B3_ASSERT(A.N == B.N);

	B3_ASSERT(A.M == out.M);
	B3_ASSERT(A.N == out.N);

	// out = A
	for (u32 i = 0; i < A.valueCount; ++i)
	{
		out.values[i] = A.values[i];
		out.cols[i] = A.cols[i];
	}
	out.valueCount = A.valueCount;

	for (u32 i = 0; i < A.M + 1; ++i)
	{
		out.row_ptrs[i] = A.row_ptrs[i];
	}

	// out += B
	for (u32 i = 0; i < B.M; ++i)
	{
		for (u32 row_ptr_B = B.row_ptrs[i]; row_ptr_B < B.row_ptrs[i + 1]; ++row_ptr_B)
		{
			u32 col_B = B.cols[row_ptr_B];

			// Does A has a non-zero element that exist in B?
			u32 row_ptr_A = A.row_ptrs[i];
			while (row_ptr_A != A.row_ptrs[i + 1])
			{
				u32 col_A = A.cols[row_ptr_A];

				if (col_A > col_B)
				{
					break;
				}

				++row_ptr_A;
			}

			u32 col_A = A.cols[row_ptr_A];

			if (col_A == col_B)
			{
				out.values[col_A] += B.values[col_B];
			}
			else
			{
				out.values[out.valueCount] = B.values[col_B];
				out.cols[out.valueCount] = col_B;
				++out.valueCount;

				out.row_ptrs[i + 1] = out.row_ptrs[(i + 1) - 1] + col_A;
			}
		}
	}

	B3_ASSERT(out.valueCount <= A.N);
}

#endif