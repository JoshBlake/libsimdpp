/*  Copyright (C) 2011-2014  Povilas Kanapickas <povilas@radix.lt>

    Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
            http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LIBSIMDPP_SIMDPP_DETAIL_INSN_F_TRUNC_H
#define LIBSIMDPP_SIMDPP_DETAIL_INSN_F_TRUNC_H

#ifndef LIBSIMDPP_SIMD_H
    #error "This file must be included through simd.h"
#endif

#include <cmath>
#include <simdpp/types.h>
#include <simdpp/core/f_ceil.h>

namespace simdpp {
namespace SIMDPP_ARCH_NAMESPACE {
namespace detail {
namespace insn {


SIMDPP_INL float32x4 i_trunc(const float32x4& a)
{
#if SIMDPP_USE_NULL || SIMDPP_USE_NEON_NO_FLT_SP
    float32x4 r;
    for (unsigned i = 0; i < a.length; i++) {
        r.el(i) = std::trunc(a.el(i));
    }
    return r;
#elif SIMDPP_USE_SSE4_1
    return _mm_round_ps(a, 3); // 3 = i_truncate
#elif SIMDPP_USE_NEON64
    return vrndq_f32(a); // FIXME: in ARM8 A32 too
#elif SIMDPP_USE_SSE2 || SIMDPP_USE_NEON_FLT_SP
    //check if the value is not too large
    float32x4 af = abs(a);
    mask_float32x4 mask_range = cmp_le(af, 8388607.0f);

    // don't change the sign of negative zero
    mask_float32x4 mask_nonzero = cmp_gt(af, 0);
    mask_float32x4 mask = bit_and(mask_range, mask_nonzero);

    //truncate
    int32x4 ia = to_int32(a);
    float32x4 fa = to_float32(ia);

    //combine the results
    return blend(fa, a, mask);     // takes care of NaNs
#elif SIMDPP_USE_ALTIVEC
    return vec_trunc((__vector float)a);
#endif
}

#if SIMDPP_USE_AVX
SIMDPP_INL float32x8 i_trunc(const float32x8& a)
{
    return _mm256_round_ps(a, 3); // 3 = i_truncate
}
#endif

#if SIMDPP_USE_AVX512F
SIMDPP_INL float32<16> i_trunc(const float32<16>& a)
{
    return _mm512_roundscale_ps(a, 0x13); // scale by 1, truncate
}
#endif

template<unsigned N> SIMDPP_INL
float32<N> i_trunc(const float32<N>& a)
{
    SIMDPP_VEC_ARRAY_IMPL1(float32<N>, i_trunc, a);
}


} // namespace insn
} // namespace detail
} // namespace SIMDPP_ARCH_NAMESPACE
} // namespace simdpp

#endif

