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

list(APPEND DNC_sources
    relay_control_impl.cc
    end_node_control_impl.cc
    delay_packet_impl.cc
    check_received_packet_impl.cc
    transmitter_control_p2p_impl.cc
    receiver_control_p2p_impl.cc
    end_node_controller_impl.cc
    relay_node_controller_impl.cc
    terminal_controller_impl.cc
    relay_controller_impl.cc
    nodeB_controller_impl.cc
    nodeR_controller_impl.cc
    flow_controller_cc_impl.cc
    nodeA_controller_impl.cc
    nodeC_controller_impl.cc
    source1_controller_impl.cc
    flow_controller_bb_impl.cc
    packet_arrangement_impl.cc
    source2_controller_impl.cc
    relay1_controller_impl.cc
    relay2_controller_impl.cc
    destination1_controller_impl.cc
    destination2_controller_impl.cc
    MDC_decoding_impl.cc
    LC_decoding_impl.cc
)

set(DNC_sources "${DNC_sources}" PARENT_SCOPE)
if(NOT DNC_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT DNC_sources)

add_library(gnuradio-DNC SHARED ${DNC_sources})
target_link_libraries(gnuradio-DNC ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-DNC PROPERTIES DEFINE_SYMBOL "gnuradio_DNC_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-DNC PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-DNC
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_DNC_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_DNC.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_DNC.cc
)

add_executable(test-DNC ${test_DNC_sources})

target_link_libraries(
  test-DNC
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-DNC
)

GR_ADD_TEST(test_DNC test-DNC)
