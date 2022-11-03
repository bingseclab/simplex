//=== simplex/tools/benchmark/benchmark.h - Benchmark -*- C -*-===//
//
// \date 2020
// \author Matthew Cole
// \copyright Apache License 2.0
// SPDX-License-Identifier: Apache-2.0

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//    http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 
//===----------------------------------------------------------------------===//
///
/// \file benchmark.h
/// Performance benchmark 
///
//===----------------------------------------------------------------------===//



#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <cstddef> // size_t

/// \def KB Number of bytes in a kibibyte (2**10)
#define KB (1024)
/// \def MB Number of bytes in a mebibyte (2**20)
#define MB (1048576)

#define RUNS (100)
#define ITERS (1000000)

///
/// @brief      { Randomize a buffer of memory. }
/// @warning    {The buffer will contain low-quality randomized values in [1,127],
///             with the final character null-terminating the buffer.}
/// @param      ptr   Pointer to the start of the buffer.
/// @param[in]  sz    Size of the buffer.
///
void randomize(char *ptr, size_t sz);

///
/// @brief {Copy src to dst, but invert one random byte in dst}
/// @param      dst   Pointer to the source buffer.
/// @param		src   Pointer to the destination buffer.
/// @param[in]  sz    Size of the buffers.
void corrupt(char *dst, size_t sz);

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

int arch_prctl(int code, unsigned long addr);

#endif
