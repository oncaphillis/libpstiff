project(pstiff)

cmake_minimum_required(VERSION 2.8)

add_executable(pstiff_tool
 pstiff_tool.cpp
)

INCLUDE_DIRECTORIES( "." )

# libpstiff

set(pstiff_SRCS
  PsTiffResource.cpp
  pstiff/Types.h
  pstiff/Resource.h
  pstiff/ResourceId.h
  pstiff/ResourceList.h
  pstiff/io/to_hex.h
  pstiff/io/hex_dump.h
)

add_library(pstiff SHARED ${pstiff_SRCS})

# pstiff_tool

target_link_libraries(pstiff tiff)
target_link_libraries(pstiff_tool pstiff)
