//========================================================================
//
// pstiff/Types.h
//
// Copyright 2014 Sebastian Kloska (oncaphillis@snafu.de)
//
// PSTIFF is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//
//========================================================================

#ifndef PSTIFF_TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdexcept>

namespace PsTiff {
    typedef unsigned char Byte_t;

    inline Byte_t  to8(const Byte_t *p) {
        return *p;
    }


    inline
    uint16_t to16(const Byte_t * p) {
        return (uint16_t)p[0] << 8 | (uint16_t)p[1];
    }

    inline
    uint32_t to32(const Byte_t * p) {
        return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
               (uint32_t)p[2] << 16 | (uint32_t)p[3] << 0;
    }
}

#endif // PSTIFF_TYPES_H
