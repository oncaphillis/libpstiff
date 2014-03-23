project(pstiff)
cmake_minimum_required(VERSION 2.8)

add_executable(pstiff_tool
 pstiff_tool.cpp
)

set(pstiff_SRCS 
  pstiff_image_resource.cpp
  pstiff_image_resource_id.cpp
  pstiff.h
)

add_library(pstiff SHARED ${pstiff_SRCS})

target_link_libraries(pstiff tiff)
target_link_libraries(pstiff_tool pstiff)
