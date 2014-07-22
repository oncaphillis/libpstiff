//========================================================================
//
// pstiff/Resource.h
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

#ifndef PSTIFF_RESOURCE_H
#define PSTIFF_RESOURCE_H

#include "pstiff/ResourceId.h"
#include "pstiff/io/hex_dump.h"
#include "pstiff/tools/strings.h"

#include <vector>
#include <iostream>

namespace PsTiff
{
    class Resource {
    public:
        typedef ResourceId Id_t;

        Resource(const std::string & n,Id_t id) : _p(NULL),_name(n),_size(0),_id(id)
        {
        }

        Resource(const Byte_t * p=NULL) : _p(p),_size(0),_id(0) {
            if(p!=NULL)   {
                if(std::string((char*)p,4)!="8BIM")
                    throw std::runtime_error("expected '8BIM' tag found "+std::string((char*)p,4));


                _id   = ((uint16_t) *(p+4) << 8 | *(p+5));
                _name = std::string((char*)p+7,(int)*(p+6));
                p    += _name.length() & 0x1 != 0x0 ? 7 + _name.length() : 8 + _name.length();
                _size = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 | (uint32_t) p[2] << 8 | (uint32_t) p[3] << 0;
            }
        }

        ~Resource() {
        }

        uint32_t get_size() const {
            if(_p!=NULL)    {
                return 4 + sizeof(uint16_t) + (_name.length() & 0x1 ? _name.length()+1 : _name.length() + 2  ) + sizeof(uint32_t) + ( (get_raw_size() & 0x1) == 0 ? get_raw_size() : get_raw_size()+1);
            }
            return 0;
        }

        const Id_t & get_id() const  {
            return _id;
        }

        virtual
        const Byte_t *  get_data() const {
            if(_p!=NULL) {
                return _p+4+sizeof(uint16_t)+(_name.length() & 0x1 ? _name.length()+1 : _name.length() + 2) + sizeof(uint32_t);
            }
            return NULL;
        }

        const std::string & get_name () const {
            return _name;
        }

        virtual
        uint32_t get_raw_size() const {
            return _size;
        }

    private:
        const Byte_t    * _p;
        uint32_t          _size;
        ResourceId        _id;
        std::string       _name;
    };

    inline
    std::ostream & operator<<(std::ostream & os,const Resource & r) {
        return os << "[" << r.get_id() << "(" << "'" << r.get_name() << "') " << r.get_raw_size() << "/" << r.get_size() << "] ";
    }

    class SpotColorResource : public Resource
    {
    private:
        typedef Resource super;

    public:
        typedef struct Channel {
            Channel(uint32_t i,uint16_t s,uint16_t v0,uint16_t v1,uint16_t v2,uint16_t v3)
                :id(i),sp(s) {
                v[0]=v0; v[1]=v1; v[2]=v2; v[3]=v3;
            }

            const
            int16_t & operator[](size_t i) const {
                if(i<0 || i>=4)
                    throw std::runtime_error("illegal index");
                return v[i];
            }
            uint32_t id;  // channle ID
            uint16_t sp;  // color space
            int16_t v[4];
        } Channel_t;

        SpotColorResource(const Byte_t *p) : super(p)  {
            if(get_id()!=ResourceId::AlternateSpotColors) {
                std::stringstream ss;
                ss << "illegal id #" << get_id() << " for SpotColorResource";
                throw std::runtime_error(ss.str());
            }

            if(get_size()<4)
                throw  std::runtime_error("SpotColorResource has to have a size of min 4");


            _v = to16(get_data()+0);
            _s = to16(get_data()+2);

            if(get_raw_size()!=_s*14+4)
            {
                std::stringstream ss;
                ss << "expected data of SpotColorResources to have size" << _s*14+4
                      << " but found " << get_raw_size();
                throw std::runtime_error(ss.str());
            }
            const Byte_t * pp=get_data()+4;

            for(int i=0;i<_s;i++) {
                _ch.push_back(Channel_t(to32(pp),to16(pp+4),
                                        to16(pp+6),to16(pp+8),to16(pp+10),to16(pp+12)));
                pp+=14;
            }

        }

        size_t get_count() const {
            return _ch.size();
        }

