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

#ifndef _ST_HPC_PPL_COMMON_ARM_SYSINFO_H_
#define _ST_HPC_PPL_COMMON_ARM_SYSINFO_H_

namespace ppl { namespace common {

enum {
    // isa versions
    ISA_ARMV8 = 0x1,
    ISA_ARMV8_2 = 0x2,
    ISA_ARMV8_6 = 0x4,
    ISA_ARMV9 = 0x8,
    // sub features
    ISA_ARMV8_2_BF16 = (0x1 << 16),
    ISA_ARMV8_2_I8MM = (0x2 << 16)
};

}} // namespace ppl::common

#endif
