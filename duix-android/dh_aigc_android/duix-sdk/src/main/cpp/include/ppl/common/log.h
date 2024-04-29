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

#ifndef _ST_HPC_PPL_COMMON_LOG_H_
#define _ST_HPC_PPL_COMMON_LOG_H_

#include "ppl/common/retcode.h"
#include "ppl/common/str_utils.h"
#include <ctime>
#include <string>
#include <type_traits> // static_assert

namespace ppl { namespace common {

enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_FATAL = 4,
    LOG_LEVEL_MAX,
};

class Logger {
public:
    Logger(uint32_t level) : level_(level) {}
    virtual ~Logger() {}

    void SetLogLevel(uint32_t l) {
        level_ = l;
    }
    uint32_t GetLogLevel() const {
        return level_;
    }

    virtual void Write(uint32_t level, const char* log_prefix, uint64_t prefix_len, const char* log_content,
                       uint64_t content_len) = 0;

private:
    uint32_t level_;
};

class LogMessage final {
public:
    LogMessage(uint32_t, Logger*, const char* filename, uint32_t linenum);
    ~LogMessage();

    template <typename T>
    LogMessage& operator<<(const T& value) {
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "is not a number type.");
        content_.append(ToString(value));
        return *this;
    }

    LogMessage& operator<<(const char* str) {
        if (str) {
            content_.append(str);
        }
        return *this;
    }
    LogMessage& operator<<(char* str) {
        if (str) {
            content_.append(str);
        }
        return *this;
    }
    LogMessage& operator<<(const std::string& str) {
        content_.append(str);
        return *this;
    }

    LogMessage& operator<<(void* p);
    LogMessage& operator<<(const void* p);

private:
    uint32_t level_;
    uint32_t linenum_;
    Logger* logger_;
    const char* filename_;
    std::string content_;
};

class DummyOperatorPlaceHolder final {
public:
    void operator&(const LogMessage&) const {}
};

void SetCurrentLogger(Logger*);
Logger* GetCurrentLogger();

#define LOG(___level___)                                                                                             \
    (ppl::common::LOG_LEVEL_##___level___ < ppl::common::GetCurrentLogger()->GetLogLevel())                          \
        ? (void)0                                                                                                    \
        : ppl::common::DummyOperatorPlaceHolder() &                                                                  \
            ppl::common::LogMessage(ppl::common::LOG_LEVEL_##___level___, ppl::common::GetCurrentLogger(), __FILE__, \
                                    __LINE__)

}} // namespace ppl::common

#endif
