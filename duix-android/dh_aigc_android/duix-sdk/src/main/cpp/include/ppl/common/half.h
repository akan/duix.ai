// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef _ST_HPC_PPL_COMMON_HALF_H_
#define _ST_HPC_PPL_COMMON_HALF_H_
#include <stdint.h>
#include <stddef.h>

namespace ppl { namespace common {

/* Conversion between float and raw uint16_t
 * @ref half2float
 * @note The most of modern CPUs supports conversion
 * between float16 and float32, this is so called `f16c`.
 * But, unfortunately not all CPUs support `f16c`. If the
 * current does not support `f16c` the `half2float` and
 * `float2half` will call `half2float_soft` and `float2half_soft`.
 *****************************************************************/
float half2float(uint16_t half);
uint16_t float2half(float value);
float half2float_soft(uint16_t half);
uint16_t float2half_soft(float value);

struct float16_t {
    uint16_t bit16;

    float16_t() : bit16(0) {}

    float16_t(float value) : bit16(float2half(value)) {}

    inline float16_t& operator=(float other) {
        this->bit16 = float2half(other);
        return (*this);
    }
    inline float ToFloat32() const {
        return half2float(this->bit16);
    }
    inline uint16_t Raw16() const {
        return this->bit16;
    }
    inline operator float() const {
        return ToFloat32();
    }

    static float16_t FromRawU16(uint16_t bit16) {
        float16_t ret;
        ret.bit16 = bit16;
        return ret;
    }
};

inline bool operator==(float16_t a, float16_t b) {
    return a.bit16 == b.bit16;
}
inline bool operator!=(float16_t a, float16_t b) {
    return a.bit16 != b.bit16;
}
inline bool operator<(float16_t a, float16_t b) {
    return a.ToFloat32() < b.ToFloat32();
}

void ConvertFp32ToFp16(const void* fp32, void* fp16, size_t count);
void ConvertFp16ToFp32(const void* fp16, void* fp32, size_t count);

}} // namespace ppl::common
#endif
