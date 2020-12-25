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
    s4a_active_stream_selector.xml
    s4a_bit_to_byte.xml
    s4a_bpsk_demapper.xml
    s4a_byte_to_bit.xml
    s4a_channel_reciprocity_tester.xml
    s4a_coherence_time_counter_generator.xml
    s4a_coherence_time_estimator.xml
    s4a_console_sink_xx.xml
    s4a_csi_converter.xml
    s4a_cyclic_prefix_insertion.xml
    s4a_cyclic_prefix_removal.xml
    s4a_frame_generation.xml
    s4a_ofdm_demodulator.xml
    s4a_ofdm_equalizer.xml
    s4a_ofdm_metrics_controller.xml
    s4a_ofdm_modulator.xml
    s4a_ofdm_power_scaling.xml
    s4a_parallel_to_serial.xml
    s4a_primary_user_rx_controller.xml
    s4a_qam4_mapper.xml
    s4a_qam4_demapper.xml
    s4a_random_bit_generator.xml
    s4a_secondary_user_tx_controller.xml
    s4a_serial_to_parallel.xml
    s4a_stream_selector.xml
    s4a_vfdm_demodulator.xml
    s4a_vfdm_equalizer.xml
    s4a_vfdm_metrics_controller.xml
    s4a_vfdm_modulator.xml
    s4a_vfdm_power_scaling.xml
    s4a_vfdm_precoder.xml
    s4a_vfdm_sync.xml
    s4a_zero_padding.xml
    s4a_bpsk_mapper.xml
    s4a_ofdm_sync.xml DESTINATION share/gnuradio/grc/blocks
)
