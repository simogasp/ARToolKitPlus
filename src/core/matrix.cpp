/**
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

#include <cmath>
#include <cstdlib>
#include <cassert>

#include <ARToolKitPlus/matrix.h>

namespace ARToolKitPlus::Matrix {

#define MATRIX(name,x,y,width)  ( *(name + (width) * (x) + (y)) )

// declaration of internal helper functions (see end of file)
// static ARFloat mdet(ARFloat *ap, int dimen, int rowa);
static ARFloat *minv(ARFloat *ap, int dimen, int rowa);

// from mAlloc.
ARMat* alloc(int row, int clm) {

    ARMat* m = (ARMat*)malloc(sizeof(ARMat));
	if (m == nullptr)
		return nullptr;

	m->m = (ARFloat *) malloc(sizeof(ARFloat) * static_cast<std::size_t>(row * clm));
	if (m->m == nullptr) {
		free(m);
		return nullptr;
	} else {
		m->row = row;
		m->clm = clm;
	}

	return m;
}

// from mFree.c
int free(ARMat *m) {
	::free(m->m);
	::free(m);

	return 0;
}

// from mAllocDup.c
ARMat* allocDup(const ARMat *source) {

    ARMat* dest = alloc(source->row, source->clm);
	if (dest == nullptr)
		return nullptr;

	if (dup(dest, source) < 0) {
		free(dest);
		return nullptr;
	}

	return dest;
}

// from mDup.c
int dup(const ARMat *dest, const ARMat *source) {

    assert(dest->row == source->row && dest->clm == source->clm);

	for (int r = 0; r < source->row; ++r) {
		for (int c = 0; c < source->clm; ++c) {
			ARELEM0(dest, r, c) = ARELEM0(source, r, c);
		}
	}
	return 0;
}

// from mMul.c
int mul(const ARMat *dest, const ARMat *a, const ARMat *b) {

	assert(a->clm == b->row && dest->row == a->row && dest->clm == b->clm);

	for (int r = 0; r < dest->row; ++r) {
		for (int c = 0; c < dest->clm; ++c) {
			ARELEM0(dest, r, c) = 0.0;
			for (int i = 0; i < a->clm; ++i) {
				ARELEM0(dest, r, c) += ARELEM0(a, r, i) * ARELEM0(b, i, c);
			}
		}
	}

	return 0;
}

// from mSelfInv.c
int selfInv(const ARMat *m) {
	if (minv(m->m, m->row, m->row) == nullptr)
		return -1;

	return 0;
}

// from mSelfInv.c -- MATRIX inverse function
static ARFloat* minv(ARFloat *ap, int dimen, int rowa) {
	ARFloat *wap;
	ARFloat *wcp;
	ARFloat *wbp;/* work pointer                 */
	int i{0};
	int j{0};
	int ip = 0;
    int nos[50];
    ARFloat p;
    ARFloat pbuf;
    ARFloat work;

    constexpr auto epsl = static_cast<ARFloat>(1.0e-10); /* Threshold value      */

	if (dimen == 0)
	{
	    return nullptr; /* check size */
	}
    if (dimen == 1)
	{
		*ap = static_cast<ARFloat>(1.0) / *ap;
		return ap; /* 1 dimension */
	}

	for (int n = 0; n < dimen; ++n)
		nos[n] = n;

	for (int n = 0; n < dimen; ++n) {
		wcp = ap + n * rowa;

		for (i = n, wap = wcp, p = 0.0; i < dimen; i++, wap += rowa)
			if (p < (pbuf = (ARFloat) fabs(*wap))) {
				p = pbuf;
				ip = i;
			}
		if (p <= epsl)
			return nullptr;

		const int nwork = nos[ip];
		nos[ip] = nos[n];
		nos[n] = nwork;

		for (j = 0, wap = ap + ip * rowa, wbp = wcp; j < dimen; j++) {
			work = *wap;
			*wap++ = *wbp;
			*wbp++ = work;
		}

		for (j = 1, wap = wcp, work = *wcp; j < dimen; j++, wap++)
			*wap = *(wap + 1) / work;
		*wap = static_cast<ARFloat>(1.0) / work;

		for (i = 0; i < dimen; i++) {
			if (i != n) {
				wap = ap + i * rowa;
				for (j = 1, wbp = wcp, work = *wap; j < dimen; j++, wap++, wbp++)
					*wap = *(wap + 1) - work * (*wbp);
				*wap = -work * (*wbp);
			}
		}
	}

	for (int n = 0; n < dimen; n++) {
		for (j = n; j < dimen; j++)
			if (nos[j] == n)
				break;
		nos[j] = nos[n];
		for (i = 0, wap = ap + j, wbp = ap + n; i < dimen; i++, wap += rowa, wbp += rowa) {
			work = *wap;
			*wap = *wbp;
			*wbp = work;
		}
	}
	return ap;
}

} // namespace ARToolKitPlus::Matrix
