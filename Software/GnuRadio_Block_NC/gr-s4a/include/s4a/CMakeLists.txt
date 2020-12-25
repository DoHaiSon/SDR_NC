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
    active_stream_selector.h
    bit_to_byte.h
    bpsk_demapper.h
    byte_to_bit.h
    channel_reciprocity_tester.h
    coherence_time_counter_generator.h
    coherence_time_estimator.h
    console_sink_bb.h
    console_sink_cc.h
    csi_converter.h
    cyclic_prefix_insertion.h
    cyclic_prefix_removal.h
    frame_generation.h
    ofdm_demodulator.h
    ofdm_equalizer.h
    ofdm_metrics_controller.h
    ofdm_modulator.h
    ofdm_power_scaling.h
    parallel_to_serial.h
    primary_user_rx_controller.h
    qam4_mapper.h
    random_bit_generator.h
    secondary_user_tx_controller.h
    serial_to_parallel.h
    stream_selector.h
    vfdm_demodulator.h
    vfdm_equalizer.h
    vfdm_metrics_controller.h
    vfdm_modulator.h
    vfdm_power_scaling.h
    vfdm_precoder.h
    zero_padding.h
    qam4_demapper.h
    vfdm_sync.h
    bpsk_mapper.h
    ofdm_sync.h DESTINATION include/s4a
)
