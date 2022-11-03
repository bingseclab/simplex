////////////////////////////////////////////////////////////////////////////////
/// \file simplex.h
/// \brief      This file defines the Simplex programmer interface
/// \author     Matthew
/// \date       2022-11-03T14:46:27-0400
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

#ifdef __cplusplus
extern "C" {
#endif

#include "cpuid.h"
#include "mpxrt.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef SIMPLEX_H
#define SIMPLEX_H

typedef enum bndreg { BND0, BND1, BND2, BND3 } bndreg_t;
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

// Runtime Functions
static bool check_mpx_support(void);
bool process_specific_init(void);
bool process_specific_finish(void);

// Accessor Functions
uint64_t getbndl(bndreg_t reg);
uint64_t getbndu(bndreg_t reg);
uint128_t getbnd(bndreg_t reg);
uint64_t qgetbndl(bndreg_t reg);

// Mutator Functions
void setbndl(bndreg_t reg, uint64_t val);
void setbndu(bndreg_t reg, uint64_t val);
void setbnd(bndreg_t reg, uint128_t val);
void qsetbndl(bndreg_t reg, uint64_t val);

#endif

#ifdef __cplusplus
}
#endif
