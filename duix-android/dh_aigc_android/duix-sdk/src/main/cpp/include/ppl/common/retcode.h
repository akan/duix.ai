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

#ifndef _ST_HPC_PPL_COMMON_RETCODE_H_
#define _ST_HPC_PPL_COMMON_RETCODE_H_

#include <stdint.h>

namespace ppl { namespace common {

typedef uint32_t RetCode;

enum {
    RC_SUCCESS = 0,
    RC_OTHER_ERROR,
    RC_UNSUPPORTED,
    RC_OUT_OF_MEMORY,
    RC_INVALID_VALUE,
    RC_EXISTS,
    RC_NOT_FOUND,
    RC_PERMISSION_DENIED,
    RC_HOST_MEMORY_ERROR,
    RC_DEVICE_MEMORY_ERROR,
    RC_DEVICE_RUNTIME_ERROR,
    RC_OUT_OF_RANGE,
};

static inline const char* GetRetCodeStr(RetCode rc) {
    static const char* code_str[] = {
        "success",       "other error", "unsupported", "out of memory",
        "invalid value", "exists",      "not found",   "perimission denied",
        "host memory error", "device memory error", "device runtime error",
        "out of range",
    };
    return code_str[rc];
}

}} // namespace ppl::common

#endif
