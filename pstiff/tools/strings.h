//========================================================================
//
// pstiff/tools/String.h
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

#ifndef PSTIFF_TOOLS_STRINGS_H
#define PSTIFF_TOOLS_STRINGS_H

#include <locale>
#include <string>

namespace PsTiff {

    namespace Tools {
        inline
        std::string from_wstring(const std::wstring & si,const std::string & ec="en_US.utf8")         {
            typedef std::codecvt< wchar_t, char, std::mbstate_t > c_t;

            std::locale loc(ec.c_str());

            const c_t & f = std::use_facet<c_t>( loc );

            std::string s;
            {
                std::mbstate_t st=mbstate_t();

                const wchar_t *p = si.c_str();    // current start position in source
                const wchar_t *e = p+si.length(); // one of end
                const wchar_t *o = p;              // current end position reported by f.our

                while(o!=e)
                {
                    static const int n = 30;
                    static char c[n];
                    char *d=NULL;
                    f.out(st,p,e,o,c,c+n,d);
                    s+=std::string(c,d-c);
                    p=o;
                }
            }
            return s;
        }
    }
}

#endif // PSTIFF_TOOL_STRINGS_H
