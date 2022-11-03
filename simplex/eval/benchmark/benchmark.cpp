//=== simplex/tools/benchmark/benchmark.cpp - benchmark -*- C++ -*-===//
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
/// \file benchmark.cpp
/// Performance benchmarks for Simplex using Hayai framework.
///
//===----------------------------------------------------------------------===//

#include "benchmark.h"
#include "hayai.hpp"
#include "simplex.h"
#include "simplex_string.h"
#include "traverse.h"
#include <ctime>   // time
#include <cstddef> // size_t
#include <cstring> // memcpy
#include <cerrno>  // errno
#include <fstream> // ifstream
#include <iostream>

// Enabling Segment Registers
#include <sys/mman.h> //mmap
#include <asm/prctl.h> //arch_prctl
#include <sys/prctl.h> //arch_prctl, prctl
#include <unistd.h> //getpagesize

void randomize(char *ptr, size_t sz) {
  size_t c;
  if (ptr != NULL) {
    for (c = 0; c < (sz - 1); c++) {
      ptr[c] = (rand() % 127) + 1;
    }
    ptr[c] = '\0';
  }
}

void corrupt(char *dst, char *src, size_t sz){
  if (src != NULL && dst != NULL){ memcpy(dst,src,(size_t) 4 * MB); }
  if (dst != NULL){
    size_t i = (rand() % sz);
    dst[i] = ~(dst[i]);
  }
}

int arch_prctl(int code, unsigned long addr)
{
    return syscall(SYS_arch_prctl, code, addr);
}

///
/// @brief      Microbenchmark testing read/write operations with the bounds registers.
///
class LoadStoreFixture : public ::hayai::Fixture {
public:
  void* clave;
  uint64_t val;
  size_t size = sizeof(val);
  std::ifstream urandom;

