/**************************************************************************/
/*!
    @file     ssd1306_i2c.h
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2012 K. Townsend
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
#ifndef __SSD1306_I2C_H__
#define __SSD1306_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "projectconfig.h"

#include "drivers/displays/smallfonts.h"

/*=========================================================================
    Display Size
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
    Select the appropriate display below to create an appropriately
    sized framebuffer, etc.

    SSD1306_128_64  128x64 pixel display

    SSD1306_128_32  128x32 pixel display

    You also need to set the LCDWIDTH and LCDHEIGHT defines to an
    appropriate size

    -----------------------------------------------------------------------*/
    #define SSD1306_128_64
    // #define SSD1306_128_32

    #if defined SSD1306_128_64 && defined SSD1306_128_32
      #error "Only one SSD1306 display can be specified at once in ssd1306.h"
    #endif
    #if !defined SSD1306_128_64 && !defined SSD1306_128_32
      #error "At least one SSD1306 display must be specified in ssd1306.h"
    #endif

    #if defined SSD1306_128_64
      #define SSD1306_LCDWIDTH            (128)
      #define SSD1306_LCDHEIGHT           (64)
    #endif
    #if defined SSD1306_128_32
      #define SSD1306_LCDWIDTH            (128)
      #define SSD1306_LCDHEIGHT           (32)
    #endif
/*=========================================================================*/


/*=========================================================================
    I2C Address - 011110+SA0 ... 0x3C for SA0 = 0, 0x3D for SA0 = 1
    ---------------------------------------------------------------------*/
    #define SSD1306_I2C_ADDRESS           (0x3D << 1)
    #define SSD1306_I2C_READWRITE         (0x01)
/*=========================================================================*/


/*=========================================================================
    Commands
    ---------------------------------------------------------------------*/
    #define SSD1306_SETCONTRAST           0x81
    #define SSD1306_DISPLAYALLON_RESUME   0xA4
    #define SSD1306_DISPLAYALLON          0xA5
    #define SSD1306_NORMALDISPLAY         0xA6
    #define SSD1306_INVERTDISPLAY         0xA7
    #define SSD1306_DISPLAYOFF            0xAE
    #define SSD1306_DISPLAYON             0xAF
    #define SSD1306_SETDISPLAYOFFSET      0xD3
    #define SSD1306_SETCOMPINS            0xDA
    #define SSD1306_SETVCOMDETECT         0xDB
    #define SSD1306_SETDISPLAYCLOCKDIV    0xD5
    #define SSD1306_SETPRECHARGE          0xD9
    #define SSD1306_SETMULTIPLEX          0xA8
    #define SSD1306_SETLOWCOLUMN          0x00
    #define SSD1306_SETHIGHCOLUMN         0x10
    #define SSD1306_SETSTARTLINE          0x40
    #define SSD1306_MEMORYMODE            0x20
    #define SSD1306_COMSCANINC            0xC0
    #define SSD1306_COMSCANDEC            0xC8
    #define SSD1306_SEGREMAP              0xA0
    #define SSD1306_CHARGEPUMP            0x8D
    #define SSD1306_EXTERNALVCC           0x01
    #define SSD1306_INTERNALVCC           0x02
    #define SSD1306_SWITCHCAPVCC          0x02
/*=========================================================================*/

err_t ssd1306Init ( uint8_t vccstate );
void    ssd1306DrawPixel ( uint8_t x, uint8_t y );
void    ssd1306ClearPixel ( uint8_t x, uint8_t y );
uint8_t ssd1306GetPixel ( uint8_t x, uint8_t y );
void    ssd1306ClearScreen ( void );
void    ssd1306Refresh ( void );
void    ssd1306DrawString( uint16_t x, uint16_t y, char* text, struct FONT_DEF font );
void    ssd1306ShiftFrameBuffer( uint8_t height );

#ifdef __cplusplus
}
#endif

#endif
