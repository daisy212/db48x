#ifndef TARGET_DM42_H
#define TARGET_DM42_H
// ****************************************************************************
//  target.h                                                      DB48X project
// ****************************************************************************
//
//   File Description:
//
//    Description of the DM42 platform
//
//
//
//
//
//
//
//
// ****************************************************************************
//   (C) 2022 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the terms outlined in LICENSE.txt
// ****************************************************************************
//   This file is part of DB48X.
//
//   DB48X is free software: you can redistribute it and/or modify
//   it under the terms outlined in the LICENSE.txt file
//
//   DB48X is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// ****************************************************************************

#include "blitter.h"
#include "dmcp.h"

enum target
// ----------------------------------------------------------------------------
//   Constants for a given target
// ----------------------------------------------------------------------------
{
#if (KBD_CASIO_50 == 1)
  LCD_W          = 536,
  LCD_H          = 336,
  LCD_SCANLINE   = 536,
  #ifndef CONFIG_COLOR
    BITS_PER_PIXEL = 1,
  #else
    BITS_PER_PIXEL = 16,
  #endif
#else
  #ifndef CONFIG_COLOR
    LCD_W          = 400,
    LCD_H          = 240,
    LCD_SCANLINE   = 416,
    BITS_PER_PIXEL = 1,
  #else
    LCD_W          = 400,
    LCD_H          = 240,
    LCD_SCANLINE   = 400,
    BITS_PER_PIXEL = 16,
  #endif
#endif


};

// We need to reverse grobs during parsing and rendering
#define REVERSE_GROBS

#ifdef CONFIG_COLOR
using surface = blitter::surface<blitter::mode::RGB_16BPP>;
using color   = blitter::color  <blitter::mode::RGB_16BPP>;
using pattern = blitter::pattern<blitter::mode::RGB_16BPP>;
#else
using surface = blitter::surface<blitter::mode::MONOCHROME_REVERSE>;
using color   = blitter::color  <blitter::mode::MONOCHROME_REVERSE>;
using pattern = blitter::pattern<blitter::mode::MONOCHROME_REVERSE>;
#endif
using coord   = blitter::coord;
using size    = blitter::size;
using rect    = blitter::rect;
using point   = blitter::point;
using pixword = blitter::pixword;

extern surface Screen;


// Soft menu tab size
#define MENU_TAB_SPACE      1
#define MENU_TAB_INSET      2
#define MENU_TAB_WIDTH      ((LCD_W - 5 * MENU_TAB_SPACE) / 6)
#define MENU_TAB_HEIGHT     (FONT_HEIGHT(FONT_MENU) + 2 * MENU_TAB_INSET)

// Put slow-changing data in the QSPI
#if SIMULATOR
#  define FONT_QSPI
#else
#  define FONT_QSPI __attribute__((section(".fonts")))
#endif // SIMULATOR

// Keyboard, toggles functions keys
extern uint8_t MemT_Main_Math_Tool;
extern uint8_t MemT_Cat_Mem_Lib;
extern uint8_t MemT_Prg_Dbg_Mem;
extern uint8_t MemT_Trig_Ang_Hypb;
extern uint8_t MemT_Base_Numb_Frac;
extern uint8_t MemT_Pow_Real_Cplx;
extern uint8_t MemT_Solv_Eq_Fin;
extern uint8_t MemT_STK_Cte_Parts;
extern uint8_t MemT_Unit_Cnv_Cte;


