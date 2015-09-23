/**************************************************************************/
/*! 
    @file     fonts.h
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
#ifndef __FONTS_H__
#define __FONTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "projectconfig.h"

/**************************************************************************/
/*! 
    @brief Describes a single character's display information
*/
/**************************************************************************/
typedef struct
{
  const uint8_t widthBits;              // width, in bits (or pixels), of the character
  const uint16_t offset;                // offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
} FONT_CHAR_INFO;	

/**************************************************************************/
/*! 
    @brief Describes a single font
*/
/**************************************************************************/
typedef struct
{
  const uint8_t           height;       // height of the font's characters
  const uint8_t           startChar;    // the first character in the font (e.g. in charInfo and data)
  const uint8_t           endChar;      // the last character in the font (e.g. in charInfo and data)
  const FONT_CHAR_INFO*	  charInfo;     // pointer to array of char information
  const uint8_t*          data;         // pointer to generated array of character visual representation
} FONT_INFO;

void      fontsDrawString      ( uint16_t x, uint16_t y, uint16_t color, const FONT_INFO *fontInfo, char *str );
uint16_t  fontsGetStringWidth  ( const FONT_INFO *fontInfo, char *str ); 

#ifdef __cplusplus
}
#endif 

#endif
