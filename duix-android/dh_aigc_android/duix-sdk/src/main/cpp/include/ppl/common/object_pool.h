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

#ifndef _ST_HPC_PPL_COMMON_OBJECT_POOL_H_
#define _ST_HPC_PPL_COMMON_OBJECT_POOL_H_

#include "ppl/common/generic_cpu_allocator.h"
#include "ppl/common/lock_utils.h"
#include <vector>
#include <utility> // std::forward

namespace ppl { namespace common {

template <typename T, typename LockType = DummyLock>
class ObjectPool final {
public:
    ObjectPool() {
        Reserve();
    }
    ~ObjectPool() {
        Clear();
    }

    void Clear() {
        WriteLockGuard<LockType> __guard__(&lock_);

        for (auto x = slabs_.begin(); x != slabs_.end(); ++x) {
            allocator_.Free(*x);
        }
        slabs_.clear();
        free_objects_.clear();
    }

    template <typename... Args>
    T* Alloc(Args&&... args) {
        WriteLockGuard<LockType> __guard__(&lock_);

        if (free_objects_.empty()) {
            Reserve();
            if (free_objects_.empty()) {
                return nullptr;
            }
        }

        auto o = free_objects_.back();
        free_objects_.pop_back();

        return new (o) T(std::forward<Args>(args)...);
    }

    void Free(T* obj) {
        if (obj) {
            WriteLockGuard<LockType> __guard__(&lock_);
            obj->~T();
            free_objects_.push_back(obj);
        }
    }

private:
    void Reserve() {
        auto slab = static_cast<T*>(allocator_.Alloc(sizeof(T) * PREALLOC_NUM));
        if (!slab) {
            return;
        }

        slabs_.push_back(slab);

        free_objects_.reserve(free_objects_.size() + PREALLOC_NUM);
        for (uint32_t i = 0; i < PREALLOC_NUM; ++i) {
            free_objects_.push_back(slab);
            ++slab;
        }
    }

private:
    static constexpr uint32_t PREALLOC_NUM = 32;

private:
    LockType lock_;
    std::vector<T*> free_objects_;
    std::vector<void*> slabs_;
    GenericCpuAllocator allocator_;

private:
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
};

}} // namespace ppl::common

#endif