        const Channel_t & operator[](const size_t idx) const {
            std::stringstream ss;
            if(idx<0 || idx>=get_count()) {
                ss << "illegal idx #" << idx << " for spot color table of size #" << get_count();
                throw std::runtime_error(ss.str());
            }
            return _ch[idx];
        }

    private:
         uint16_t               _v;  // version
         uint16_t               _s;  // size aka # of channels
         std::vector<Channel_t> _ch; // extra channels
    };

    inline
    std::ostream & operator<<(std::ostream & os,const SpotColorResource::Channel_t & c) {
        os << "id=" << c.id << ";spc=" << c.sp << " ";
        for(int i=0;i<4;i++) {
            os << (i==0 ? "" : ";") << c[i];
        }
        return os;
    }

    inline
    std::ostream & operator<<(std::ostream &os,const SpotColorResource & r) {
        os << "[COLOR]";
        for(int i=0;i<r.get_count();i++) {
            os << "[" << r[i] << "]";
        }
        return os;
    }

    template<class CI,class CE>
    class BasicAlphaNamesResource : public Resource {
    private:
        typedef Resource super;
    public:
        typedef std::basic_string<CI> String_t;
        typedef CI                    Char_t;
        typedef CE                    Ext_t;
        BasicAlphaNamesResource(const Byte_t * p) : super(p) {

        }

        const String_t & operator[](size_t idx) const {
            if(idx>=_c.size())
                throw std::runtime_error("illegal idx");
            return _c[idx];
         }

        String_t & operator[](size_t i) {
            if(i>=_c.size())
                throw std::runtime_error("illegal idx");
            return _c[i];
        }

        void clear() {
            _c.clear();
        }

        void push_back(const String_t & s) {
            _c.push_back(s);
        }

        size_t size() const {
            return _c.size();
        }

    private:
        std::vector<String_t> _c;
    };

    template<class CI,class CE>
    inline
    std::ostream & operator<<(std::ostream &os,const BasicAlphaNamesResource<CI,CE> & r ) {
        os << "[ALPHANAMES](";
        for(int i=0;i<r.size();i++) {
            os << ( i==0 ? "" : ";") << r[i];
        }
        return os << ")";
    }

    template<>
    inline
    std::ostream & operator<< <wchar_t,uint16_t> (std::ostream &os,
                                                  const BasicAlphaNamesResource<wchar_t,uint16_t> & r ) {
        os << "[UALPHANAMES](";
        for(int i=0;i<r.size();i++) {
            os << ( i==0 ? "" : ";") << PsTiff::Tools::from_wstring(r[i]);
        }
        return os << ")";
    }

    class  AlphaNamesResource : public BasicAlphaNamesResource<char,char> {
    private:
        typedef BasicAlphaNamesResource<char,char> super;
    public:
        AlphaNamesResource(const Byte_t * p) : super(p) {
            clear();
            String_t s;
            if(get_id()!=ResourceId::AlphaNames)
                throw std::runtime_error("Expected AlphaNames Resource Id");

            const Byte_t *p0;
            const Byte_t *p1;
            if((p1=p0=get_data())!=NULL) {
                Byte_t  n = 0;
                while((p1+sizeof(uint16_t)-p0)<get_raw_size() && (n=to8(p1)))
                {
                    push_back(String_t((Char_t*)(p1+sizeof(Byte_t)),(int)n));
                    p1+=sizeof(Byte_t)+n;
                }
            }
            if(p1-p0!=get_raw_size()) {
                std::stringstream ss;
                ss << "expected " << get_raw_size() << " bytes; found " << (p1-p1) << std::endl;
            }
        }
    };

    class UnicodeAlphaNamesResource : public BasicAlphaNamesResource<wchar_t,uint16_t> {
    private:
        typedef BasicAlphaNamesResource<wchar_t,uint16_t> super;
    public:
        UnicodeAlphaNamesResource(const Byte_t * p) : super(p) {
            clear();
            String_t s;

            if(get_id()!=ResourceId::UnicodeAlphaNames)
                throw std::runtime_error("Expected AlphaNames Resource Id");

            const Byte_t *p0;
            const Byte_t *p1;

            if((p1=p0=get_data())!=NULL) {
                while(((p1+sizeof(uint32_t)-p0)<get_raw_size()))
                {
                    String_t w;
                    uint32_t  n = to32(p1);

                    p1+=sizeof(uint32_t);

                    for(int i=0;i<n;i++)
                    {
                        if(to16(p1)!=0)
                            w+=Char_t(to16(p1));
                        p1+=sizeof(uint16_t);
                    }
                    push_back(w);
                }
            }

            if(p1-p0!=get_raw_size()) {
                std::stringstream ss;
                ss << "expected " << get_raw_size() << " bytes; found " << (p1-p1) << std::endl;
            }
        }
    };

