//========================================================================
//
// pstiff_image_resource_id.cpp
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

#include "pstiff.h"
#include <sstream>

namespace PsTiff
{
    const ImageResourceId::Map_t & ImageResourceId::GetMap()
    {
        static const struct Node_t _array[]={
            // (Obsolete--Photoshop 2.0 only ) Contains five 2-byte values: number of channels, rows, columns, depth, and mode
            {Unsupported,{1000,1000}},
            // Macintosh print manager print info record
            {Unsupported,{ 1001, 1001}},
            // (Obsolete--Photoshop 2.0 only ) Indexed color table
            {Unsupported,{ 1003, 1003}},
            // ResolutionInfo structure. See Appendix A in Photoshop API Guide.pdf.
            {ResolutionInfo,{ 1005, 1005}},
            // Names of the alpha channels as a series of Pascal strings.
            {AlphaNames,{ 1006, 1006}},
            // (Obsolete) See ID 1077 DisplayInfo structure. See Appendix A in Photoshop API Guide.pdf.
            {Unsupported,{ 1007, 1007}},
            // The caption as a Pascal string.
            {Unsupported,{ 1008, 1008}},
            // Border information. Contains a fixed number (2 bytes real, 2 bytes fraction) for the border width,
            // {and 2 bytes for border units (1 = inches, 2 = cm, 3 = points, 4 = picas, 5 = columns)}.
            {Unsupported,{ 1009, 1009}},
            //{Background color. See See Color structure}.
            {Unsupported,{ 1010, 1010}},
            // Print flags. A series of one-byte boolean values (see Page Setup dialog):
            // labels, crop marks, color bars, registration marks, negative, flip, interpolate, caption, print flags.
            {PageSetupBools,{ 1011, 1011}},
            //Grayscale and multichannel halftoning information
            {Unsupported,{ 1012, 1012}},
            //Color halftoning information
            {HalftoneInformation,{ 1013, 1013}},
            //Duotone halftoning information
            {Unsupported,{ 1014, 1014}},
            //Grayscale and multichannel transfer function
            {Unsupported,{ 1015, 1015}},
            //Color transfer functions
            {ColorTransferFunctions,{ 1016, 1016}},
            //Duotone transfer functions
            {Unsupported,{ 1017, 1017}},
            //Duotone image information
            {Unsupported,{ 1018, 1018}},
            //Two bytes for the effective black and white values for the dot range
            {Unsupported,{ 1019, 1019}},
            // (Obsolete)
            {Unsupported,{ 1020, 1020}},
            //EPS options
            {Unsupported,{ 1021, 1021}},
            // Quick Mask information. 2 bytes containing Quick Mask channel ID; 1- byte boolean indicating whether the mask was initially empty.
            {Unsupported,{ 1022, 1022}},
            // (Obsolete)
            {Unsupported,{ 1023, 1023}},
            // Layer state information. 2 bytes containing the index of target layer (0 = bottom layer).
            {LayerStateInformation,{ 1024, 1024}},
            // Working path (not saved). See See Path resource format.
            {Unsupported,{ 1025, 1025}},
            //Layers group information. 2 bytes per layer containing a group ID for the dragging groups. Layers in a group have the same group ID.
            {LayersGroupInformation,{ 1026, 1026}},
            //(Obsolete)
            {Unsupported,{ 1027, 1027}},
            //IPTC-NAA record. Contains the File Info... information. See the documentation in the IPTC folder of the Documentation folder.
            {Iptc,{ 1028, 1028}},
            //Image mode for raw format files
            {Unsupported,{ 1029, 1029}},
            //JPEG quality. Private.
            {Unsupported,{ 1030, 1030}},
            //(Photoshop 4.0) Grid and guides information. See See Grid and guides resource format.
            {GridAndGuideInformation,{ 1032, 1032}},
            // (Photoshop 4.0) Thumbnail resource for Photoshop 4.0 only. See See Thumbnail resource format.
            {Unsupported,{ 1033, 1033}},
            // (Photoshop 4.0) Copyright flag. Boolean indicating whether image is copyrighted. 
            // Can be set via Property suite or by user in File Info...
            {CopyrightFlag,{ 1034, 1034}},
            // (Photoshop 4.0) URL. Handle of a text string with uniform resource locator. 
            /// Can be set via Property suite or by user in File Info...
            {Unsupported,{ 1035, 1035}},
            // (Photoshop 5.0) Thumbnail resource (supersedes resource 1033). See See Thumbnail resource format.
            {ThumbnailResource,{ 1036, 1036}},
            // (Photoshop 5.0) Global Angle. 4 bytes that contain an integer between 0 and 359, which is the global lighting angle 
            // for effects layer. If not present, assumed to be 30.
            {GlobaleAngle,{ 1037, 1037}},
            // (Obsolete) See ID 1073 below. (Photoshop 5.0) Color samplers resource. See See Color samplers resource format.
            {Unsupported,{ 1038, 1038}},
            // (Photoshop 5.0) ICC Profile. The raw bytes of an ICC (International Color Consortium) format profile.
            // See ICC1v42_2006-05.pdf in the Documentation folder and icProfileHeader.h in Sample Code\Common\Includes .
            {Unsupported,{ 1039, 1039}},
            // (Photoshop 5.0) Watermark. One byte.
            {Unsupported,{ 1040, 1040}},
            // (Photoshop 5.0) ICC Untagged Profile.
            // 1 byte that disables any assumed profile handling when opening the file.
            // 1 = intentionally untagged.
            {IccUntaggedProfile,{ 1041, 1041}},
            // (Photoshop 5.0) Effects visible. 1-byte global flag to show/hide all the effects layer. Only present when they are hidden.
            {Unsupported,{ 1042, 1042}},
            // (Photoshop 5.0) Spot Halftone.
            // 4 bytes for version, 4 bytes for length, and the variable length data.
            {SpotHalftone,{ 1043, 1043}},
            // (Photoshop 5.0) Document-specific IDs seed number. 
            // 4 bytes: Base value, starting at which layer IDs will be generated (or a greater value if existing IDs already exceed it).
            // Its purpose is to avoid the case where we add layers, flatten, save, open, and then add more layers that end up with the same 
            // IDs as the first set.
            {IdSeedNumber,{ 1044, 1044}},
            // (Photoshop 5.0) Unicode Alpha Names. Unicode string
            {UnicodeAlphaNames,{ 1045, 1045}},
            // (Photoshop 6.0) Indexed Color Table Count. 2 bytes for the number of colors in table that are actually defined
            {Unsupported,{ 1046, 1046}},
            // (Photoshop 6.0) Transparency Index. 2 bytes for the index of transparent color, if any.
            {Unsupported,{ 1047, 1047}},
            // (Photoshop 6.0) Global Altitude. 4 byte entry for altitude
            {GlobalAltitude,{ 1049, 1049}},
            // (Photoshop 6.0) Slices. See See Slices resource format.
            {Slices,{ 1050, 1050}},
            // (Photoshop 6.0) Workflow URL. Unicode string
            {Unsupported,{ 1051, 1051}},
            // (Photoshop 6.0) Jump To XPEP. 2 bytes major version, 2 bytes minor version, 
            // 4 bytes count. Following is repeated for count: 4 bytes block size, 4 bytes key, if key = 'jtDd' , 
            // then next is a Boolean for the dirty flag; otherwise it's a 4 byte entry for the mod date.
            {Unsupported,{ 1052, 1052}},
            //(Photoshop 6.0) Alpha Identifiers. 4 bytes of length, followed by 4 bytes each for every alpha identifier.
            {AlphaIndentifiers,{ 1053, 1053}},
            //(Photoshop 6.0) URL List. 4 byte count of URLs,
            // followed by 4 byte long, 4 byte ID, and Unicode string for each count.
            {UrlList,{ 1054, 1054}},
            //(Photoshop 6.0) Version Info. 4 bytes version, 1 byte hasRealMergedData , 
            // Unicode string: writer name, Unicode string: reader name, 4 bytes file version.
            {VersionInfo,{ 1057, 1057}},
            //(Photoshop 7.0) EXIF data 1. See http://www.kodak.com/global/plugins/acrobat/en/service/digCam/exifStandard2.pdf
            {Unsupported,{ 1058, 1058}},
            //(Photoshop 7.0) EXIF data 3. See http://www.kodak.com/global/plugins/acrobat/en/service/digCam/exifStandard2.pdf
            {Unsupported,{ 1059, 1059}},
            //(Photoshop 7.0) XMP metadata. File info as XML description. See http://www.adobe.com/devnet/xmp/
            {Unsupported,{ 1060, 1060}},
            //(Photoshop 7.0) Caption digest. 16 bytes: RSA Data Security, MD5 message-digest algorithm
            {CaptionDigest,{ 1061, 1061}},
            //(Photoshop 7.0) Print scale. 2 bytes style (0 = centered, 1 = size to fit, 2 = user defined).
            // 4 bytes x location (floating point). 4 bytes y location (floating point). 4 bytes scale (floating point)
            {PrintScale,{ 1062, 1062}},
            //(Photoshop CS) Pixel Aspect Ratio. 4 bytes (version = 1 or 2), 8 bytes double, x / y of a pixel. 
            // Version 2, attempting to correct values for NTSC and PAL, previously off by a factor of approx. 5%.
            {PixelAspectRatio,{ 1064, 1064}},
            //(Photoshop CS) Layer Comps. 4 bytes (descriptor version = 16), Descriptor (see See Descriptor structure)
            {Unsupported,{ 1065, 1065}},
            //(Photoshop CS) Alternate Duotone Colors.
            // 2 bytes (version = 1), 2 bytes count, following is repeated for each count:
            // [ Color: 2 bytes for space followed by 4 * 2 byte color component ],
            // following this is another 2 byte count, usually 256, followed by Lab colors one byte each for L, a, b.
            // This resource is not read or used by Photoshop.
            {Unsupported,{ 1066, 1066}},
            //(Photoshop CS)Alternate Spot Colors.
            // 2 bytes (version = 1), 2 bytes channel count, following is repeated for each count:
            // 4 bytes channel ID, Color: 2 bytes for space followed by 4 * 2 byte color component.
            //This resource is not read or used by Photoshop.
            {AlternateSpotColors,{ 1067, 1067}},
            // (Photoshop CS2) Layer Selection ID(s).
            // 2 bytes count, following is repeated for each count: 4 bytes layer ID
            {LayerSelectionIds,{ 1069, 1069}},
            //(Photoshop CS2) HDR Toning information
            {Unsupported,{ 1070, 1070}},
            //(Photoshop CS2) Print info
            {Unsupported,{ 1071, 1071}},
            //(Photoshop CS2) Layer Group(s) Enabled ID.
            // 1 byte for each layer in the document, repeated by length of the resource.
            // NOTE: Layer groups have start and end markers
            {LayerGroupsEnabled,{ 1072, 1072}},
            //(Photoshop CS3) Color samplers resource. Also see ID 1038 for old format. See See Color samplers resource format.
            {Unsupported,{ 1073, 1073}},
            //(Photoshop CS3) Measurement Scale. 4 bytes (descriptor version = 16), Descriptor (see See Descriptor structure)
            {Unsupported,{ 1074, 1074}},
            //(Photoshop CS3) Timeline Information. 4 bytes (descriptor version = 16), Descriptor (see See Descriptor structure)
            {Unsupported,{ 1075, 1075}},
            //(Photoshop CS3) Sheet Disclosure. 4 bytes (descriptor version = 16), Descriptor (see See Descriptor structure)
            {Unsupported,{ 1076, 1076}},
            //(Photoshop CS3) DisplayInfo structure to support floating point clors. Also see ID 1007. See Appendix A in Photoshop API Guide.pdf .
            {Unsupported,{ 1077, 1077}},
            // (Photoshop CS3) Onion Skins. 4 bytes (descriptor version = 16), Descriptor (see See Descriptor structure)
            {Unsupported,{ 1078, 1078}},
            // (Photoshop CS4) Count Information. 4 bytes (descriptor version = 16), 
            // Descriptor (see See Descriptor structure) Information about the count in the document. See the Count Tool.
            {Unsupported,{ 1080, 1080}},
            // (Photoshop CS5) Print Information. 4 bytes (descriptor version = 16), 
            // Descriptor (see See Descriptor structure) Information about the current print settings in the document. 
            // The color management options.
            {PrintInformation,{ 1082, 1082}},
            // (Photoshop CS5) Print Style. 4 bytes (descriptor version = 16), Descriptor (see See Descriptor structure) 
            // Information about the current print style in the document. The printing marks, labels, ornaments, etc.
            {PrintStyle,{ 1083, 1083}},
            // (Photoshop CS5) Macintosh NSPrintInfo. Variable OS specific info for Macintosh. NSPrintInfo. 
            // It is recommened that you do not interpret or use this data.
            {Unsupported,{ 1084, 1084}},
            // (Photoshop CS5) Windows DEVMODE. Variable OS specific info for Windows. DEVMODE. 
            // It is recommened that you do not interpret or use this data.
            {Unsupported,{ 1085, 1085}},
            // (Photoshop CS6) Auto Save File Path. Unicode string. It is recommened that you do not interpret or use this data.
            {Unsupported,{ 1086, 1086}},
            // (Photoshop CS6) Auto Save Format. Unicode string. It is recommened that you do not interpret or use this data.
            {Unsupported,{ 1087, 1087}},
            // (Photoshop CC) Path Selection State. 4 bytes (descriptor version = 16), 
            // Descriptor (see See Descriptor structure) Information about the current path selection state.
            {Unsupported,{ 1088, 1088}},
            // Path Information (saved paths). See See Path resource format.
            {PathInformation,{2000,2997}},
            // Name of clipping path. See See Path resource format.
            {Unsupported,{ 2999, 2999}},
            // (Photoshop CC) Origin Path Info. 4 bytes (descriptor version = 16), 
            // Descriptor (see See Descriptor structure) Information about the origin path data.
            {Unsupported,{ 3000, 3000}},
            //Plug-In resource(s). Resources added by a plug-in. See the plug-in API found in the SDK documentation
            {PluginResources,{  4000,4999, }},
            //Image Ready variables. XML representation of variables definition
            {Unsupported,{  7000,  7000}},
            //Image Ready data sets
            {Unsupported,{  7001,  7001}},
            // (Photoshop CS3) Lightroom workflow, if present the document is in the middle of a Lightroom workflow.
            {Unsupported,{  8000,  8000}},
            // Print flags information. 2 bytes version ( = 1),
            // 1 byte center crop marks, 1 byte ( = 0), 4 bytes bleed width value, 2 bytes bleed width scale
            {PrintFlags,{ 10000, 10000}}
        }; // Map

        static Map_t m;
        if( m.empty() )
            BuildMap(_array,m);
        return m;
    }

