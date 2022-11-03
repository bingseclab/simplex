////////////////////////////////////////////////////////////////////////////////
/// \file mpxrt.h
/// \brief      Replacement MPX runtime extracted from GCC's MPX runtime
/// \author     Matthew
/// \date       2022-11-03T14:43:31-0400
/// \copyright  GNU Lesser General Public License v2.1 or later
/// SPDX-License-Identifier: LGPL-2.1-or-later
////////////////////////////////////////////////////////////////////////////////

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

#include <stdint.h>

#ifndef MPXRT_H
#define MPXRT_H

#ifdef __i386__

/* i386 directory size is 4MB.  */
#define NUM_L1_BITS 20
#define NUM_L2_BITS 10
#define NUM_IGN_BITS 2
#define MPX_L1_ADDR_MASK 0xfffff000UL
#define MPX_L2_ADDR_MASK 0xfffffffcUL
#define MPX_L2_VALID_MASK 0x00000001UL

#define REG_IP_IDX REG_EIP
#define REX_PREFIX

#define XSAVE_OFFSET_IN_FPMEM sizeof(struct _libc_fpstate)

#else /* __i386__ */

/* x86_64 directory size is 2GB.  */
#define NUM_L1_BITS 28
#define NUM_L2_BITS 17
#define NUM_IGN_BITS 3
#define MPX_L1_ADDR_MASK 0xfffffffffffff000ULL
#define MPX_L2_ADDR_MASK 0xfffffffffffffff8ULL
#define MPX_L2_VALID_MASK 0x0000000000000001ULL

#define REG_IP_IDX REG_RIP
#define REX_PREFIX "0x48, "

#define XSAVE_OFFSET_IN_FPMEM 0

#endif /* !__i386__ */

#define MPX_L1_SIZE ((1UL << NUM_L1_BITS) * sizeof(void *))

#define MPX_ENABLE_BIT_NO 0
#define BNDPRESERVE_BIT_NO 1

struct xsave_hdr_struct {
  uint64_t xstate_bv;
  uint64_t reserved1[2];
  uint64_t reserved2[5];
} __attribute__((packed));

struct bndregs_struct {
  uint64_t bndregs[8];
} __attribute__((packed));

struct bndcsr_struct {
  uint64_t cfg_reg_u;
  uint64_t status_reg;
} __attribute__((packed));

struct xsave_struct {
  uint8_t fpu_sse[512];
  struct xsave_hdr_struct xsave_hdr;
  uint8_t ymm[256];
  uint8_t lwp[128];
  struct bndregs_struct bndregs;
  struct bndcsr_struct bndcsr;
} __attribute__((packed));

#endif
