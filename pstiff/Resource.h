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

#ifndef PSTIFF_RESOURCE_H
#define PSTIFF_RESOURCE_H

#include "pstiff/ResourceId.h"
#include "pstiff/io/hex_dump.h"
#include "pstiff/tools/strings.h"

#include <vector>
#include <iostream>
#include <algorithm>

#include <memory.h>
#include <string.h>

/** Everything we need to analyse Photoshop specific TIFF resources
    should live in this namespace.
 */

namespace PsTiff
{

    /**
     * @brief The Resource class
       Base class of all blobs of information kept within the
       TIFF Photoshop resource array.

       A sequence starting with '8BIM'<ID><M>string[M]<N>data[N]
       <ID> defines the concrete type of data held in the Resource.
       These types are defined by subclasses of Resource.
     */

    class Resource {
    public:
        typedef ResourceId Id_t;

        /** Create a new empty resource.
         */

        Resource(const std::string & n,ResourceId::Enum_t e) : _pstd(NULL),_pdyn(NULL),_name(n),_size(0),_id(-1) {
            _id=ResourceId::to_range(e).from;
        }

        /** Construct from an existing Photoshop TIFF resource blob.
         */

        Resource(const Byte_t * p) : _pstd(p),_pdyn(NULL),_size(0),_id(0) {
            if(p!=NULL)   {
                if(std::string((char*)p,4)!="8BIM")
                    throw std::runtime_error("expected '8BIM' tag found '"+std::string((char*)p,4)+"'");

                _id   = ((uint16_t) *(p+4) << 8 | *(p+5));
                _name = std::string((char*)p+7,(int)*(p+6));
                p    += _name.length() & 0x1 != 0x0 ? 7 + _name.length() : 8 + _name.length();
                _size = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 | (uint32_t) p[2] << 8 | (uint32_t) p[3] << 0;
            }
        }

        ~Resource() {
            delete _pdyn;
        }


        const Id_t & get_id() const  {
            return _id;
        }

        uint32_t get_size()  const {
            const Byte_t *p = _pstd==NULL ? _pdyn : _pstd;

            if(p!=NULL)
                return 4 + sizeof(uint16_t) + (_name.length() & 0x1 ? _name.length()+1 : _name.length() + 2  ) + sizeof(uint32_t) + ( (get_data_size() & 0x1) == 0 ? get_data_size() : get_data_size()+1);

            return 0;
        }

        const std::string & get_name () const {
            return _name;
        }

        virtual
        const Byte_t *  get_data() const {
            const Byte_t *p = get_raw();
            if( p != NULL) {
                return p+4+sizeof(uint16_t)+(_name.length() & 0x1 ? _name.length()+1 : _name.length() + 2) + sizeof(uint32_t);
            }
            return NULL;
        }

        const Byte_t * get_raw() const {
            return _pstd!=NULL ? _pstd : _pdyn;
        }

        virtual
        uint32_t get_data_size() const {
            return _size;
        }

        /** Generate a string representation of the Resource
            Meant to be overwritten by subclasses. This concrete
            implementation just dumps out the #ID and a hex dump.
         */

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << "#" << get_id() << " '" << get_name() << "'" << std::endl << IO::hex_dump(get_data(),get_data_size());
            return ss.str();
        }

    protected:

        /** Rebuild the binary blob representation of the Resource.
         *  Here we expect the pure type specific data and will add
         *  the '8BIM'<ID><M>name[M] part.
         */

        void rebuild(const Byte_t * p,uint32_t s) {
            _pstd = NULL;
            delete _pdyn;
            int so= s + 4 + sizeof(uint16_t) + (_name.length() & 0x1 ? _name.length()+1 : _name.length()+2+sizeof(uint32_t));

            Byte_t *pp = _pdyn = new Byte_t[so];

            ::memset(pp,0,so);
            ::strncpy((char*)pp,"8BIM",4);

            from16(  pp + 4,get_id().to_range().from);
            fromstr( pp + 6,_name);
            pp   += _name.length() & 0x1 != 0x0 ? 7 + _name.length() : 8 + _name.length();
            from32(  pp, s);

            pp+=sizeof(uint32_t);

            memcpy(pp,p,s);
            _size=s;
        }

