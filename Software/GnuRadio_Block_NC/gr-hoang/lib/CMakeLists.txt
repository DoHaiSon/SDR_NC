# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND hoang_sources
    qam16_mapper_impl.cc
    qam16_demapper_impl.cc
    psk_mapper_impl.cc
    psk_demapper_impl.cc
    sequence_combination_impl.cc
    sequence_inversion_impl.cc
    sequence_splitting_impl.cc
    signal_gain_impl.cc
    snr_check_impl.cc
    image_text_source_impl.cc
    image_text_sink_impl.cc
    int_to_char_impl.cc
    char_to_int_impl.cc
    image_vector_source_impl.cc
    image_vector_sink_impl.cc
)

add_library(gnuradio-hoang SHARED ${hoang_sources})
target_link_libraries(gnuradio-hoang ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES})
set_target_properties(gnuradio-hoang PROPERTIES DEFINE_SYMBOL "gnuradio_hoang_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-hoang
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_hoang_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_hoang.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_hoang.cc
)

add_executable(test-hoang ${test_hoang_sources})

target_link_libraries(
  test-hoang
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-hoang
)

GR_ADD_TEST(test_hoang test-hoang)
