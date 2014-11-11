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

#ifndef PSTIFF_IO_HEX_DUMP_H                                               
#define PSTIFF_IO_HEX_DUMP_H
                       
#include <pstiff/io/to_hex.h> 
                                       
namespace PsTiff {                                                                              
    namespace IO  {                          
        template<class C>      
        struct hex_proxy; 
                  
        template<class C> 
        std::ostream & operator<<(std::ostream & os,const hex_proxy<C> & ); 
 
        template<class C> 
        struct hex_proxy  {          
            typedef C char_t;  
            hex_proxy(const C * p, int n)  
                : _p(p), _n(n) { 
            }                                        
                                                     
        private: 
            friend  
            std::ostream & operator<< <C>  (std::ostream & os,const hex_proxy<C> & ); 
 
            const char_t * _p;                                         
            int            _n;           
        };    
                                     
        template<class C> 
        std::ostream & operator<<(std::ostream & os,const hex_proxy<C> & p) {                                                               
            size_t m = 0;                    
            size_t n = p._n; 
            size_t w = sizeof(C) == 1 ? 16 : 8; 
            while(n>0) {                                                                          
                size_t o = n >= w ? w : n; 
                os << to_hex(p._p + m, o ," "); 
  
                os << " " << std::string((w-o)*sizeof(C)*2+(w-o),' '); 
             
                for(size_t i=0;i<o;i++) 
                { 
                    os << (((int)p._p[m+i]<32 || (int)p._p[m+i]>127) ? '.' : (char)p._p[m+i]); 
                }          
                                                                                         
                n -= o; 
                m += o;    
                if(n>0)                 
                {     
                    os << std::endl; 
                } 
            }              
            return os; 
        }                   
                                        
        template<class C> 
        hex_proxy<C> hex_dump(const C * p, size_t n) { 
            return hex_proxy<C>(p,n); 
        }                                                             
          
        template<class C,int N>                            
        hex_proxy<C> hex_dump(const C (&r) [N])  { 
            return hex_proxy<C>( r , N); 
        }                                                                 
          
        template<class C>                                   
        hex_proxy<C> hex_dump(const std::basic_string<C> & r) { 
            return hex_proxy<C>(r.c_str(),r.length()); 
        }                                                                      
          
    }                                                        
}         
 
#endif // PSTIFF_IO_HEX_DUMP_H 
