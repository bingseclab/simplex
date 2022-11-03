//=== simplex/tools/benchmark/simplex_string.h - memory operations -*- C++ -*-===//
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
/// \file simplex_string.h
/// Simplex implementations of selected memory operation functions from string.h
///
/// \brief
/// The functions have been modified so that any buffer passed as an argument is
/// instead passed through either bounds register BND1 or BND2. Function
/// implementations have been modified accordingly.
///
//===----------------------------------------------------------------------===//

#ifndef SIMPLEX_STRING_H
#define SIMPLEX_STRING_H

#include <cstddef>
#include <cstdio>  // NULL
#include <cstdlib> // malloc/free, srand/rand
#include <cstring> /// mem(.*) functions

int simplex_memcmp(size_t count);
void *simplex_memcpy(size_t len);
void *simplex_memmove(size_t len);
void *simplex_memset(int val, size_t len);
void *simplex_memchr(int c, size_t len);

int libgcc_memcmp(const void *str1, const void *str2, size_t count);
void *libgcc_memcpy(void *dest, const void *src, size_t len);
void *libgcc_memmove(void *dest, const void *src, size_t len);
void *libgcc_memset(void *dest, int val, size_t len);
void *libgcc_memchr(const void *src_void, int c, size_t length);

#endif