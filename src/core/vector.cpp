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
#include <cassert>

#include <ARToolKitPlus/Tracker.h>
#include <ARToolKitPlus/matrix.h>
#include <ARToolKitPlus/vector.h>

namespace ARToolKitPlus::Vector {

// from vAlloc.c
ARVec*
alloc(int clm) {
    ARVec *v;

    v = (ARVec *) malloc(sizeof(ARVec));
    if (v == nullptr)
        return nullptr;

    v->v = (ARFloat *) malloc(sizeof(ARFloat) * clm);
    if (v->v == nullptr) {
        free(v);
        return nullptr;
    }

    v->clm = clm;

    return v;
}

// from vFree.c
int free(ARVec *v) {
    ::free(v->v);
    ::free(v);

    return 0;
}

// from vHouse.c
ARFloat household(const ARVec *x) {

    auto s = sqrt(innerproduct(x, x));

    if (s != 0.0) {
        if (x->v[0] < 0)
            s = -s;
        x->v[0] += s;
        const ARFloat t = static_cast<ARFloat>(1) / sqrt(x->v[0] * s);
        for (int i = 0; i < x->clm; i++) {
            x->v[i] *= t;
        }
    }

    return (-s);
}

// from vInnerP.c
ARFloat innerproduct(const ARVec *x, const ARVec *y) {
    ARFloat result = 0.0;

    assert(x->clm == y->clm);

    for (int i = 0; i < x->clm; i++) {
        result += x->v[i] * y->v[i];
    }

    return result;
}

// from vTridiag.c
int tridiagonalize(const ARMat *a, const ARVec *d, const ARVec *e) {

    int i;
    int j;
    int k;

    if (a->clm != a->row)
        return (-1);
    if (a->clm != d->clm)
        return (-1);
    if (a->clm != e->clm + 1)
        return (-1);
    const int dim = a->clm;

    for (k = 0; k < dim - 2; k++) {
        const auto v = &(a->m[k * dim]);
        d->v[k] = v[k];

        ARVec wv1{};
        ARVec wv2{};
        wv1.clm = dim - k - 1;
        wv1.v = &(v[k + 1]);
        e->v[k] = household(&wv1);
        if (e->v[k] == 0.0)
            continue;

        for (i = k + 1; i < dim; i++) {
            ARFloat s = 0.0;
            for (j = k + 1; j < i; j++) {
                s += a->m[j * dim + i] * v[j];
            }
            for (j = i; j < dim; j++) {
                s += a->m[i * dim + j] * v[j];
            }
            d->v[i] = s;
        }

        wv1.clm = wv2.clm = dim - k - 1;
        wv1.v = &(v[k + 1]);
        wv2.v = &(d->v[k + 1]);
        const auto t = innerproduct(&wv1, &wv2) / 2;
        for (i = dim - 1; i > k; i--) {
            const ARFloat p = v[i];
            const ARFloat q = d->v[i] -= t * p;
            for (j = i; j < dim; j++) {
                a->m[i * dim + j] -= p * (d->v[j]) + q * v[j];
            }
        }
    }

    if (dim >= 2) {
        d->v[dim - 2] = a->m[(dim - 2) * dim + (dim - 2)];
        e->v[dim - 2] = a->m[(dim - 2) * dim + (dim - 1)];
    }

    if (dim >= 1)
        d->v[dim - 1] = a->m[(dim - 1) * dim + (dim - 1)];

    for (k = dim - 1; k >= 0; k--) {
        const auto v = &(a->m[k * dim]);
        if (k < dim - 2) {
            for (i = k + 1; i < dim; i++) {
                ARVec wv1{};
                ARVec wv2{};
                wv1.clm = wv2.clm = dim - k - 1;
                wv1.v = &(v[k + 1]);
                wv2.v = &(a->m[i * dim + k + 1]);
                const auto t = innerproduct(&wv1, &wv2);
                for (j = k + 1; j < dim; j++)
                    a->m[i * dim + j] -= t * v[j];
            }
        }
        for (i = 0; i < dim; i++)
            v[i] = 0.0;
        v[k] = 1;
    }

    return 0;
}

} // namespace ARToolKitPlus::Vector
