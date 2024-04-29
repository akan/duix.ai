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

#ifndef _ST_HPC_PPL_COMMON_SYS_H_
#define _ST_HPC_PPL_COMMON_SYS_H_

#include <stdint.h>

namespace ppl { namespace common {

typedef uint32_t isa_t;
static constexpr uint32_t ISA_UNKNOWN = 0;

struct CpuInfo {
    isa_t isa;
    uint64_t l1_cache_size;
    uint64_t l2_cache_size;
    uint64_t l3_cache_size;
    char vendor_id[64];
};

const CpuInfo* GetCpuInfo(int which = 0);

static inline uint64_t GetCpuCacheL1(int which = 0) {
    return GetCpuInfo(which)->l1_cache_size;
}
static inline uint64_t GetCpuCacheL2(int which = 0) {
    return GetCpuInfo(which)->l2_cache_size;
}
static inline uint64_t GetCpuCacheL3(int which = 0) {
    return GetCpuInfo(which)->l3_cache_size;
}
static inline const char* GetCpuVendor(int which = 0) {
    return GetCpuInfo(which)->vendor_id;
}

static inline bool CpuSupports(isa_t flag, int which = 0) {
    return (GetCpuInfo(which)->isa & flag);
}

static inline isa_t GetCpuISA(int which = 0) {
    return GetCpuInfo(which)->isa;
}

void GetCPUInfoByCPUID(CpuInfo* info);
void GetCPUInfoByRun(CpuInfo* info);

void* AlignedAlloc(uint64_t size, uint32_t alignment);
void AlignedFree(void* p);

}} // namespace ppl::common

#endif
