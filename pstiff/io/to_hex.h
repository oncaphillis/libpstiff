//========================================================================
//
// pstiff/io/to_hex.h
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

#ifndef PSTIFF_IO_TOHEX_H 
#define PSTIFF_IO_TOHEX_H 
 
#include <locale> 
#include <sstream> 
#include <iomanip> 
 
namespace PsTiff  { 
    namespace IO { 
        // The most generic string =>  hex transformator
        //

        template<class I,class O> 
        void hex_transform(const std::basic_string<I> & in,std::basic_string<O> & out,
                        const std::basic_string<O> & gap=std::basic_string<O>() )  { 
            typedef std::basic_string<I> i_t; 
            typedef std::basic_string<O> o_t; 

            unsigned int m = 0x00; 
 
            for(size_t i=0;i < sizeof( I );i++) 
            { 
                m = (m << 8) | 0xff; 
            } 
 
            std::basic_stringstream<O> ss; 
            
            ss << std::hex; 
 
            for(typename i_t::const_iterator i = in.begin();i != in.end();i++) 
            { 
                ss << (i==in.begin() ? o_t() : gap)  << std::setw( sizeof( O ) * 2 ) << std::setfill(O('0')) << (unsigned int) (*i & m);
            }              
            out = ss.str(); 
        } 
 
        template<class C> 
        std::string to_hex(const std::basic_string<C> & in,const std::string & gap="") { 
            std::string o; 
            hex_transform(in,o,gap); 
            return o; 
        } 

        template<class C> 
        std::string to_hex(const C * in,const std::string & gap="") { 
            return to_hex(std::basic_string<C>(in),gap);
        } 

        template<class C> 
        std::string to_hex(const C * in,size_t n,const std::string & gap="") { 
            return to_hex(std::basic_string<C>(in,n),gap);
        } 
    } 
} 
 
#endif // PSTIFF_IO_TOHEX_H 
