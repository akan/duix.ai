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

#ifndef _ST_HPC_PPL_COMMON_TYPES_H_
#define _ST_HPC_PPL_COMMON_TYPES_H_

#include "ppl/common/half.h"
#include <stdint.h>

namespace ppl { namespace common {

enum {
    DATATYPE_UNKNOWN = 0,
    DATATYPE_UINT8 = 1,
    DATATYPE_UINT16 = 2,
    DATATYPE_UINT32 = 3,
    DATATYPE_UINT64 = 4,
    DATATYPE_FLOAT16 = 5,
    DATATYPE_FLOAT32 = 6,
    DATATYPE_FLOAT64 = 7,
    DATATYPE_BFLOAT16 = 8,
    DATATYPE_INT4B = 9,
    DATATYPE_INT8 = 10,
    DATATYPE_INT16 = 11,
    DATATYPE_INT32 = 12,
    DATATYPE_INT64 = 13,
    DATATYPE_BOOL = 14,
    DATATYPE_COMPLEX64 = 15,
    DATATYPE_COMPLEX128 = 16,
    DATATYPE_MAX,
};
typedef uint32_t datatype_t;

static inline const char* GetDataTypeStr(datatype_t dt) {
    static const char* data_type_str[] = {
        "UNKNOWN", "UINT8", "UINT16", "UINT32", "UINT64", "FLOAT16", "FLOAT32",   "FLOAT64",    "BFLOAT16",
        "INT4B",   "INT8",  "INT16",  "INT32",  "INT64",  "BOOL",    "COMPLEX64", "COMPLEX128",
    };
    if (dt >= DATATYPE_MAX) {
        return data_type_str[0];
    }
    return data_type_str[dt];
}

static inline uint32_t GetSizeOfDataType(datatype_t dt) {
    static const uint32_t data_type_size[] = {
        0, // unknown
        1, // uint8_t
        2, // uint16_t
        4, // uint32_t
        8, // uint64_t
        2, // float16_t
        4, // float32
        8, // float64
        2, // BFLOAT16
        1, // INT4B
        1, // int8_t
        2, // int16_t
        4, // int32_t
        8, // int64_t
        1, // bool
        8, // COMPLEX64
        16, // COMPLEX128
    };
    return data_type_size[dt];
}

enum {
    DATAFORMAT_UNKNOWN = 0,
    DATAFORMAT_NDARRAY = 1,
    DATAFORMAT_NHWC8 = 2,
    DATAFORMAT_NHWC16 = 3,
    DATAFORMAT_N2CX = 4,
    DATAFORMAT_N4CX = 5,
    DATAFORMAT_N8CX = 6,
    DATAFORMAT_N16CX = 7,
    DATAFORMAT_N32CX = 8,
    DATAFORMAT_NHWC = 9,
    DATAFORMAT_MAX,
};
typedef uint32_t dataformat_t;

static inline const char* GetDataFormatStr(dataformat_t df) {
    static const char* data_format_str[] = {
        "UNKNOWN", "NDARRAY", "NHWC8", "NHWC16", "N2CX", "N4CX", "N8CX", "N16CX", "N32CX", "NHWC",
    };

    if (df >= DATAFORMAT_MAX) {
        return data_format_str[0];
    }
    return data_format_str[df];
}

}} // namespace ppl::common

#endif
