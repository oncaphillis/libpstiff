//========================================================================
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
// along with PSTIFF; if not, write to the Free Software
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
    Byte_t * from8(Byte_t *p,Byte_t b) {
        p[0]=b;
        return p+1;
    }


    inline
    uint16_t to16(const Byte_t * p) {
        return (uint16_t)p[0] << 8 | (uint16_t)p[1];
    }

    inline
    Byte_t * from16(Byte_t *p,uint16_t w) {
        p[0] = (Byte_t)(w >> 8 & 0xff);
        p[1] = (Byte_t)(w >> 0 & 0xff);
        return p+2;
    }

    inline
    uint32_t to32(const Byte_t * p) {
        return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
               (uint32_t)p[2] << 16 | (uint32_t)p[3] << 0;
    }

    inline
    Byte_t * from32(Byte_t *p,uint32_t ww) {
        p[0] = (Byte_t)(ww >> 24 & 0xff);
        p[1] = (Byte_t)(ww >> 16 & 0xff);
        p[2] = (Byte_t)(ww >>  8 & 0xff);
        p[3] = (Byte_t)(ww >>  0 & 0xff);
        return p+4;
    }

    inline
    Byte_t * fromstr(Byte_t * p,const std::string & s) {
        p[0] = (Byte_t) s.length();
        for(int i=0;i<s.length();i++)
            p[1+i] = s[i];
        return p+1+s.length();
    }

    inline
    Byte_t * fromstr(Byte_t * p,const std::wstring & s) {
        p[0] = (Byte_t) s.length();
        for(int i=0;i<s.length();i++) {
            p[ 1 + i * 2 + 0 ] = (Byte_t)((s[i] >> 0) & 0xff);
            p[ 1 + i * 2 + 1 ] = (Byte_t)((s[i] >> 8) & 0xff);
        }
        return p+1+s.length()*2;
    }
}

#endif // PSTIFF_TYPES_H
