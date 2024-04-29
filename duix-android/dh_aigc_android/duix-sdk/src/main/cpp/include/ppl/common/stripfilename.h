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

#ifndef _ST_HPC_PPL_COMMON_STRIPFILENAME_H_
#define _ST_HPC_PPL_COMMON_STRIPFILENAME_H_

#include <string.h>

namespace ppl { namespace common {

/*!
 * @stripfilename
 * Get file name from the full path.
 * ./xxx/abc.cc -> abc.cc
 * @remark
 * Modern compiler can optimize the call
 * of the function, if the parameter is
 * a compilation runtime constant, such
 * as __FILE__
 ************************************/
inline const char* stripfilename(const char* filename) {
    const size_t l = strlen(filename);
    const char* end = filename + l;
    while (end >= filename) {
        if (
#ifdef _MSC_VER
            *end == '/' || *end == '\\'
#else
            *end == '/'
#endif
        ) {
            return end + 1;
        }
        // "--" is ok. For filename is not null.
        --end;
    }
    return filename;
}

}} // namespace ppl::common
#endif
