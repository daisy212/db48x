// ****************************************************************************
//  target.cc                                                     DB48X project
// ****************************************************************************
//
//   File Description:
//
//
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

#include "target.h"

static const byte dmcp_position[] =
// ----------------------------------------------------------------------------
//   Convert DMCP key codes to row/column positions
// ----------------------------------------------------------------------------
{
#if (KBD_CASIO_50 == 0)
    KB_F1,      11,
    KB_F2,      12,
    KB_F3,      13,
    KB_F4,      14,
    KB_F5,      15,
    KB_F6,      16,

    KB_A,       21,
    KB_B,       22,
    KB_C,       23,
    KB_D,       24,
    KB_E,       25,
    KB_F,       26,

    KB_G,       31,
    KB_H,       32,
    KB_I,       33,
    KB_J,       34,
    KB_K,       35,
    KB_L,       36,

    KB_ENT,     41,
    KB_M,       42,
    KB_N,       43,
    KB_O,       44,
    KB_BKS,     45,

    KB_UP,      51,
    KB_P,       52,
    KB_Q,       53,
    KB_R,       54,
    KB_S,       55,

    KB_DN,      61,
    KB_T,       62,
    KB_U,       63,
    KB_V,       64,
    KB_W,       65,

    KB_SHIFT,   71,
    KB_X,       72,
    KB_Y,       73,
    KB_Z,       74,
    KB_SUB,     75,


    KB_ON,      81,
    KB_0,       82,
    KB_DOT,     83,
    KB_SPC,     84,
    KB_ADD,     85
#endif
#if (KBD_CASIO_50 == 1)
// functions key A1  A6 : 1  6
    KB_F1,      11,
    KB_F2,      12,
    KB_F3,      13,
    KB_F4,      14,
    KB_F5,      15,
    KB_F6,      16,

    K_B1,       21,
    K_B2,       22,
    K_B3,       23,
    K_B4,       24,
    K_B5,       25,
    K_B6,       26,

    K_C1,      31,
    K_C2,      32,
    K_C3,      33,
    K_C4,      34,
    K_C5,      35,
    K_C6,      36,


    K_D1,      41,
    K_D2,      42,
    K_D3,      43,
    K_D4,      44,
    K_D5,      45,
    K_D6,      46,

    K_E1,      51,
    K_E2,      52,
    K_E3,      53,
    K_E4,      54,
    K_E5,      55,
    K_E6,      56,

    K_F1,      61,
    K_F2,      62,
    K_F3,      63,
    K_F4,      64,
    K_F5,      65,

    K_G1,      71,
    K_G2,      72,
    K_G3,      73,
    K_G4,      74,
    K_G5,      75,

    K_H1,      81,
    K_H2,      82,
    K_H3,      83,
    K_H4,      84,
    K_H5,      85,
    
    K_I1,  //   91
    K_I2,      92,
    K_I3,      93,
    K_I4,      94,
    K_I5,      95,
//    K_I5,      95,
    0, 

#endif
};



uint platform_plane(bool ls, bool rs, bool al, bool lc, bool trans)
// ----------------------------------------------------------------------------
//   Create the platform plane for a combination of modifiers
// ----------------------------------------------------------------------------
{
    return ls + 2 * rs + 3 * al + 3 * (lc && al) + 10 * trans;
}


uint platform_keyid(uint pkey, bool ls, bool rs, bool al, bool lc, bool trans)
// ----------------------------------------------------------------------------
//   Create the platform plane for a combination of modifiers
// ----------------------------------------------------------------------------
{
    return pkey + 100 * platform_plane(ls, rs, al, lc, trans);
}


uint platform_keyid(uint rc, uint plane)
// ----------------------------------------------------------------------------
//   Return the complete key ID from HP48-style Row/Column/Plane position
// ----------------------------------------------------------------------------
// HP48 key codes are given in the form rc.ph, where r, c, p and h
// are one decimal digit each.
// r = row
// c = column
// p = plane
// h = hold (i.e. shift + key held simultaneously)
//
// On DMCP, we reinterpret "h" as "transient alpha"
//
// The plane is documented as follows:
// 0: Like 1
// 1: Unshifted
// 2: Left shift
// 3: Right shift
// 4: Alpha
// 5: Alpha left shift
// 6: Alpha right shift
//
// DB48X adds 7, 8 and 9 for lowercase alpha
{
    const size_t max = sizeof(dmcp_position) / sizeof(dmcp_position[0]);
    for (size_t k = 0; k < max; k += 2)
    {
        if (dmcp_position[k+1] == rc)
        {
            uint keyid = dmcp_position[k];
            if (plane)
            {
                plane = 100 * (plane/10 - 1) + 1000 * (plane % 10);
                keyid = keyid + plane;
            }
            return keyid;
        }
    }
    return 0;
}



uint compatible_key_position(uint key)
// ----------------------------------------------------------------------------
//   Return the key position for a given key ID
// ----------------------------------------------------------------------------
{
    // Strip the key plane
    key = key % 100;
    const size_t max = sizeof(dmcp_position) / sizeof(dmcp_position[0]);
    for (size_t k = 0; k < max; k += 2)
        if (dmcp_position[k] == key)
            return dmcp_position[k+1];
    return 0;
}


uint compatible_key_plane(uint keyid)
// ----------------------------------------------------------------------------
//   Return the keyplane in the format expected by RPL code
// ----------------------------------------------------------------------------
{
    uint plane = keyid / 100;
    uint h = plane / 10;
    uint p = plane % 10 + 1;
    return p * 10 + h;
}