    private:
        const Byte_t    * _pstd;    //< Static Data pointer as read by the TIFFLib
        Byte_t          * _pdyn;    //< Dynamic Data pointer for later manipulation
        uint32_t          _size;
        ResourceId        _id;
        std::string       _name;
    };

    inline
    std::ostream & operator<<(std::ostream & os,const Resource & r) {
        return os << r.get_data_size() << "/" << r.get_size() << std::endl << r.to_string();
    }

    /**
     * @brief The SpotColorResource class
     *
     * Defines Spot colors for all additional color
     * channels.
     */

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
            uint32_t id;  // chanle ID
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

            if(get_data_size() != _s * 14 + 4 )
            {
                std::stringstream ss;
                ss << "expected data of SpotColorResources to have size " << _s*14+4
                   << " but found " << get_data_size();
                throw std::runtime_error(ss.str());
            }
            const Byte_t * pp=get_data()+4;

            for(int i=0;i<_s;i++) {
                _ch.push_back(Channel_t(to32(pp),to16(pp+4),
                                        to16(pp+6),to16(pp+8),to16(pp+10),to16(pp+12)));
                pp+=14;
            }

        }

        SpotColorResource() : super("",ResourceId::AlternateSpotColors) {

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

        void push_back(const Channel_t & c) {
            _ch.push_back(c);
            rebuild();
        }

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << "[COLOR]";
            for(int i=0;i<get_count();i++) {
                ss << "[id=" << (*this)[i].id << ";spc=" << (*this)[i].sp << " (";
                for(int j=0;j<4;j++) {
                    ss << (j==0 ? "" : ";") << (*this)[i].v[j];
                }
            }
            return ss.str();
        }
    private:
         void rebuild() {
             int s = sizeof(uint16_t)*2;
             for(std::vector<Channel_t>::const_iterator i=_ch.begin();i!=_ch.end();i++) {
                 s+=sizeof(uint32_t)+sizeof(uint16_t)*5;
             }
             Byte_t * p  = new Byte_t[s];
             ::memset(p,0,s);
             Byte_t * pp = p;
             pp=from16(pp,1);
             pp=from16(pp,get_count());
             for(int i=0;i<get_count();i++) {
                 pp=from32(pp,(*this)[i].id);
                 pp=from16(pp,(*this)[i].sp);
                 pp=from16(pp,(*this)[i].v[0]);
                 pp=from16(pp,(*this)[i].v[1]);
                 pp=from16(pp,(*this)[i].v[2]);
                 pp=from16(pp,(*this)[i].v[3]);

             }
             super::rebuild(p,s);
             delete[] p;
         }

         uint16_t               _v;  // version
         uint16_t               _s;  // size aka # of channels
         std::vector<Channel_t> _ch; // extra channels
    };

    template<class C>
    struct NameTraits;

    template<>
    struct NameTraits<char> {
        static std::string name() {
            return "ALPHANAMES";
        }
        typedef std::basic_string<char> String_t;
        static std::string to_str(const String_t & s) {
            return s;
        }
    };

    template<>
    struct NameTraits<uint16_t> {
        static std::string name () {
            return "UALPHANAMES";
        }
        typedef std::basic_string<wchar_t> String_t;
        static std::string to_str(const String_t & s) {
            return Tools::from_wstring(s);
        }
    };

    /** templated base class for all alpha channel names-
     *
     */

    template<class CI,class CE>
    class BasicAlphaNamesResource : public Resource {
    private:
        typedef Resource super;


    public:
        typedef std::basic_string<CI> String_t;
        typedef CI                    Char_t;
        typedef CE                    Ext_t;
        typedef NameTraits<CE>        Traits_t;

        BasicAlphaNamesResource(const Byte_t * p) : super(p) {
        }

        BasicAlphaNamesResource(const std::string & name, ResourceId::Enum_t e ) : super(name,e) {
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
            rebuild();
        }

        size_t size() const {
            return _c.size();
        }

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << Traits_t::name << " " << size() << " (";
            for(int i=0;i < size();i++) {
                ss << ( i==0 ? "" : ";") << Traits_t::to_str((*this)[i]);
            }
            ss << ")";
            return ss.str();
        }

    protected:
        void rebuild() {

            int s = 0;

            for(typename std::vector<String_t>::const_iterator i=_c.begin();i!=_c.end();i++)
                s += 1 + (*i).length() * sizeof(Ext_t);

            Byte_t *p  = new Byte_t[s];
            Byte_t *p0 = p;

            for(typename std::vector<String_t>::const_iterator i=_c.begin();i!=_c.end();i++) {
                p0 = fromstr(p0,*i);
            }

            super::rebuild(p,s);

            delete[] p;
        }
    private:
        std::vector<String_t> _c;
    };

    /**
     * @brief The AlphaNamesResource class
     *
     * Old style alpha channel names
     */

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
                int s = get_data_size();

                while( (p1-p0) < s && (n=to8(p1)))
                {
                    push_back(String_t((Char_t*)(p1+sizeof(Byte_t)),(int)n));
                    p1+=sizeof(Byte_t)+n;
                }
            }

            if(p1-p0!=get_data_size()) {
                std::stringstream ss;
                ss << "expected " << get_data_size() << " bytes; found " << (p1-p1) << std::endl;
            }

            rebuild();
        }

        AlphaNamesResource() : super("",ResourceId::AlphaNames) {

        }
    };

    /**
     * @brief The UnicodeAlphaNamesResource class
     *
     * 16 bit Unicode Alpha names.
     */

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

            if((p1=p0=get_data()) != NULL) {
                while(( ( p1 + sizeof(uint32_t) - p0 ) < get_data_size()))
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

            if(p1-p0!=get_data_size()) {
                std::stringstream ss;
                ss << "expected " << get_data_size() << " bytes; found " << (p1-p1) << std::endl;
            }

            std::cerr << " > - - - - - - " << get_data_size() << "/" << get_size() << std::endl
                      << IO::hex_dump(get_data(),get_data_size()) << std::endl
                      << " < - - - - - - " << std::endl;

        }
    };

    /**
     * @brief The AlphaIdentifiersResource class
     *
     * Integer Ids of alpha channels.
     */

    class AlphaIdentifiersResource : public Resource {
    private:
        typedef Resource super;
    public:

        AlphaIdentifiersResource(const Byte_t * p) : super(p) {
            if(get_id()!=ResourceId::AlphaIdentifiers)
                throw std::runtime_error("Expected AlphaChannelIds");

            if((get_data_size() % sizeof(uint32_t))!=0)
                throw std::runtime_error("Illegal size of AlphaChannelIdsResource");

            int n = get_data_size() / sizeof(uint32_t);

            for(int i=0;i<n;i++) {

                _c.push_back(to32(get_data()+i*sizeof(uint32_t)));
            }
            rebuild();
        }

        AlphaIdentifiersResource() : super("",ResourceId::AlphaIdentifiers) {

        }

        void push_back(uint32_t n) {
            _c.push_back(n);
            rebuild();
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

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << "[ALPHAIDS](" << get_name() << ")::" ;
            for(int i=0;i<size();i++) {
                ss << (i==0 ? "" : ";") << (*this)[i];
            }
            ss << ")";
            return ss.str();
        }

    private:
        void rebuild() {
            int s=size() * sizeof(uint32_t);
            Byte_t * p  = new Byte_t[s];
            Byte_t * pp = p;
            ::memset(p,0,s);
            for(int i=0;i<size();i++) {
                pp = from32(pp,(*this)[i]);
            }
            super::rebuild(p,s);
            delete[] p;
        }

        std::vector<uint32_t> _c;
    };

    /**
     * @brief The IdSeedNumberResource class
     *
     * Just a simple 16bit number. Not sure what it is good for.
     */

    class IdSeedNumberResource : public Resource {
    private:
        typedef Resource super;
    public:
        IdSeedNumberResource(const Byte_t *p)  : super(p) {
            if(get_id()!=ResourceId::IdSeedNumber)
                throw std::runtime_error("Expected IsSeedNumberId");

            if((get_data_size() != sizeof(uint32_t)))
                throw std::runtime_error("Illegal size of IdSeedNumberResource");
            _seed = to32(get_data());
        }

        IdSeedNumberResource(int id) : super("",ResourceId::IdSeedNumber),_seed(id) {
            rebuild();
        }

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << "[IDSEED](" << get_name() << ")::";
            ss << "id=" << _seed;
            return ss.str();
         }

    private:
        void rebuild() {
            Byte_t b[4];
            from32(b,_seed);
            super::rebuild(b,sizeof(uint32_t));
        }
        int _seed;
    };


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


            {
                std::wstring ws;
                for(int i=0;i<s0;i++)
                    ws+=wchar_t(to16(get_data()+9+i*2));
                _reader_name=ws;
            }

            int s1 = to32(get_data()+9+s0*2);

            {
                std::wstring ws;
                for(int i=0;i<s1;i++)
                    ws+=wchar_t(to16(get_data()+13+s0*2+i*2));
                _writer_name=ws;
            }
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

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << "[VERSION]" << get_version() << "; has_merged_data="
               << has_merged_data()
               << "; reader = " << PsTiff::Tools::from_wstring(get_reader_name())
               << ";writer =" << PsTiff::Tools::from_wstring(get_writer_name());
            return ss.str();
         }

    private:
        bool     _has_merged_data;
        uint32_t _v;
        std::wstring _reader_name;
        std::wstring _writer_name;
    };


    class DisplayInfoResource : public Resource {
    private:
        typedef Resource super;
    public:
        typedef enum {
            CS_RGB       = 0,    /* RGB */
            CS_HSB       = 1,    /* Hue, Saturation, Brightness */
            CS_CMYK      = 2,    /* CMYK */
            CS_PANTONE   = 3,    /* Pantone matching system (Lab)*/
            CS_FOCOLTONE = 4,    /* Focoltone colour system (CMYK)*/
            CS_TRUMATCH  = 5,    /* Trumatch color (CMYK)*/
            CS_TOYO      = 6,    /* Toyo 88 colorfinder 1050 (Lab)*/
            CS_LAB       = 7,    /* L*a*b*/
            CS_GRAYSCALE = 8,    /* Grey scale */
            CS_HKS       = 10,   /* HKS colors (CMYK)*/
            CS_DIC       = 11,   /* DIC color guide (Lab)*/
            CS_ANPA      = 3000, /* Anpa color (Lab)*/
        } ColorSpace_t;

        struct DisplayInfo {
            DisplayInfo()  {
                colorspace = CS_RGB;
                color[0]   = 0;
                color[1]   = 0;
                color[2]   = 0;
                color[3]   = 0;
                opacity    = 100;
                kind       = 2;
                padding    = 0;
            }
            DisplayInfo(ColorSpace_t cs,uint16_t c0,uint16_t c1,uint16_t c2,uint16_t c3,int op=100,int kd=2) {
                colorspace = cs;
                color[0]   = c0;
                color[1]   = c1;
                color[2]   = c2;
                color[3]   = c3;
                opacity    = op;
                kind       = kd;
                padding    = 0;
            }

            uint16_t colorspace; /* Colour space from PSDColorSpace */
            uint16_t color[4];   /* 4 * 16 bit color components */
            uint16_t opacity;    /* Opacity 0 to 100 */
            Byte_t   kind;       /* Selected = 0, Protected = 1, Spot = 2 */
            Byte_t   padding;    /* Padding */
        };

        DisplayInfoResource(const Byte_t * p) : super( p ) {

            if(get_id()!=ResourceId::DisplayInfo) {
                throw std::runtime_error("Expected DisplayInfoId");
            }

            if(this->get_data_size() % sizeof(DisplayInfo) != 0) {
                std::stringstream ss;
                ss << "expected DisplayInfoResource size to be a multitude of " << sizeof(DisplayInfo) << " found "
                   << get_size();
                throw std::runtime_error(ss.str());
            }

            for(const Byte_t *p = get_data();p<get_data()+get_data_size();p+=sizeof(DisplayInfo)) {
                DisplayInfo di;
                di.colorspace = to16(p);

                for(int i=0;i<4;i++) {
                    di.color[i]=to16(p+sizeof(uint16_t)+i*sizeof(uint16_t));
                }

                di.opacity = to16(p + sizeof(uint16_t) + 4 * sizeof(uint16_t) );
                di.kind    = to8( p + sizeof(uint16_t) + 5 * sizeof(uint16_t) );
                di.padding = 0;

                _v.push_back(di);
            }

        }

        DisplayInfoResource() : super("",ResourceId::DisplayInfo) {

        }

        void add(const DisplayInfo & di) {
            _v.push_back(di);
            rebuild();
        }

        size_t size() const {
            return _v.size();
        }

        const DisplayInfo & operator[](int i) const {
            if(i<0 || i>=size()) {
                std::stringstream ss;
                ss << " index " << i << " out of range for size " << size();
                throw std::runtime_error(ss.str());
            }
            return _v[i];
        }

        virtual
        std::string to_string() const {
            std::stringstream ss;
            ss << "[DISPLAYINFO](";
            for(int i=0;i<size();i++) {
                ss << (i==0 ? "" : ";") << "#" << (*this)[i].colorspace << " " << (*this)[i].opacity << "% " << (int)((*this)[i].kind) << " <";
                for(int j=0;j<4;j++) {
                    ss << ((j==0) ? "" : ",") << (*this)[i].color[j];
                }
                ss << ">";
            }
            ss << ")";
            return ss.str();
        }

    private:

        void rebuild() {

            int s = size() * sizeof(DisplayInfo);

            Byte_t * p  = new Byte_t[s];
            Byte_t * pp = p;

            for(std::vector<DisplayInfo>::const_iterator i=_v.begin();i!=_v.end();i++) {

                pp = from16(pp,(*i).colorspace);
                for(int j=0;j<4;j++)
                   pp = from16(pp,(*i).color[j]);
                pp = from16(pp,(*i).opacity);
                pp = from8( pp,(*i).kind);
                pp = from8( pp,(*i).padding);
            }

            super::rebuild(p,s);

            delete[] p;
        }

        std::vector<DisplayInfo> _v;
    };
}

#endif // PSTIFF_RESOURCE_H
