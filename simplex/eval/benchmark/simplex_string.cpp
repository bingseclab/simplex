//=== simplex/tools/benchmark/simplex_string.cpp - Memory operations -*- C++ -*-===//
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
/// \file simplex_string.cpp
/// Simplex implementations of selected memory operation functions from string.h
///
/// \brief
/// The functions have been modified so that any buffer passed as an argument is
/// instead passed through either bounds register BND1 or BND2. Function
/// implementations have been modified accordingly.
///
//===----------------------------------------------------------------------===//

#include "simplex_string.h"
#include "simplex.h"

int simplex_memcmp(size_t count) {
  const unsigned char *s1 = (unsigned char *)qgetbndl(BND1);
  const unsigned char *s2 = (unsigned char *)qgetbndl(BND2);

  while (count-- > 0) {
    if (*s1++ != *s2++)
      return s1[-1] < s2[-1] ? -1 : 1;
  }
  return 0;
}

void *simplex_memcpy(size_t len) {

  char *d = (char *)qgetbndl(BND1);
  const char *s = (char *)qgetbndl(BND2);
  while (len--)
    *d++ = *s++;
  return (void *)qgetbndl(BND1);
}

void *simplex_memmove(size_t len) {
  char *d = (char *)qgetbndl(BND1);
  const char *s = (const char *)qgetbndl(BND2);
  if (d < s)
    while (len--)
      *d++ = *s++;
  else {
    char *lasts = (char *)s + (len - 1);
    char *lastd = d + (len - 1);
    while (len--)
      *lastd-- = *lasts--;
  }
  return (void *)qgetbndl(BND1);
}

void *simplex_memset(int val, size_t len) {
  unsigned char *ptr = (unsigned char *)qgetbndl(BND1);
  while (len-- > 0)
    *ptr++ = val;
  return (void *)qgetbndl(BND1);
}

void *simplex_memchr(int c, size_t len) {
  const unsigned char *src = (const unsigned char *)qgetbndl(BND1);

  while (len-- > 0) {
    if (*src == c)
      return (void *)src;
    src++;
  }
  return 0;
}

int libgcc_memcmp(const void *str1, const void *str2, size_t count) {
  const unsigned char *s1 = (const unsigned char *)str1;
  const unsigned char *s2 = (const unsigned char *)str2;

  while (count-- > 0) {
    if (*s1++ != *s2++)
      return s1[-1] < s2[-1] ? -1 : 1;
  }
  return 0;
}

void *libgcc_memcpy(void *dest, const void *src, size_t len) {
  char *d = (char *)dest;
  const char *s = (const char *)src;
  while (len--)
    *d++ = *s++;
  return dest;
}

void *libgcc_memmove(void *dest, const void *src, size_t len) {
  char *d = (char *)dest;
  const char *s = (const char *)src;
  if (d < s)
    while (len--)
      *d++ = *s++;
  else {
    char *lasts = (char *)(s + (len - 1));
    char *lastd = (char *)(d + (len - 1));
    while (len--)
      *lastd-- = *lasts--;
  }
  return dest;
}

void *libgcc_memset(void *dest, int val, size_t len) {
  unsigned char *ptr = (unsigned char *)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

void *libgcc_memchr(const void *src_void, int c, size_t length) {
  const unsigned char *src = (const unsigned char *)src_void;

  while (length-- > 0) {
    if (*src == c)
      return (void *)src;
    src++;
  }
  return 0;
}
