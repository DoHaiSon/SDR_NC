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
    CogNC_delay_packet.xml
    CogNC_flow_filling.xml
    CogNC_primary_rx_control.xml
    CogNC_primary_tx_control.xml
    CogNC_primary_source_control.xml
    CogNC_primary_destination_control.xml
    CogNC_secondary_endnode_control.xml
    CogNC_secondary_relay_control.xml
    CogNC_hybrid_stream_control.xml
    CogNC_vfdm_metrics_control.xml
    CogNC_hybrid_stream_controller.xml
    CogNC_vfdm_header_insertion.xml
    CogNC_vfdm_header_removal.xml
    CogNC_test_block.xml DESTINATION share/gnuradio/grc/blocks
)