/*
    KEYBOARD BIT MAP
    ----------------
    This is the bit number in the 64-bit keymatrix.
    Bit set means key is pressed.
    Note that DMCP does not define keys as bitmaps,
    but rather using keycodes.

      +--------+--------+--------+--------+--------+--------+
      |   F1   |   F2   |   F3   |   F4   |   F5   |   F6   |
      |   38   |   39   |   40   |   41   |   42   |   43   |
      +--------+--------+--------+--------+--------+--------+
    S |  Sum-  |  y^x   |  x^2   |  10^x  |  e^x   |  GTO   |
      |  Sum+  |  1/x   |  Sqrt  |  Log   |  Ln    |  XEQ   |
      |   1    |   2    |   3    |   4    |   5    |   6    |
    A |   A    |   B    |   C    |   D    |   E    |   F    |
      +--------+--------+--------+--------+--------+--------+
    S | Complx |   %    |  Pi    |  ASIN  |  ACOS  |  ATAN  |
      |  STO   |  RCL   |  R_dwn |   SIN  |   COS  |   TAN  |
      |   7    |   8    |   9    |   10   |   11   |   12   |
    A |   G    |   H    |   I    |    J   |    K   |    L   |
      +--------+--------+--------+--------+--------+--------+
    S |     Alpha       | Last x |  MODES |  DISP  |  CLEAR |
      |     ENTER       |  x<>y  |  +/-   |   E    |   <--  |
      |       13        |   14   |   15   |   16   |   17   |
    A |                 |    M   |    N   |    O   |        |
      +--------+--------+-+------+----+---+-------++--------+
    S |   BST  | Solver   |  Int f(x) |  Matrix   |  STAT   |
      |   Up   |    7     |     8     |     9     |   /     |
      |   18   |   19     |    20     |    21     |   22    |
    A |        |    P     |     Q     |     R     |    S    |
      +--------+----------+-----------+-----------+---------+
    S |   SST  |  BASE    |  CONVERT  |  FLAGS    |  PROB   |
      |  Down  |    4     |     5     |     6     |    x    |
      |   23   |   24     |    25     |    26     |   27    |
    A |        |    T     |     U     |     V     |    W    |
      +--------+----------+-----------+-----------+---------+
    S |        | ASSIGN   |  CUSTOM   |  PGM.FCN  |  PRINT  |
      |  SHIFT |    1     |     2     |     3     |    -    |
      |   28   |   29     |    30     |    31     |   32    |
    A |        |    X     |     Y     |     Z     |    -    |
      +--------+----------+-----------+-----------+---------+
    S |  OFF   |  TOP.FCN |   SHOW    |   PRGM    | CATALOG |
      |  EXIT  |    0     |     .     |    R/S    |    +    |
      |   33   |   34     |    35     |    36     |   37    |
    A |        |    :     |     .     |     ?     |   ' '   |
      +--------+----------+-----------+-----------+---------+

*/

