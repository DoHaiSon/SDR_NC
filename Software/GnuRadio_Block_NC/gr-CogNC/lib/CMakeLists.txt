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

list(APPEND CogNC_sources
    delay_packet_impl.cc
    flow_filling_impl.cc
    primary_rx_control_impl.cc
    primary_tx_control_impl.cc
    primary_source_control_impl.cc
    primary_destination_control_impl.cc
    secondary_endnode_control_impl.cc
    secondary_relay_control_impl.cc
    hybrid_stream_control_impl.cc
    vfdm_metrics_control_impl.cc
    hybrid_stream_controller_impl.cc
    vfdm_header_insertion_impl.cc
    vfdm_header_removal_impl.cc
    test_block_impl.cc
    vfdm_adaptive_precoder_impl.cc
    radom_complex_generator_impl.cc
)

set(CogNC_sources "${CogNC_sources}" PARENT_SCOPE)
if(NOT CogNC_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT CogNC_sources)

add_library(gnuradio-CogNC SHARED ${CogNC_sources})
target_link_libraries(gnuradio-CogNC ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-CogNC PROPERTIES DEFINE_SYMBOL "gnuradio_CogNC_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-CogNC PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-CogNC
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_CogNC_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_CogNC.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_CogNC.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_vfdm_header_removal.cc
)

add_executable(test-CogNC ${test_CogNC_sources})

target_link_libraries(
  test-CogNC
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-CogNC
)

GR_ADD_TEST(test_CogNC test-CogNC)
