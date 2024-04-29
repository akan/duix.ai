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

#ifndef _ST_HPC_PPL_COMMON_FILE_MAPPING_H_
#define _ST_HPC_PPL_COMMON_FILE_MAPPING_H_

#include "ppl/common/retcode.h"
#include <limits> // UINT64_MAX

#ifdef _MSC_VER
#define NO_MINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>
#endif

namespace ppl { namespace common {

class FileMapping final {
public:
    static constexpr uint32_t READ = 1;
    static constexpr uint32_t WRITE = 2;

private:
    static constexpr uint32_t MAX_MSG_BUF_SIZE = 1024;

public:
    FileMapping();
    ~FileMapping();

    FileMapping(FileMapping&&);
    void operator=(FileMapping&&);

    /**
       @param permission MUST be one of: READ_ONLY, WRITE_ONLY or READ_WRITE
       @param offset no alignment is required.
    */
    ppl::common::RetCode Init(const char* filename, uint32_t permission, uint64_t offset = 0,
                              uint64_t length = UINT64_MAX);
    char* GetData() {
        return static_cast<char*>(start_);
    }
    const char* GetData() const {
        return static_cast<const char*>(start_);
    }
    uint64_t GetSize() const {
        return size_;
    }
    const char* GetErrorMessage() const {
        return error_message_;
    }

private:
    void DoMove(FileMapping&& fm);
    void Destroy();

private:
#ifdef _MSC_VER
    HANDLE h_file_;
    HANDLE h_map_file_;
#else
    int fd_;
#endif
    void* base_;
    void* start_;
    uint64_t size_;
    char error_message_[MAX_MSG_BUF_SIZE];

private:
    FileMapping(const FileMapping&) = delete;
    FileMapping& operator=(const FileMapping&) = delete;
};

}} // namespace ppl::common

#endif