#if (KBD_CASIO_50 == 1)
    #define KB_ROW     9
    #define POFF_COL   4
    #define POFF_ROW   3

  // WARNING missing values not allowed by db48x struct construction, KEY_SIGMA = 1
  // mapping with free42 method
  #define     K_A1     45
  #define     K_A2     46
  #define     K_A3     47
  #define     K_A4     48
  #define     K_A5     49
  #define     K_A6     50

  #define     K_B1     1
  #define     K_B2     2
  #define     K_B3     3
  #define     K_B4     4
  #define     K_B5     5
  #define     K_B6     6

  #define     K_C1     7
  #define     K_C2     8
  #define     K_C3     9
  #define     K_C4     10
  #define     K_C5     11
  #define     K_C6     12

  #define     K_D1     13
  #define     K_D2     14
  #define     K_D3     15
  #define     K_D4     16
  #define     K_D5     17
  #define     K_D6     18

  #define     K_E1     19
  #define     K_E2     20
  #define     K_E3     21
  #define     K_E4     22
  #define     K_E5     23
  #define     K_E6     24

  #define     K_F1     25
  #define     K_F2     26
  #define     K_F3     27
  #define     K_F4     28
  #define     K_F5     29

  #define     K_G1     30
  #define     K_G2     31
  #define     K_G3     32
  #define     K_G4     33
  #define     K_G5     34

  #define     K_H1     35
  #define     K_H2     36
  #define     K_H3     37
  #define     K_H4     38
  #define     K_H5     39

  #define     K_I1     40
  #define     K_I2     41
  #define     K_I3     42
  #define     K_I4     43
  #define     K_I5     44
  // functions to key
  #define KB_F1        K_A1         //! Function key 1
  #define KB_F2        K_A2         //! Function key 2
  #define KB_F3        K_A3         //! Function key 3
  #define KB_F4        K_A4         //! Function key 4
  #define KB_F5        K_A5         //! Function key 5
  #define KB_F6        K_A6         //! Function key 6

  #define KB_0         K_I1         //! 0
  #define KB_1         K_H1         //! 1
  #define KB_2         K_H2         //! 2
  #define KB_3         K_H3         //! 3
  #define KB_4         K_G1         //! 4
  #define KB_5         K_G2         //! 5
  #define KB_6         K_G3         //! 6
  #define KB_7         K_F1         //! 7
  #define KB_8         K_F2         //! 8
  #define KB_9         K_F3         //! 9

  #define KB_A         K_D1         //! A          
  #define KB_B         K_D2         //! B
  #define KB_C         K_D3         //! C
  #define KB_D         K_D4         //! D
  #define KB_E         K_D5         //! E
  #define KB_F         K_D6         //! F

  #define KB_G         K_E1         //! G
  #define KB_H         K_E2         //! H
  #define KB_I         K_E3         //! I
  #define KB_J         K_E4         //! J
  #define KB_K         K_E5         //! K
  #define KB_L         K_E6         //! L

  #define KB_M         K_F1         //! M
  #define KB_N         K_F2         //! N
  #define KB_O         K_F3         //! O

  #define KB_P         K_G1         //! P
  #define KB_Q         K_G2         //! Q
  #define KB_R         K_G3         //! R
  #define KB_S         K_G4         //! S
  #define KB_T         K_G5         //! T

  #define KB_U         K_H1         //! U
  #define KB_V         K_H2         //! V
  #define KB_W         K_H3         //! W
  #define KB_X         K_H4         //! X
  #define KB_Y         K_H5         //! Y

  #define KB_Z         K_I2         //! Z

  #define KEY_SIGMA    K_C2           // quick menu
  #define KEY_INV      K_D6           // 1/x
  #define KEY_SQRT     K_D4           // ^0.5
  // needed for tests
  #define KEY_LOG      K_D2           // 
  #define KEY_LN       K_D3           // 
  #define KEY_XEQ      K_E6           // equation % algebric


  #define KEY_N1       K_C1
  #define KEY_N2       K_C2
  #define KEY_N3       K_C3
  #define KEY_N4       K_C4
  #define KEY_N5       K_C5
  #define KEY_N6       K_C6

  #define KEY_STO      K_B2     // not used ??
  #define KEY_RCL      K_B3
  #define KEY_RDN      K_E3     // STK
  #define KEY_SIN      70     // not used ??
  #define KEY_COS      71     // not used ??
  #define KEY_TAN      72     // not used ??


  #define KEY_ENTER    K_F5

  #define KEY_EQ_AL    K_E6           // equation and algebric : '

  #define KEY_SWAP     K_E5
  #define KEY_CHS      K_I4           // orginal casio
  #define KEY_E        K_I3
  #define KEY_BSP      K_F4


  #define KEY_7        KB_7
  #define KEY_8        KB_8
  #define KEY_9        KB_9
  #define KEY_DIV      K_G5
  #define KEY_4        KB_4
  #define KEY_5        KB_5
  #define KEY_6        KB_6
  #define KEY_MUL      K_G4
  #define KEY_1        KB_1
  #define KEY_2        KB_2
  #define KEY_3        KB_3
  #define KEY_SUB      K_H5
  #define KEY_0        KB_0
  #define KEY_DOT      K_I2
  #define KEY_RUN      K_I5
  #define KEY_ADD      K_H4

  #define KEY_SHIFT    K_B1
  #define KEY_SHIFT2   K_B4
  #define KEY_ALPHA    K_C1
  #define KEY_CAT      K_C3


  #define KEY_UP       K_B6
  // original casio layout
  #define KEY_EXIT     K_C4          // used 15x
  #define KEY_LEFT     K_B5
  #define KEY_DOWN     K_C5
  #define KEY_RIGHT    K_C6


  #define KEY_F1       K_A1
  #define KEY_F2       K_A2
  #define KEY_F3       K_A3
  #define KEY_F4       K_A4
  #define KEY_F5       K_A5
  #define KEY_F6       K_A6

  #define KEY_SCREENSHOT 51
  #define KEY_SH_UP      52
  #define KEY_SH_DOWN    53
  #define KEY_DOUBLE_RELEASE 99

  #define   KEY_PAGEUP          KEY_DIV
  #define   KEY_PAGEDOWN        KEY_MUL
  #define   K_Main_Math_Tool    K_C2
  #define   K_Cat_Mem_Lib	    K_C3
  #define   K_Trig_Ang_Hypb	    K_D1
  #define   K_Base_Numb_Frac	  K_D2
  #define   K_Pow_Real_Cplx		  K_D3
  #define   K_Unit_Cnv_Cte		  K_E1
  #define   K_Solv_Eq_Fin	      K_E2
  #define   K_Prg_Dbg_Mem	      K_E3
  #define   K_STK_Cte_Parts	    K_E4

