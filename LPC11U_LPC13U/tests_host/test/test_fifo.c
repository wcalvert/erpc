/**************************************************************************/
/*!
    @file     test_fifo.c
    @author   K. Townsend (microBuilder.eu)

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
#include "fifo.h"

#define FIFO_SIZE 10

FIFO_DEF(ff_non_overwritable , FIFO_SIZE, uint32_t, false, 0);

void setUp(void)
{
  fifo_clear(&ff_non_overwritable);
}

void tearDown(void)
{

}
void test_read_from_null_fifo(void)
{
  fifo_t ff_null = { 0 };
  uint32_t dummy;
  TEST_ASSERT_FALSE ( fifo_read(&ff_null, &dummy) );
}

void test_write_to_null_fifo(void)
{
  fifo_t ff_null = { 0 };
  uint32_t dummy;
  TEST_ASSERT_FALSE ( fifo_write(&ff_null, &dummy) );
}

void test_normal(void)
{
  for(uint32_t i=0; i < FIFO_SIZE; i++)
  {
    fifo_write(&ff_non_overwritable, &i);
  }

  for(uint32_t i=0; i < FIFO_SIZE; i++)
  {
    uint32_t c;
    fifo_read(&ff_non_overwritable, &c);
    TEST_ASSERT_EQUAL(i, c);
  }
}

void test_circular(void)
{
  FIFO_DEF(ff_overwritable, 2, uint32_t, true, 0);

  uint32_t data;

  // feed fifo to full
  data = 1;
  fifo_write(&ff_overwritable, &data); // 1
  data = 2;
  fifo_write(&ff_overwritable, &data); // 2

  // overflow data
  data = 100;
  fifo_write(&ff_overwritable, &data);

  //------------- 1st read should be 2, second is 100 -------------//
  fifo_read(&ff_overwritable, &data);
  TEST_ASSERT_EQUAL(2, data);

  fifo_read(&ff_overwritable, &data);
  TEST_ASSERT_EQUAL(100, data);
}

void test_is_empty(void)
{
  uint32_t dummy;
  TEST_ASSERT_TRUE(fifo_isEmpty(&ff_non_overwritable));
  fifo_write(&ff_non_overwritable, &dummy);
  TEST_ASSERT_FALSE(fifo_isEmpty(&ff_non_overwritable));
}

void test_is_full(void)
{
  TEST_ASSERT_FALSE(fifo_isFull(&ff_non_overwritable));

  for(uint32_t i=0; i < FIFO_SIZE; i++)
  {
    fifo_write(&ff_non_overwritable, &i);
  }

  TEST_ASSERT_TRUE(fifo_isFull(&ff_non_overwritable));
}

