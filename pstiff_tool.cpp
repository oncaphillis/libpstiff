//========================================================================
//
// pstiff_tool.cpp
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

#include "tiffio.h"
#include "pstiff.h"
#include <stdlib.h>
#include <stdint.h>
#include <set>

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <pstiff/io/hex_dump.h>

#define TIFFTAG_PHOTOSHOP_DDB 37724

static
void _Warning(const char* module, const char* fmt, va_list ap) {
    std::stringstream ss;
    ss  << "## warning:";

    if (module != NULL)
        ss << module << ":";

    static const int n=100;
    static char b[n];

    if(vsnprintf(b, n,fmt, ap)>=n)
        ss << b << "...";
    else
        ss << b;;

    std::cerr << ss.str() << std::endl;
}

static
void _Error(const char* module, const char* fmt, va_list ap)
{
    std::stringstream ss;
    ss << " ## error:";

    if (module != NULL)
        ss << module << ":";
    static const int n=100;
    static char b[n];

    if(vsnprintf(b, n,fmt, ap)>=n)
        ss << b << "...";
    else
        ss << b;;

    std::cerr << ss.str() << std::endl;
}

typedef unsigned char byte_t;

void ParsePhotoshop(const byte_t * p,int n,std::ostream &os=std::cout) {
    const byte_t *p0=p;

    while(p<p0+n) {
        if(std::string((char*)p,4)!="8BIM")
            throw std::runtime_error("expected '8BIM' signature");

        
        uint16_t     id  = ((uint16_t) *(p+4) << 8 | *(p+5));
        std::string name= std::string((char*)p+7,(int)*(p+6));
        
        os << std::setw(4) << std::setfill('0')
           << PsTiff::ImageResourceId(id).ToString() <<  "] '" << name << "'";
        
        p += name.length() & 0x1 != 0x0 ? 7+name.length(): 8+name.length();

        uint32_t s = (uint32_t)*(p+0) << 24 |(uint32_t)*(p+1)<<16
            | (uint32_t)*(p+2) <<  8 |(uint32_t)*(p+3)<<0;
        
        os << "[" << s << "]" << std::endl;
        
        os << PsTiff::IO::hex_dump(std::string( (char*) (p+sizeof(uint32_t)),s)) << std::endl;
        
        p+=sizeof(uint32_t) + ((s & 0x1) == 0 ? s : s+1);
        

        os << "*******************************************"   << std::endl;
    }
    
    if(p-p0 != n) {
        throw std::runtime_error("failed to parse PostScript Tag");
    }
}

void ParsePhotoshopDDB(const byte_t * p,int n,std::ostream &os=std::cout) {
    const byte_t *p0=p;
    std::string s((char*)p);
    p+=s.length()+1;

    std::string t;
    t+=p[3]; t+=p[2]; t+=p[1]; t+=p[0];

    std::string i;
    i+=p[7]; i+=p[6]; i+=p[5]; i+=p[4];
    
    p+=8;

    uint32_t nn =  (uint32_t)p[0] |  (uint32_t)p[1] << 8 |  (uint32_t)p[2] << 16 |  (uint32_t)p[3] << 24;  

    os << s << "[" << t << "|" << i << "]::" << nn << std::endl;
}


static const std::string Usage = "Usage: pstiff_dump tiff-file";

int main(int argc, char* argv[]) {
    TIFF *in, *out;

    TIFFSetErrorHandler(_Error);
    TIFFSetWarningHandler(_Warning);

    if(argc!=2)
        std::cerr << Usage << std::endl;
    
    if((in = TIFFOpen(argv[1], "r"))==NULL)
    {
        std::cerr << "Unable to open '" << argv[1] << "'" << std::endl;
        ::exit(1);
    }

    int n=0;

    do {
        {
            int n;
            byte_t *data;

            if(TIFFGetField(in,TIFFTAG_PHOTOSHOP,&n,&data)==1) {
                ParsePhotoshop(data,n,std::cout);
            }
#if 0
            if(TIFFGetField(in,TIFFTAG_PHOTOSHOP_DDB,&n,&data)==1) {
                std::cout << "DDB::" << n << std::endl
                          << PsTiff::IO::hex_dump(data,n)
                          << std::endl;
                ParsePhotoshopDDB(data,n);
            }

            if(TIFFGetField(in,TIFFTAG_XMLPACKET,&n,&data)==1) {
                std::cout << "XMP" << n << std::endl
                          << std::string((char*)data,n)
                          << std::endl;
            }
#endif
        }
        n++;
    } while (TIFFReadDirectory(in));

    TIFFClose(in);

    return 0;
}