#else 
  #define KB_ALPHA     28         //! Alpha
  #define KB_ON        33         //! ON
  #define KB_ESC       33         //! Exit
  #define KB_DOT       35         //! Dot
  #define KB_SPC       36         //! Space (on R/S)
  #define KB_RUNSTOP   36         //! R/S
  #define KB_QUESTION  36         //! ?
  #define KB_SHIFT     28         //! Shift
  #define KB_LSHIFT    28         //! Left shift
  #define KB_RSHIFT    28         //! Right shift

  #define KB_ADD       37         //! +
  #define KB_SUB       32         //! -
  #define KB_MUL       27         //! *
  #define KB_DIV       22         //! /

  #define KB_ENT       13         //! ENTER
  #define KB_BKS       17         //! backspace
  #define KB_UP        18         //! up arrow
  #define KB_DN        23         //! down arrow
  #define KB_LF        18         //! left arrow
  #define KB_RT        23         //! right arrow

  #define KB_F1        38         //! Function key 1
  #define KB_F2        39         //! Function key 2
  #define KB_F3        40         //! Function key 3
  #define KB_F4        41         //! Function key 4
  #define KB_F5        42         //! Function key 5
  #define KB_F6        43         //! Function key 6

  #define KB_0         34         //! 0
  #define KB_1         29         //! 1
  #define KB_2         30         //! 2
  #define KB_3         31         //! 3
  #define KB_4         24         //! 4
  #define KB_5         25         //! 5
  #define KB_6         26         //! 6
  #define KB_7         19         //! 7
  #define KB_8         20         //! 8
  #define KB_9         21         //! 9

  #define KB_A          1         //! A
  #define KB_B          2         //! B
  #define KB_C          3         //! C
  #define KB_D          4         //! D
  #define KB_E          5         //! E
  #define KB_F          6         //! F
  #define KB_G          7         //! G
  #define KB_H          8         //! H
  #define KB_I          9         //! I
  #define KB_J         10         //! J
  #define KB_K         11         //! K
  #define KB_L         12         //! L
  #define KB_M         14         //! M
  #define KB_N         15         //! N
  #define KB_O         16         //! O
  #define KB_P         19         //! P
  #define KB_Q         20         //! Q
  #define KB_R         21         //! R
  #define KB_S         22         //! S
  #define KB_T         24         //! T
  #define KB_U         25         //! U
  #define KB_V         26         //! V
  #define KB_W         27         //! W
  #define KB_X         29         //! X
  #define KB_Y         30         //! Y
  #define KB_Z         31         //! Z 
  #define KEY_SHIFT2   999
  #define KEY_ALPHA    999
  #define KEY_LEFT     999
  #define KEY_RIGHT    999


#endif

uint platform_plane(bool ls, bool rs, bool al, bool lc, bool trans);
uint platform_keyid(uint k, bool ls, bool rs, bool al, bool lc, bool trans);
uint platform_keyid(uint rc, uint plane);
uint compatible_key_position(uint key);
uint compatible_key_plane(uint keyid);



// ============================================================================
//
//    Battery configuration
//
// ============================================================================

#define BATTERY_VMAX    3000    // Max battery on display

#endif // TARGET_DM42_H