    class AlphaIdentifiersResource : public Resource {
    private:
        typedef Resource super;
    public:
        // Not initilized from a TIFF file
        AlphaIdentifiersResource() : super("Alpha",2) {

        }

        AlphaIdentifiersResource(const Byte_t * p) : super(p) {
            if(get_id()!=ResourceId::AlphaIdentifiers)
                throw std::runtime_error("Expected AlphaChannelIds");
            if((get_raw_size() % sizeof(uint32_t))!=0)
                throw std::runtime_error("Illegal size of AlphaChannelIdsResource");

            for(int i=0;i<get_raw_size() / sizeof(uint32_t);i++)
            {
                push_back(to32(get_data()+i*sizeof(uint32_t)));
            }
        }

        void push_back(uint32_t n) {
            _c.push_back(n);
        }

        size_t size() const {
            return _c.size();
        }

        const uint32_t & operator[](size_t idx) const {
            if(idx<0 || idx>=_c.size()) {
                throw std::runtime_error("access to illegal idx");

            }
            return _c[idx];
        }

        uint32_t & operator[](size_t idx) {
            if(idx<0 || idx>=_c.size()) {
                throw std::runtime_error("access to illegal idx");

            }
            return _c[idx];
        }

    private:
        std::vector<uint32_t> _c;
    };

    inline
    std::ostream & operator<<(std::ostream & os,const AlphaIdentifiersResource & r) {
        os << "[ALPHAIDS](" << r.get_name() << ")::" ;
        for(int i=0;i<r.size();i++) {
            os << (i==0 ? "" : ";") << r[i];
        }
        return os << ")";
    }

    class VersionInfoResource : public Resource {
    private:
        typedef Resource super;
    public:
        VersionInfoResource(const Byte_t * p) : super(p),_v(0),_has_merged_data(false) {
            if(get_id()!=ResourceId::VersionInfo) {
                throw std::runtime_error("Expected VersionInfoId");
            }

            if(get_size()<5)
                throw  std::runtime_error("VersionInfoResource has to have a size of min 5");

            _v = to32(get_data()+0);
            _has_merged_data = *(get_data()+4) != 0x00;

            int s0 = to32(get_data()+5);

            std::cerr << "[" << s0 << "]" << std::endl;

            {
                std::wstring ws;
                for(int i=0;i<s0;i++)
                    ws+=wchar_t(to16(get_data()+9+i*2));
                _reader_name=ws;
            }

            int s1 = to32(get_data()+9+s0*2);
            std::cerr << "[" << s1 << "]" << std::endl;

            {
                std::wstring ws;
                for(int i=0;i<s1;i++)
                    ws+=wchar_t(to16(get_data()+13+s0*2+i*2));
                _writer_name=ws;
            }
            std::cerr << " ****** " << get_size() << std::endl;
        }

        uint32_t get_version() const {
            return _v;
        }

        bool has_merged_data() const {
            return _has_merged_data;
        }

        std::wstring get_reader_name() const {
            return _reader_name;
        }

        std::wstring get_writer_name() const {
            return _writer_name;
        }

    private:
        bool     _has_merged_data;
        uint32_t _v;
        std::wstring _reader_name;
        std::wstring _writer_name;
    };

    inline
    std::ostream & operator<<(std::ostream & os,const VersionInfoResource & r) {
        return os << "[VERSION]" << r.get_version() << "; has_merged_data="
                  << r.has_merged_data()
                  << "; reader = " << PsTiff::Tools::from_wstring(r.get_reader_name())
                  << ";writer =" << PsTiff::Tools::from_wstring(r.get_writer_name());
    }
}

#endif // PSTIFF_RESOURCE_H
