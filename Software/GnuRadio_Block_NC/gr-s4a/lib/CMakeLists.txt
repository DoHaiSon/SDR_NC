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
list(APPEND s4a_sources
    active_stream_selector_impl.cc
    bit_to_byte_impl.cc
    bpsk_demapper_impl.cc
    byte_to_bit_impl.cc
    channel_reciprocity_tester_impl.cc
    coherence_time_counter_generator_impl.cc
    coherence_time_estimator_impl.cc
    console_sink_bb_impl.cc
    console_sink_cc_impl.cc
    csi_converter_impl.cc
    cyclic_prefix_insertion_impl.cc
    cyclic_prefix_removal_impl.cc
    frame_generation_impl.cc
    ofdm_demodulator_impl.cc
    ofdm_equalizer_impl.cc
    ofdm_metrics_controller_impl.cc
    ofdm_modulator_impl.cc
    ofdm_power_scaling_impl.cc
    parallel_to_serial_impl.cc
    primary_user_rx_controller_impl.cc
    qam4_mapper_impl.cc
    random_bit_generator_impl.cc
    secondary_user_tx_controller_impl.cc
    serial_to_parallel_impl.cc
    stream_selector_impl.cc
    vfdm_demodulator_impl.cc
    vfdm_equalizer_impl.cc
    vfdm_metrics_controller_impl.cc
    vfdm_modulator_impl.cc
    vfdm_power_scaling_impl.cc
    vfdm_precoder_impl.cc
    zero_padding_impl.cc
    qam4_demapper_impl.cc
    vfdm_sync_impl.cc
    bpsk_mapper_impl.cc
    ofdm_sync_impl.cc )

add_library(gnuradio-s4a SHARED ${s4a_sources})
target_link_libraries(gnuradio-s4a ${Boost_LIBRARIES} ${LAPACK_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES} ${GNURADIO_BLOCKS_LIBRARIES} ${GNURADIO_FFT_LIBRARIES})
set_target_properties(gnuradio-s4a PROPERTIES DEFINE_SYMBOL "gnuradio_s4a_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-s4a
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_s4a_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_s4a.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_s4a.cc
)

add_executable(test-s4a ${test_s4a_sources})

target_link_libraries(
  test-s4a
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${GNURADIO_BLOCKS_LIBRARIES}
  ${GNURADIO_FFT_LIBRARIES}
  ${Boost_LIBRARIES}
  ${LAPACK_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-s4a
)

GR_ADD_TEST(test_s4a test-s4a)
