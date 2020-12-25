# Copyright 2011 Free Software Foundation, Inc.
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

install(FILES
    DNC_relay_control.xml
    DNC_end_node_control.xml
    DNC_delay_packet.xml
    DNC_check_received_packet.xml
    DNC_transmitter_control_p2p.xml
    DNC_receiver_control_p2p.xml
    DNC_end_node_controller.xml
    DNC_relay_node_controller.xml
    DNC_terminal_controller.xml
    DNC_relay_controller.xml
    DNC_nodeB_controller.xml
    DNC_nodeR_controller.xml
    DNC_flow_controller_cc.xml
    DNC_nodeA_controller.xml
    DNC_nodeC_controller.xml
    DNC_source1_controller.xml
    DNC_flow_controller_bb.xml
    DNC_packet_arrangement.xml
    DNC_source2_controller.xml
    DNC_relay1_controller.xml
    DNC_relay2_controller.xml
    DNC_destination1_controller.xml
    DNC_destination2_controller.xml
    DNC_MDC_decoding.xml
    DNC_LC_decoding.xml DESTINATION share/gnuradio/grc/blocks
)
