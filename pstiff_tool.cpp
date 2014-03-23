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

static
void _Warning(const char* module, const char* fmt, va_list ap)
{
    std::cerr << "## warning" << std::endl;
#if 0
    char e[512] = { '\0' };
    if (module != NULL)
        strcat(strcpy(e, module), ": ");

    vsprintf(e+strlen(e), fmt, ap);
    strcat(e, ".");
    if (Verbose) {

        fprintf(stderr, "\nWarning");
        fprintf(stderr, " %s\n", e);
        fflush(stderr);
    }
#endif
}

static
void _Error(const char* module, const char* fmt, va_list ap)
{
    std::cerr << " ## error" << std::endl;

#if 0
    char e[512] = { '\0' };

    if (module != NULL) {
        if (strlen(module) < 500)
               strcat(strcpy(e, module), ": ");
    }

    vsprintf(e+strlen(e), fmt, ap);
    strcat(e, ".");
    fprintf(stderr, "\nError");
    fprintf(stderr, " %s\n", e);
    fflush(stderr);
#endif
}

typedef unsigned char byte_t;

void ParsePhotoshop(const byte_t * p,int n)

{
    const byte_t *p0=p;
    while(p<p0+n) {
        std::string sig = std::string((char*)p,4);
        uint16_t     id  = ((uint16_t) *(p+4) << 8 | *(p+5));
        std::string name= std::string((char*)p+7,(int)*(p+6));

        std::cerr << "'" << sig << "' [";
        std::cerr << std::setw(4) << std::setfill('0')
                  << PsTiff::ImageResourceId(id).ToString() <<  "] '" << name << "'";

        p += name.length() & 0x1 != 0x0 ? 7+name.length(): 8+name.length();

        uint32_t s = (uint32_t)*(p+0) << 24 |(uint32_t)*(p+1)<<16
                   | (uint32_t)*(p+2) <<  8 |(uint32_t)*(p+3)<<0;

        std::cerr << "[" << std::hex << s << std::dec << "]" << std::endl;
        
        std::cerr << PsTiff::IO::hex_dump(std::string( (char*) (p+sizeof(uint32_t)),s)) << std::endl;
        
        p+=sizeof(uint32_t) + ((s & 0x1) == 0 ? s : s+1);
        

        std::cerr << "*******************************************"   << std::endl;
    }

    if(p-p0 != n) {
        throw std::runtime_error("failed to parse PostScript Tag");
    }
}


static const std::string Usage = "Usage: pstiff_dump tiff-file";

int main(int argc, char* argv[])
{
    {
        PsTiff::ImageResourceId i(2005);
        std::cerr << "#" << i.ToString() << std::endl;
    }

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
                std::cerr << "N=" << n << std::endl;
                ParsePhotoshop(data,n);
            }
        }
        n++;
    } while (TIFFReadDirectory(in));

    TIFFClose(in);

    return 0;
}

