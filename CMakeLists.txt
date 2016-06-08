###############################################################################
#
# Require 3.0 for INTERFACE projects

cmake_minimum_required (VERSION 3.0)

###############################################################################
#
# Create INTERFACE for googletest
#

add_library(gtest_interface INTERFACE)
target_include_directories(gtest_interface INTERFACE ${gtest_SOURCE_DIR}/include)
target_link_libraries(gtest_interface INTERFACE gtest_main)

###############################################################################
#
# Create executable 
#

add_executable(blink_test main.cpp iterator_tests.cpp raster_tests.cpp)
target_link_libraries(blink_test PRIVATE gtest_interface)
target_link_libraries(blink_test PRIVATE raster)
target_link_libraries(blink_test PRIVATE iterator)
