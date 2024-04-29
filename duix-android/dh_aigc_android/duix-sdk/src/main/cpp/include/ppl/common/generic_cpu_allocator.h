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

#ifndef _ST_HPC_PPL_COMMON_GENERIC_CPU_ALLOCATOR_H_
#define _ST_HPC_PPL_COMMON_GENERIC_CPU_ALLOCATOR_H_

#include "ppl/common/allocator.h"
#include "ppl/common/sys.h"
#include <new>
#include <utility> // std::forward

namespace ppl { namespace common {

class GenericCpuAllocator final : public Allocator {
public:
    GenericCpuAllocator(uint64_t alignment = 64) : alignment_(alignment) {}

    template <typename T, typename... Args>
    T* TypedAlloc(Args&&... args) {
        auto obj = (T*)Alloc(sizeof(T));
        if (obj) {
            new (obj) T(std::forward<Args>(args)...);
        }
        return obj;
    }

    template <typename T>
    void TypedFree(T* obj) {
        if (obj) {
            obj->~T();
            Free(obj);
        }
    }

    void* Alloc(uint64_t size) override {
        return ppl::common::AlignedAlloc(size, alignment_);
    }

    void Free(void* ptr) override {
        ppl::common::AlignedFree(ptr);
    }

private:
    uint64_t alignment_;
};

}} // namespace ppl::common

#endif
