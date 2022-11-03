////////////////////////////////////////////////////////////////////////////////
/// \file simplex_test.c
/// \brief      Implements the simplex unit tests using the CUnity library.
/// \author     Matthew
/// \date       2022-11-03T15:25:46-0400
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

#include "CUnit/CUnitCI.h"
#include "simplex.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/// Reset all values in bounds registers
void reset_bnd_regs(void) {
  for (bndreg_t reg = 0; reg < 4; reg++) {
    setbndl(reg, 0);
    setbndu(reg, 0);
  }
}

CU_SUITE_SETUP() { 
  CU_ASSERT_FATAL(process_specific_init());
  return CUE_SUCCESS; 
}

CU_SUITE_TEARDOWN() { 
  CU_ASSERT_FATAL(process_specific_finish());
  return CUE_SUCCESS;
}

CU_TEST_SETUP() { 
  reset_bnd_regs(); 
}

/// Test library functions on lower half registers
static void test_lower_half(void) {
  uint64_t res, val;

  for (bndreg_t reg = 0; reg < 4; reg++) {
    res = 0;
    val = 0xDEADBEEFDEADBEEF;

    setbndl(reg, val);
    res = getbndl(reg);
    CU_ASSERT_EQUAL(res, val);
  }
}

/// Test library functions on upper half registers
static void test_upper_half(void) {
  uint64_t res, val;

  for (bndreg_t reg = 0; reg < 4; reg++) {
    res = 0;
    val = 0xDEADBEEFDEADBEEF;

    setbndu(reg, val);
    res = getbndu(reg);
    CU_ASSERT_EQUAL(res, val);
  }
}

/// Test setting lower before upper, with value in lower < upper
static void test_lower_lt_upper(void) {
  uint64_t res_l, val_l, res_u, val_u;
  
  for (bndreg_t reg = 0; reg < 4; reg++) {
    val_l = 0xBEEFUL;
    val_u = 0xDEADUL;

    setbndl(reg, val_l);
    setbndu(reg, val_u);
    res_l = getbndl(reg);
    res_u = getbndu(reg);

    CU_ASSERT_EQUAL(res_l, val_l);
    CU_ASSERT_EQUAL(res_u, val_u);
  }
}

/// Test setting lower before upper, with value in lower > upper
static void test_lower_gt_upper(void) {
  uint64_t res_l, val_l, res_u, val_u;
  
  for (bndreg_t reg = 0; reg < 4; reg++) {
    val_l = 0xDEADUL;
    val_u = 0xBEEFUL;

    setbndl(reg, val_l);
    setbndu(reg, val_u);
    res_l = getbndl(reg);
    res_u = getbndu(reg);

    CU_ASSERT_EQUAL(res_l, val_l);
    CU_ASSERT_EQUAL(res_u, val_u);
  }
}

/// Test setting upper before lower, with value in upper < lower
static void test_upper_lt_lower(void) {
  uint64_t res_l, val_l, res_u, val_u;

  for (bndreg_t reg = 0; reg < 4; reg++) {
    val_l = 0xDEADUL;
    val_u = 0xBEEFUL;

    setbndu(reg, val_u);
    setbndl(reg, val_l);
    res_l = getbndl(reg);
    res_u = getbndu(reg);

    CU_ASSERT_EQUAL(res_l, val_l);
    CU_ASSERT_EQUAL(res_u, val_u);
  }
}

/// Test setting upper before lower, with value in upper > lower
static void test_upper_gt_lower(void) {
  uint64_t res_l, val_l, res_u, val_u;
  
  for (bndreg_t reg = 0; reg < 4; reg++) {
    val_l = 0xBEEFUL;
    val_u = 0xDEADUL;

    setbndu(reg, val_u);
    setbndl(reg, val_l);
    res_l = getbndl(reg);
    res_u = getbndu(reg);

    CU_ASSERT_EQUAL(res_l, val_l);
    CU_ASSERT_EQUAL(res_u, val_u);
  }
}

/// Test library functions on wide (128-bit) registers
static void test_wide(void) {
  uint128_t res, val;

  for (bndreg_t reg = 0; reg < 4; reg++) {
    res = 0;
    val = 0xDEADBEEFDEADBEEF << 16;

    setbnd(reg, val);
    res = getbnd(reg);
    CU_ASSERT_EQUAL(res, val);
  }
}

/// Test quick library functions on lower half registers
static void test_quick(void) {
  uint64_t res, val;

  for (bndreg_t reg = 0; reg < 4; reg++) {
    res = 0;
    val = 0xDEADBEEFDEADBEEF;

    qsetbndl(reg, val);
    res = qgetbndl(reg);
    CU_ASSERT_EQUAL(res, val);
  }
}

CUNIT_CI_RUN("simplex-suite", 
  CUNIT_CI_TEST(test_lower_half),
  CUNIT_CI_TEST(test_upper_half),
  CUNIT_CI_TEST(test_lower_lt_upper),
  CUNIT_CI_TEST(test_lower_gt_upper),
  CUNIT_CI_TEST(test_upper_lt_lower),
  CUNIT_CI_TEST(test_upper_gt_lower), 
  CUNIT_CI_TEST(test_wide),
  CUNIT_CI_TEST(test_quick)
  );