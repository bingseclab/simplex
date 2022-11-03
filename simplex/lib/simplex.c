////////////////////////////////////////////////////////////////////////////////
/// \file simplex.c
/// \brief      This file implements the simplex programmer interface.
/// \author     Matthew
/// \date       2022-11-03T12:29:53-0400
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

#include "simplex.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOWER (0xFFFFFFFFFFFFFFFFULL)
#define UPPER (0xFFFFFFFFFFFFFFFFULL << 64)

///
/// \section Accessors
///

///
/// \brief      { Get contents of lower half of a bounds register }
///
/// \param[in]  reg   The bounds register
///
/// \return     { The contents of the lower 64 bits of the bounds register }
/// 
uint64_t getbndl(bndreg_t reg) {
  uint64_t val;

  // Move bound register contents to stack
  // Move contents from stack to lower
  switch (reg) {
  case BND0:
    __asm__("bndmov %%bnd0, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND1:
    __asm__("bndmov %%bnd1, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND2:
    __asm__("bndmov %%bnd2, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND3:
    __asm__("bndmov %%bnd3, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(getbndl) invalid bounds register: %i\n", reg);
    return 0;
  }

  // Sanitize the 16 bytes below the stack pointer
  __asm__ volatile("andq $0, -0x78(%rsp);"
                   "andq $0, -0x80(%rsp);");

  return val;
}

///
/// \brief      { Get contents of the upper half of a bounds register}
///
/// \param[in]  reg   The bounds register
///
/// \return     { The contents of the upper 64 bits of the bounds register }
/// 
uint64_t getbndu(bndreg_t reg) {
  uint64_t val;

  // Move bound register contents to stack
  switch (reg) {
  case BND0:
    __asm__("bndmov %%bnd0, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND1:
    __asm__("bndmov %%bnd1, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND2:
    __asm__("bndmov %%bnd2, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND3:
    __asm__("bndmov %%bnd3, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(getbndu) invalid bounds register: %i\n", reg);
    return 0;
  }

  // Sanitize the 16 bytes below the stack pointer
  __asm__ volatile("andq $0, -0x78(%rsp);"
                   "andq $0, -0x80(%rsp);");

  // The value is stored in the bounds register as its one's complement.
  // Bitwise inversion recovers the original value stored.
  return ~val;
}

///
/// \brief      { Get the contents of a bounds register as a single 
///               value. The upper half is converted from its stored 
///               one's compliment value. }
///
/// \param[in]  reg   The bounds register
///
/// \return     { The contents of the entire 128 bits of the bounds register }
/// 
uint128_t getbnd(bndreg_t reg) {
  uint128_t val;
  uint64_t lower;
  uint64_t upper;

  // Move bound register contents to stack
  switch (reg) {
  case BND0:
    __asm__("bndmov %%bnd0, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0;"
            "mov -0x80(%%rsp), %1"
            : "=q"(upper), "=q"(lower)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND1:
    __asm__("bndmov %%bnd1, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0;"
            "mov -0x80(%%rsp), %1"
            : "=q"(upper), "=q"(lower)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND2:
    __asm__("bndmov %%bnd2, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0;"
            "mov -0x80(%%rsp), %1"
            : "=q"(upper), "=q"(lower)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND3:
    __asm__("bndmov %%bnd3, -0x80(%%rsp);"
            "mov -0x78(%%rsp), %0;"
            "mov -0x80(%%rsp), %1"
            : "=q"(upper), "=q"(lower)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(getbndu) invalid bounds register: %i\n", reg);
    return 0;
  }

  // Sanitize the 16 bytes below the stack pointer
  __asm__ volatile("andq $0, -0x78(%rsp);"
                   "andq $0, -0x80(%rsp);");

  // Compute return value
  val = ((uint128_t)lower) | ((~(uint128_t)upper) << 64);
  return val;
}

///
/// \brief      { Get the contents of the lower 64 bits of a bounds register 
///               without attempting to sanitize the spilled values from the 
///               stack }
///
/// \param[in]  reg   The bounds register
///
/// \return     { The contents of the lower 64 bits of the bounds register }
/// 
uint64_t qgetbndl(bndreg_t reg) {
  uint64_t val;

  // Move bound register contents to stack
  // Move contents from stack to lower
  switch (reg) {
  case BND0:
    __asm__("bndmov %%bnd0, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND1:
    __asm__("bndmov %%bnd1, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND2:
    __asm__("bndmov %%bnd2, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  case BND3:
    __asm__("bndmov %%bnd3, -0x80(%%rsp);"
            "mov -0x80(%%rsp), %0;"
            : "=q"(val)
            : /*InputOperands*/
            : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(getbndl) invalid bounds register: %i\n", reg);
    return 0;
  }

  return val;
}

///
/// \section Mutators
/// 

///
///\brief      { Set the contents of the lower 64 bits of a bounds register, 
///              while preserving the upper 64 bits }
///
///\param[in]  reg   The bounds register
///\param[in]  val   The value to be set
///
void setbndl(bndreg_t reg, uint64_t val) {
  // Capture original value of bndn.upper,
  // then adjust it for sib-address expression
  uint64_t upper = getbndu(reg);
  upper -= val;

  // Set bndn by bndmk
  switch (reg) {
  case BND0:
    __asm__ volatile("bndmk (%0,%1), %%bnd0"
                     : /*OutputOperands*/
                     : "q"(val), "q"(upper)
                     : /*Clobbers*/);
    break;
  case BND1:
    __asm__ volatile("bndmk (%0,%1), %%bnd1"
                     : /*OutputOperands*/
                     : "q"(val), "q"(upper)
                     : /*Clobbers*/);
    break;
  case BND2:
    __asm__ volatile("bndmk (%0,%1), %%bnd2"
                     : /*OutputOperands*/
                     : "q"(val), "q"(upper)
                     : /*Clobbers*/);
    break;
  case BND3:
    __asm__ volatile("bndmk (%0,%1), %%bnd3"
                     : /*OutputOperands*/
                     : "q"(val), "q"(upper)
                     : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(setbndl) invalid bounds register: %i\n", reg);
  }
}

///
///\brief      { Set the contents of the upper 64 bits of a bounds register, 
///              while preserving the lower 64 bits. The value is stored as its 
///              one's complement }
///
///\param[in]  reg   The bounds register
///\param[in]  val   The value to be set
///
void setbndu(bndreg_t reg, uint64_t val) {
  // Capture original value of bndn.lower,
  // then adjust for sib-addressing
  uint64_t lower = getbndl(reg);
  val -= lower;

  // Set bndn by bndmk
  switch (reg) {
  case BND0:
    __asm__ volatile("bndmk (%0,%1), %%bnd0"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(val)
                     : /*Clobbers*/);
    break;
  case BND1:
    __asm__ volatile("bndmk (%0,%1), %%bnd1"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(val)
                     : /*Clobbers*/);
    break;
  case BND2:
    __asm__ volatile("bndmk (%0,%1), %%bnd2"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(val)
                     : /*Clobbers*/);
    break;
  case BND3:
    __asm__ volatile("bndmk (%0,%1), %%bnd3"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(val)
                     : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(setbndu) invalid bounds register: %i\n", reg);
  }
}

///
///\brief      { Set the contents of a bounds register as a single value. The
///            upper half value is stored within the register as its one's
///            complement. }
///
///\param[in]  reg   The register
///\param[in]  val   The value
///
void setbnd(bndreg_t reg, uint128_t val) {
  // calculate displacement for bndmk
  uint64_t lower = (uint64_t)(val & LOWER);
  uint64_t upper = (uint64_t)((val ^ LOWER) >> 64);
  upper -= lower; 

  // Set bndn by bndmk
  switch (reg) {
  case BND0:
    __asm__ volatile("bndmk (%0,%1), %%bnd0"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(upper)
                     : /*Clobbers*/);
    break;
  case BND1:
    __asm__ volatile("bndmk (%0,%1), %%bnd1"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(upper)
                     : /*Clobbers*/);
    break;
  case BND2:
    __asm__ volatile("bndmk (%0,%1), %%bnd2"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(upper)
                     : /*Clobbers*/);
    break;
  case BND3:
    __asm__ volatile("bndmk (%0,%1), %%bnd3"
                     : /*OutputOperands*/
                     : "q"(lower), "q"(upper)
                     : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(setbndu) invalid bounds register: %i\n", reg);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief      { Set the contents of the lower 64 bits of a bounds register, 
///               without preserving the upper 64 bits of the register. }
///
/// \param[in]  reg   The register
/// \param[in]  val   The value
////////////////////////////////////////////////////////////////////////////////
void qsetbndl(bndreg_t reg, uint64_t val) {
  // Set bndn by bndmk
  switch (reg) {
  case BND0:
    __asm__ volatile("bndmk (%0), %%bnd0"
                     : /*OutputOperands*/
                     : "q"(val)
                     : /*Clobbers*/);
    break;
  case BND1:
    __asm__ volatile("bndmk (%0), %%bnd1"
                     : /*OutputOperands*/
                     : "q"(val)
                     : /*Clobbers*/);
    break;
  case BND2:
    __asm__ volatile("bndmk (%0), %%bnd2"
                     : /*OutputOperands*/
                     : "q"(val)
                     : /*Clobbers*/);
    break;
  case BND3:
    __asm__ volatile("bndmk (%0), %%bnd3"
                     : /*OutputOperands*/
                     : "q"(val)
                     : /*Clobbers*/);
    break;
  default:
    fprintf(stderr, "(qsetbndl) invalid bounds register: %i\n", reg);
  }
}