  virtual void SetUp() {
    // Open /dev/urandom, read a random uint64_t
    urandom.open("/dev/urandom", std::ios::in|std::ios::binary);
    if(urandom) {
      urandom.read(reinterpret_cast<char*>(&val), size);
    } else {
      std::cerr << "Failed to open urandom" << std::endl;
      val = 0xdeadbeef;
    }

    // Enable MPX 
    process_specific_init();
    
    // Enable segmentation registers
    clave = mmap(NULL, getpagesize(),
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    arch_prctl(ARCH_SET_GS, (unsigned long)(clave));
  }

  virtual void TearDown() { 
    // Close /dev/urandom
    urandom.close();

    // Disable MPX
    process_specific_finish(); 

    // Disable segmentation registers
    munmap(clave, getpagesize());
  }
};

BENCHMARK_F(LoadStoreFixture,GPStore,10000,ITERS) {
  __asm__ volatile("movq %0, %%r15;"
                   : /*OutputOperands*/
                   : "q"(val)
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,SEGStore,10000,ITERS) {
  __asm__ volatile("mov %0, %%gs:0;"
                   : /*OutputOperands*/
                   : "q"(val)
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,MMXStore,10000,ITERS) {
  __asm__ volatile("movq %0, %%mm0;"
                   : /*OutputOperands*/
                   : "q"(val)
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,XMMStore,10000,ITERS){
  __asm__ volatile("mov %0, -0x80(%%rsp);"
                   "movaps -0x80(%%rsp), %%xmm1;"
                   : /* OutputOperands*/
                   : "q" (val)
                   : /* Clobbers */);
}

BENCHMARK_F(LoadStoreFixture,BNDStore,10000,ITERS) {
  __asm__ volatile("bndmk (%0), %%bnd0;"
                   : /*OutputOperands*/
                   : "q"(val)
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,GPLoad,10000,ITERS) {
  __asm__ volatile("mov %%r15, %0;"
                   : "=q"(val)
                   : /*InputOperands*/
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,SEGLoad,10000,ITERS) {
  __asm__ volatile("mov %%gs, %0;"
                   : "=q"(val)
                   : /*InputOperands*/
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,MMXLoad,10000,ITERS) {
  __asm__ volatile("movq %%mm0, %0;"
                   : "=q"(val)
                   : /*InputOperands*/
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,XMMLoad,10000,ITERS) {
  __asm__ volatile("movaps %%xmm1,-0x80(%%rsp);"
                   "mov -0x80(%%rsp), %0;"
                   : "=q" (val)
                   : /* InputOperands*/
                   : /*Clobbers*/);
}

BENCHMARK_F(LoadStoreFixture,BNDLoad,10000,ITERS) {
  __asm__ volatile("bndmov %%bnd0, -0x80(%%rsp);"
                   "mov -0x80(%%rsp), %0;"
                   : "=q"(val)
                   : /*InputOperands*/
                   : /*Clobbers*/);
}

/// @class TraversalFixture
/// @brief Benchmark testing traversing and copying a buffer. 
/// This fixture uses very unfavorable circumstances (frequent reads, increased 
/// cache pressure) to show a worst-case but amortized performance overhead.
class TraversalFixture : public ::hayai::Fixture {
public:
  virtual void SetUp() {
    // set up the traversal buffer
    size_t c;
    srand(time(NULL));
    buffer = (char *)malloc(4 * MB);
    randomize(buffer,4 * MB);

    // initialize simplex
    process_specific_init();
  }
  virtual void TearDown() {
    // tear down the traversal buffer
    free(buffer);

    // finalize simplex
    process_specific_finish();
  }

  char *buffer;
};

BENCHMARK_P_F(TraversalFixture, GPTraverse,RUNS,ITERS/100,(std::size_t sz)) {
  gptraverse(&this->buffer, sz);
}
BENCHMARK_P_INSTANCE(TraversalFixture, GPTraverse, (4 * KB));
BENCHMARK_P_INSTANCE(TraversalFixture, GPTraverse, (2 * MB));

BENCHMARK_P_F(TraversalFixture, MPXTraverse,RUNS,ITERS/100,(std::size_t sz)) {
  mpxtraverse(&this->buffer, sz);
}
BENCHMARK_P_INSTANCE(TraversalFixture, MPXTraverse, (4 * KB));
BENCHMARK_P_INSTANCE(TraversalFixture, MPXTraverse, (2 * MB));

///
/// @brief      Benchmark testing Simplex implementations of string.h functions.
///
class StringFixture : public ::hayai::Fixture {
public:
  char *s1;
  char *s2;

  virtual void SetUp() {
    srand(time(NULL));
    
    // Initialize two buffers. The contents should be the same except for one
    // byte (for memcmp) and both should be null terminated (for memchr).
    s1 = (char *)malloc(4 * MB);
    s2 = (char *)malloc(4 * MB);
    randomize(s1,4 * MB); 
    corrupt(s2,s1,4 * MB);

    // initialize simplex
    process_specific_init();
    qsetbndl(BND1, (uint64_t)s1);
    qsetbndl(BND2, (uint64_t)s2);
  }

  virtual void TearDown() {
    free(s1);
    free(s2);

    // finalize simplex
    process_specific_finish();
  }
};

BENCHMARK_P_F(StringFixture, libgcc_memcmp,RUNS,ITERS/1000,(std::size_t sz)) {
  libgcc_memcmp(s1, s2, sz);
}
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memcmp, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memcmp, (2 * MB));

BENCHMARK_P_F(StringFixture, simplex_memcmp,RUNS,ITERS/1000,(std::size_t sz)) {
  simplex_memcmp(sz);
}  
BENCHMARK_P_INSTANCE(StringFixture, simplex_memcmp, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, simplex_memcmp, (2 * MB));

BENCHMARK_P_F(StringFixture, libgcc_memcpy,RUNS,ITERS/100,(std::size_t sz)){
  libgcc_memcpy(s1,s2,sz);
}
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memcpy, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memcpy, (2 * MB));

BENCHMARK_P_F(StringFixture, simplex_memcpy,RUNS,ITERS/100,(std::size_t sz)){
  simplex_memcpy(sz);
}
BENCHMARK_P_INSTANCE(StringFixture, simplex_memcpy, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, simplex_memcpy, (2 * MB));

BENCHMARK_P_F(StringFixture, libgcc_memmove,RUNS,ITERS/100,(std::size_t sz)){
  libgcc_memmove(s1,s1+(sz/4)+1,sz/2);
}
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memmove, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memmove, (2 * MB));

BENCHMARK_P_F(StringFixture, simplex_memmove,RUNS,ITERS/100,(std::size_t sz)){
  qsetbndl(BND1,(uint64_t) s1);
  qsetbndl(BND2,(uint64_t) s1+(sz/4)+1);
  simplex_memmove(sz/2);
}
BENCHMARK_P_INSTANCE(StringFixture, simplex_memmove, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, simplex_memmove, (2 * MB));

BENCHMARK_P_F(StringFixture, libgcc_memset,RUNS,ITERS/100,(std::size_t sz)){
  libgcc_memset(s1, 0x43, sz);
}
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memset, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memset, (2 * MB));

BENCHMARK_P_F(StringFixture, simplex_memset,RUNS,ITERS/100,(std::size_t sz)){
  simplex_memset(0x43, sz);
}
BENCHMARK_P_INSTANCE(StringFixture, simplex_memset, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, simplex_memset, (2 * MB));

BENCHMARK_P_F(StringFixture, libgcc_memchr,RUNS,ITERS/100,(std::size_t sz)){
  libgcc_memchr(s1, 0x0, sz);
}
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memchr, (4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, libgcc_memchr, (2 * MB));

BENCHMARK_P_F(StringFixture, simplex_memchr,RUNS,ITERS/100,(std::size_t sz)){
  simplex_memchr(0x0, sz);
}
BENCHMARK_P_INSTANCE(StringFixture, simplex_memchr,(4 * KB));
BENCHMARK_P_INSTANCE(StringFixture, simplex_memchr,(2 * MB));
