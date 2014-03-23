//========================================================================
//
// pstiff.h
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
#ifndef PSTIFF_H
#define PSTIFF_H

#include <map>
#include <string>
#include <sstream>
#include <stdexcept>

namespace PsTiff
{
    class ImageResourceId {
    public:
        enum Enum_t {
            Unsupported,
            Unknown,
            ResolutionInfo,
            AlphaNames,
            PageSetupBools,
            HalftoneInformation,
            ColorTransferFunctions,
            LayerStateInformation,
            LayersGroupInformation,
            GridAndGuideInformation,
            CopyrightFlag,
            ThumbnailResource,
            GlobaleAngle,
            IccUntaggedProfile,
            SpotHalftone,
            IdSeedNumber,
            UnicodeAlphaNames,
            GlobalAltitude,
            Slices,
            AlphaIndentifiers,
            UrlList,
            VersionInfo,
            PrintScale,
            PixelAspectRatio,
            AlternateSpotColors,
            LayerSelectionIds,
            LayerGroupsEnabled,
            PluginResources,
            PrintFlags,
            Iptc,
            CaptionDigest,
            PathInformation
        };
        struct Range_t {
            int from;
            int to;
            bool operator<(const Range_t &r) const {
                if(from>to || r.from>r.to)
                {
                    std::stringstream ss;
                    ss << " illegal range pair [" << from << ";" << to << "] and [" << r.from << ";" << r.to << "]";
                    throw std::runtime_error(ss.str());
                }
                return to<r.from;
            }
        };
    private:

        struct Node_t {
            Enum_t  _e;
            Range_t _r;
        };

        struct NameNode_t {
            Enum_t e;
            const char *name;
        };

        typedef std::map<Range_t,Enum_t>      Map_t;
        typedef std::map<Enum_t,std::string> Names_t;
 
        template<class NODE,int N>
        static void BuildNames(const NODE (&a)[N], Names_t & n)
        {
            n.clear();
            for(int i=0;i<N;i++) {
                n[ a[i].e ] = a[i].name;
            }
        }

        template<int N>
        static void BuildMap( const Node_t (&a)[N] , Map_t &m)
        {
            m.clear();
            for(int i=0;i<N;i++) {
                if(a[i]._r.from>a[i]._r.to)
                {
                    std::stringstream ss;
                    ss << "illegal ImageResourceId range " << a[i]._r.from << "-" << a[i]._r.to;
                    throw std::runtime_error(ss.str());
                }
                m[ a[i]._r ] = a[i]._e;
            }
        }

        static const Map_t   & GetMap();
        static const Names_t & GetNames();

    public:
        ImageResourceId(int n)
            : _n(n) ,
              _e(ToEnum(n)) {
        }

        static Enum_t ToEnum(int n) {
            Range_t r = {
                n,n
            };

            Map_t::const_iterator i=GetMap().find(r);
            if(i==GetMap().end())
            {
                return Unknown;
            }
            return (*i).second;
        }

        std::string ToString() const;

    private:
      int    _n;
      Enum_t _e;
    };
}

#endif // PSTIFF_H
