////////////////////////////////////////////////////////////////////////////////
/// \file mpxrt.c
/// \brief      This file implements mpxrt.
/// \author     Matthew
/// \date       2022-11-03T14:48:53-0400
/// \copyright  GNU Lesser General Public License v2.1 or later
/// SPDX-License-Identifier: LGPL-2.1-or-later
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2022  Matthew Cole and Aravind Prakash

// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.

// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

//////////////////////////////////////////////////////////////////////////////  
// This file contains code originating from the Intel corporation. See
// GCC source code at gcc/libmpx/mpxrt/mpxrt.c
//////////////////////////////////////////////////////////////////////////////
//  \copyright Copyright (C) 2014, Intel Corporation
//  All rights reserved.

//  \copyright
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:

//  * Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in
//  the documentation and/or other materials provided with the
//  distribution.
//  * Neither the name of Intel Corporation nor the names of its
//  contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.

//  \copyright
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
//  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
//  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include "cpuid.h"
#include "simplex.h"
#include "mpxrt.h"
#include <assert.h>
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STDC_FORMAT_MACROS

// Following vars are initialized at process startup only
//   and thus are considered to be thread safe.
static void *l1base = NULL;
static int bndpreserve = 1;
static int enable = 1;

static inline void xrstor_state(struct xsave_struct *fx, uint64_t mask) {
  uint32_t lmask = mask;
  uint32_t hmask = mask >> 32;

  __asm__ volatile(".byte " REX_PREFIX "0x0f,0xae,0x2f\n\t"
               :
               : "D"(fx), "m"(*fx), "a"(lmask), "d"(hmask)
               : "memory");
}

static inline void xsave_state(struct xsave_struct *fx, uint64_t mask) {
  uint32_t lmask = mask;
  uint32_t hmask = mask >> 32;

  __asm__ volatile(".byte " REX_PREFIX "0x0f,0xae,0x27\n\t"
               :
               : "D"(fx), "m"(*fx), "a"(lmask), "d"(hmask)
               : "memory");
}

static inline uint64_t xgetbv(uint32_t index) {
  uint32_t eax, edx;

  __asm__ volatile(".byte 0x0f,0x01,0xd0" /* xgetbv */
               : "=a"(eax), "=d"(edx)
               : "c"(index));
  return eax + ((uint64_t)edx << 32);
}

///
/// \brief      { Check whether a CPU can support Simplex operations. 
/// 
///               This function does not modify CPU context. }
///
/// \return     { Return true if the CPU supports Simplex, false otherwise. }
///
static bool check_mpx_support(void) {
  unsigned int eax, ebx, ecx, edx;
  unsigned int max_level = __get_cpuid_max(0, NULL);

  if (max_level < 13) {
    // fprintf (stderr, "No required CPUID level support.\n");
    return false;
  }

  __cpuid_count(0, 0, eax, ebx, ecx, edx);
  if (!(ecx & bit_XSAVE)) {
    // fprintf (stderr, "No XSAVE support.\n");
    return false;
  }

  if (!(ecx & bit_OSXSAVE)) {
    // fprintf (stderr, "No OSXSAVE support.\n");
    return false;
  }

  __cpuid_count(7, 0, eax, ebx, ecx, edx);
  if (!(ebx & bit_MPX)) {
    // fprintf (stderr, "No MPX support.\n");//#include "config.h"
    return false;
  }

  __cpuid_count(13, 0, eax, ebx, ecx, edx);
  if (!(eax & bit_BNDREGS)) {
    // fprintf (stderr, "No BNDREGS support.\n");
    return false;
  }

  if (!(eax & bit_BNDCSR)) {
    // fprintf (stderr, "No BNDCSR support.\n");
    return false;
  }

  // fprintf (stderr, "MPX supported!\n");
  return true;
}

static void enable_mpx(void) {
  uint8_t __attribute__((__aligned__(64))) buffer[4096];
  struct xsave_struct *xsave_buf = (struct xsave_struct *)buffer;
  memset(buffer, 0, sizeof(buffer)); //#include "config.h"
  xrstor_state(xsave_buf, 0x18);

  //fprintf(stderr, "Initalizing MPX...\n");
  //fprintf(stderr, "  Enable bit: %d\n", enable);
  //fprintf(stderr, "  BNDPRESERVE bit: %d\n", bndpreserve);

  /* Enable MPX.  */
  xsave_buf->xsave_hdr.xstate_bv = 0x10;
  xsave_buf->bndcsr.cfg_reg_u = (unsigned long)l1base;
  xsave_buf->bndcsr.cfg_reg_u |= enable << MPX_ENABLE_BIT_NO;
  xsave_buf->bndcsr.cfg_reg_u |= bndpreserve << BNDPRESERVE_BIT_NO;
  xsave_buf->bndcsr.status_reg = 0;

  xrstor_state(xsave_buf, 0x10);
}

static void disable_mpx(void) {
  uint8_t __attribute__((__aligned__(64))) buffer[4096];
  struct xsave_struct *xsave_buf = (struct xsave_struct *)buffer;
  memset(buffer, 0, sizeof(buffer));
  xrstor_state(xsave_buf, 0x18);

  //fprintf(stderr, "Disabling MPX...\n");

  /* Disable MPX.  */
  xsave_buf->xsave_hdr.xstate_bv = 0x10;
  xsave_buf->bndcsr.cfg_reg_u = 0;
  xsave_buf->bndcsr.status_reg = 0;

  xrstor_state(xsave_buf, 0x10);
}

///
///\brief      { Enable MPX context usage for a single process. }
///
///\return     { Return true if the process is ready for Simplex library usage,
///            return false otherwise.
///
///If the CPU does not support Simplex, this function returns false. }
///
bool process_specific_init(void) {
  if (!check_mpx_support())
    return false;
  enable_mpx();
  return true;
}

///
/// \brief      { Disable MPX context usage for a single process. }
///
/// \return     { Return true if Simplex usage is now disabled, 
///               return false otherwise. 
///               
///               If the CPU does not support Simplex, 
///               this function returns false even though Simplex is not (and 
///               was not ever) supported. }
///
bool process_specific_finish(void) {
  if (!check_mpx_support())
    return false;
  disable_mpx();
  return true;
}
