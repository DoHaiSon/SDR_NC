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
# Install public header files
########################################################################
install(FILES
    api.h
    relay_control.h
    end_node_control.h
    delay_packet.h
    check_received_packet.h
    transmitter_control_p2p.h
    receiver_control_p2p.h
    end_node_controller.h
    relay_node_controller.h
    terminal_controller.h
    relay_controller.h
    nodeB_controller.h
    nodeR_controller.h
    flow_controller_cc.h
    nodeA_controller.h
    nodeC_controller.h
    source1_controller.h
    flow_controller_bb.h
    packet_arrangement.h
    source2_controller.h
    relay1_controller.h
    relay2_controller.h
    destination1_controller.h
    destination2_controller.h
    MDC_decoding.h
    LC_decoding.h DESTINATION include/DNC
)
