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

#ifndef PSTIFF_RESOURCELIST_H
#define PSTIFF_RESOURCELIST_H

#include "tiffio.h"
#include <pstiff/Resource.h>
#include <memory.h>

namespace PsTiff {
    class ResourceList {
    private:
        typedef Resource resource_t;
        typedef std::vector<const resource_t *> vector_t;

    public:
        typedef vector_t::const_iterator        const_iterator;
        ResourceList() : _p(NULL) {

        }

        ~ResourceList() {
            delete[] _p;
        }

        void add(const resource_t *rp) {
            _v.push_back(rp);
        }

        size_t size() const {
            return _v.size();
        }

        int get_size() const {
            int s=0;
            for(vector_t::const_iterator i=_v.begin();i!=_v.end();i++) {
                s += (*i)->get_size();
            }
            return s;
        }

        const Byte_t * get_raw() {
            delete[] _p;
            int s = get_size();
            int s1=0;
            Byte_t *pp = _p = new Byte_t[s];

            ::memset(_p,0,s);

            for(vector_t::const_iterator i=_v.begin();i!=_v.end();i++) {
                ::memcpy(pp,(*i)->get_raw(),(*i)->get_size());
                pp += (*i)->get_size();
                s1+=(*i)->get_size();
            }

            return _p;
        }

        const_iterator begin() const  {
            return _v.begin();
        }

        const_iterator end() const  {
            return _v.end();
        }

        bool read(const std::string & path);
        bool write(const std::string & path);

        bool read(TIFF * in);
        bool write(TIFF * out);

    private:
        Byte_t * _p;
        vector_t _v;
    };
}
#endif  // PSTIFF_RESOURCELIST_H
