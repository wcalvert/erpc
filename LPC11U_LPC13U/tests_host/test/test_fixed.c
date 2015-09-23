/**************************************************************************/
/*!
    @file     test_fixed.c
    @ingroup  Unit Tests

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, K. Townsend (microBuilder.eu)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <string.h>
#include "unity.h"
#include "fixed.h"

/* Test for the fixed-point macros in src/fixed.h  */
/* These tests assume 'fixed_FRAC' = 16 (Q16.16)   */
/* min = -32768, max =  32768, step =  1.52588e-05 */

fixed_t a, b, result;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_fixed_add(void)
{
  /* Negative value */
  a = fixed_make(-2023.621F);
  b = fixed_make(1.0F);
  result = fixed_add(a, b);
  TEST_ASSERT_EQUAL_FLOAT(fixed_make(-2022.621F), result);

  /* Negation */
  a = fixed_make(-2023.621F);
  b = fixed_make(2023.621F);
  result = fixed_add(a, b);
  TEST_ASSERT_EQUAL_FLOAT(0.0F, fixed_float(result));
}

void test_fixed_subtract(void)
{
  /* Negative value */
  a = fixed_make(-2023.621F);
  b = fixed_make(1.0F);
  result = fixed_sub(a, b);
  TEST_ASSERT_EQUAL_FLOAT(fixed_make(-2024.621F), result);

  /* Negation */
  a = fixed_make(2023.621F);
  b = fixed_make(2023.621F);
  result = fixed_sub(a, b);
  TEST_ASSERT_EQUAL_FLOAT(0.0F, fixed_float(result));
}

void test_fixed_multiply(void)
{
  /* Negative */
  a = fixed_make(-2023.621F);
  b = fixed_make(10.0F);
  result = fixed_mul(a, b);
  TEST_ASSERT_EQUAL_FLOAT(fixed_make(-20236.21F), result);

  /* Positive */
  a = fixed_make(2023.621F);
  b = fixed_make(10.0F);
  result = fixed_mul(a, b);
  TEST_ASSERT_EQUAL_FLOAT(fixed_make(20236.21F), result);
}

void test_fixed_divide(void)
{
  /* Negative */
  a = fixed_make(-2023.621F);
  b = fixed_make(10.0F);
  result = fixed_div(a, b);
  TEST_ASSERT_EQUAL_FLOAT(fixed_make(-202.3621F), result);

  /* Positive */
  a = fixed_make(2023.621F);
  b = fixed_make(10.0F);
  result = fixed_div(a, b);
  TEST_ASSERT_EQUAL_FLOAT(fixed_make(202.3621F), result);
}
