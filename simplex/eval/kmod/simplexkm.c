//=== simplex/tools/kmod/simplexkm.c - kernel module -*- C -*-===//
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
/// \file simplexkm.c
/// Loadable kernel module which globally enables MPX instruction execution
///
//===----------------------------------------------------------------------===//



#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

#define __STDC_FORMAT_MACROS

#include "simplexkm.h"

#define MPX_ENABLE_BIT_NO 0
#define BNDPRESERVE_BIT_NO 1

struct xsave_hdr_struct
{
  uint64_t xstate_bv;
  uint64_t reserved1[2];
  uint64_t reserved2[5];
} __attribute__ ((packed));

struct bndregs_struct
{
  uint64_t bndregs[8];
} __attribute__ ((packed));

struct bndcsr_struct {
	uint64_t cfg_reg_u;
	uint64_t status_reg;
} __attribute__((packed));

struct xsave_struct
{
  uint8_t fpu_sse[512];
  struct xsave_hdr_struct xsave_hdr;
  uint8_t ymm[256];
  uint8_t lwp[128];
  struct bndregs_struct bndregs;
  struct bndcsr_struct bndcsr;
} __attribute__ ((packed));

/* Following vars are initialized at process startup only
   and thus are considered to be thread safe.  */
static void *l1base = NULL;
static int bndpreserve = 1;
static int enable = 1;

/* Var holding number of occured BRs.  It is modified from
   signal handler only and thus it should be thread safe.  */
//static uint64_t num_bnd_chk = 0;

static inline void xrstor_state (struct xsave_struct *fx, uint64_t mask)
{
  uint32_t lmask = mask;
  uint32_t hmask = mask >> 32;

  asm volatile (".byte " REX_PREFIX "0x0f,0xae,0x2f\n\t"
		: : "D" (fx), "m" (*fx), "a" (lmask), "d" (hmask)
		:   "memory");
}

static inline void xsave_state (struct xsave_struct *fx, uint64_t mask)
{
  uint32_t lmask = mask;
  uint32_t hmask = mask >> 32;

  asm volatile (".byte " REX_PREFIX "0x0f,0xae,0x27\n\t"
		: : "D" (fx), "m" (*fx), "a" (lmask), "d" (hmask)
		:   "memory");
}

static inline uint64_t xgetbv (uint32_t index)
{
  uint32_t eax, edx;

  asm volatile (".byte 0x0f,0x01,0xd0" /* xgetbv */
		: "=a" (eax), "=d" (edx)
		: "c" (index));
  return eax + ((uint64_t)edx << 32);
}

static bool check_no_mpx_support (void)
{
  unsigned int eax, ebx, ecx, edx;
  unsigned int max_level = __get_cpuid_max (0, NULL);

  printk(KERN_INFO "Checking MPX support.\n");

  if (max_level < 13)
    {
      printk(KERN_ERR "No required CPUID level support.\n");
      return 1;
    }

  __cpuid_count (0, 0, eax, ebx, ecx, edx);
  if (!(ecx & bit_XSAVE))
    {
      printk(KERN_ERR "No XSAVE support.\n");
      return 1;
    }

  if (!(ecx & bit_OSXSAVE))
    {
      printk(KERN_ERR "No OSXSAVE support.\n");
      return 1;
    }

  __cpuid_count (7, 0, eax, ebx, ecx, edx);
  if (!(ebx & bit_MPX))
    {
      printk(KERN_ERR "No MPX support.\n");//#include "config.h"
      return 1;
    }

  __cpuid_count (13, 0, eax, ebx, ecx, edx);
  if (!(eax & bit_BNDREGS))
    {
      printk(KERN_ERR "No BNDREGS support.\n");
      return 1;
    }

  if (!(eax & bit_BNDCSR))
    {
      printk(KERN_ERR "No BNDCSR support.\n");
      return 1;
    }

  printk(KERN_INFO "MPX supported!\n");
  return 0;
}

static void enable_mpx (void)
{
  uint8_t __attribute__ ((__aligned__ (64))) buffer[4096];
  struct xsave_struct *xsave_buf = (struct xsave_struct *)buffer;
  memset (buffer, 0, sizeof (buffer));//#include "config.h"
  xrstor_state (xsave_buf, 0x18);

  printk(KERN_INFO "Initalizing MPX...\n");
  printk(KERN_INFO "  Enable bit: %d\n", enable);
  printk(KERN_INFO "  BNDPRESERVE bit: %d\n", bndpreserve);

  /* Enable MPX.  */
  xsave_buf->xsave_hdr.xstate_bv = 0x10;
  xsave_buf->bndcsr.cfg_reg_u = (unsigned long)l1base;
  xsave_buf->bndcsr.cfg_reg_u |= enable << MPX_ENABLE_BIT_NO;
  xsave_buf->bndcsr.cfg_reg_u |= bndpreserve << BNDPRESERVE_BIT_NO;
  xsave_buf->bndcsr.status_reg = 0;

  xrstor_state (xsave_buf, 0x10);
}

static void disable_mpx (void)
{
  uint8_t __attribute__ ((__aligned__ (64))) buffer[4096];
  struct xsave_struct *xsave_buf = (struct xsave_struct *)buffer;
  memset(buffer, 0, sizeof(buffer));
  xrstor_state(xsave_buf, 0x18);

  printk(KERN_INFO "Disabling MPX...\n");

  /* Disable MPX.  */
  xsave_buf->xsave_hdr.xstate_bv = 0x10;
  xsave_buf->bndcsr.cfg_reg_u = 0;
  xsave_buf->bndcsr.status_reg = 0;

  xrstor_state(xsave_buf, 0x10);
}

static int __init enable_init (void)
{
  if (check_no_mpx_support ())
    return 1;

  enable_mpx ();

  return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit enable_cleanup (void)
{
  if (check_no_mpx_support ())
    return ;

  disable_mpx();

  return ;
}

module_init(enable_init);
module_exit(enable_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthew Cole");
MODULE_DESCRIPTION("Enable MPX instructions without a runtime.");