    const ImageResourceId::Names_t & ImageResourceId::GetNames()
    {
        static Names_t n;
        static const NameNode_t a[] ={
            {Unsupported,"Unsupported"},
            {ResolutionInfo,"ResolutionInfo"},
            {AlphaNames,"AlphaNames"},
            {PageSetupBools,"PageSetupBools"},
            {HalftoneInformation,"HalftoneInformation"},
            {ColorTransferFunctions,"ColorTransferFunctions"},
            {LayerStateInformation,"LayerStateInformation"},
            {LayersGroupInformation,"LayersGroupInformation"},
            {GridAndGuideInformation,"GridAndGuideInformation"},
            {CopyrightFlag,"CopyrightFlag"},
            {ThumbnailResource,"ThumbnailResource"},
            {GlobaleAngle,"GlobaleAngle"},
            {IccUntaggedProfile,"IccUntaggedProfile"},
            {SpotHalftone,"SpotHalftone"},
            {IdSeedNumber,"IdSeedNumber"},
            {UnicodeAlphaNames,"UnicodeAlphaNames"},
            {GlobalAltitude,"GlobalAltitude"},
            {Slices,"Slices"},
            {AlphaIndentifiers,"AlphaIndentifiers"},
            {UrlList,"UrlList"},
            {VersionInfo,"VersionInfo"},
            {PrintScale,"PrintScale"},
            {PixelAspectRatio,"PixelAspectRatio"},
            {AlternateSpotColors,"AlternateSpotColors"},
            {LayerSelectionIds,"LayerSelectionIds"},
            {LayerGroupsEnabled,"LayerGroupsEnabled"},
            {PluginResources,"PluginResources"},
            {PrintFlags,"PrintFlags"},
            {Iptc,"Iptc"},
            {CaptionDigest,"CaptionDiges"},
            {PathInformation,"PathInformation"},
            {PrintInformation,"PrintInformation"},
            {PrintStyle,"PrintStyle"}
        };

        if(n.empty()) {
            BuildNames(a,n);
        }
        return n;
    }
 
    std::string ImageResourceId::ToString() const {
        std::stringstream ss;
        Names_t::const_iterator i=GetNames().find(_e);
        ss << "(N=" << _n << "::E=" << (int)_e << ")'";
        if(i==GetNames().end())
            ss << "Unknown";
        else
            ss << (*i).second;
        ss <<"'";
        return ss.str();
    }
}
