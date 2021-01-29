#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Fri Jan 29 04:35:16 2021
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
from s4a import *
import CogNC
import osmosdr
import time
import wx


class top_block(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.vfdm_pilot_block_rep = vfdm_pilot_block_rep = 1
        self.vfdm_data_block_sz = vfdm_data_block_sz = 30
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1e6
        self.precoder_length = precoder_length = 16
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_t_sz = pilot_block_t_sz = 16
        self.ofdm_pilot_block_rep = ofdm_pilot_block_rep = 1
        self.ofdm_data_block_sz = ofdm_data_block_sz = 21
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_size = data_size = 54
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.s4a_vfdm_precoder_0 = vfdm_precoder(fft_length, occupied_tones, cp_length,
                              precoder_length,
                              48, preamble_block_rep,
                              pilot_block_t_sz,
                              vfdm_pilot_block_rep,vfdm_data_block_sz, data_block_rep,
                              silence_block_rep)

        self.s4a_vfdm_power_scaling_0 = vfdm_power_scaling(fft_length, precoder_length,
                                   1/9.6571, 1/0.8964,
                                   1/0.8964, preamble_block_rep,
                                   pilot_block_t_sz, vfdm_pilot_block_rep,
                                   vfdm_data_block_sz, data_block_rep,
                                   silence_block_rep)

        self.s4a_serial_to_parallel_2 = serial_to_parallel(occupied_tones)

        self.s4a_serial_to_parallel_0 = serial_to_parallel(precoder_length)

        self.s4a_parallel_to_serial_1_0 = parallel_to_serial((ofdm_data_block_sz*occupied_tones))

        self.s4a_parallel_to_serial_1 = parallel_to_serial(ofdm_data_block_sz*occupied_tones)

        self.s4a_parallel_to_serial_0 = parallel_to_serial((precoder_length + fft_length))

        self.s4a_ofdm_demodulator_0_0 = ofdm_demodulator(fft_length, occupied_tones,
                                cp_length, preamble_block_rep,
                                ofdm_pilot_block_rep, ofdm_data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)

        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(fft_length, occupied_tones,
                                cp_length, preamble_block_rep,
                                ofdm_pilot_block_rep, ofdm_data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)

        self.s4a_byte_to_bit_0 = byte_to_bit()

        self.s4a_bpsk_mapper_0 = bpsk_mapper()

        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()

        self.s4a_bpsk_demapper_0 = bpsk_demapper()

        self.osmosdr_source_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=2a45' )
        self.osmosdr_source_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0.set_center_freq(2.415e9, 0)
        self.osmosdr_source_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0.set_gain(30, 0)
        self.osmosdr_source_0_0.set_if_gain(20, 0)
        self.osmosdr_source_0_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_0.set_antenna('', 0)
        self.osmosdr_source_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=2a45' )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.435e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(30, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna('', 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)

        self.low_pass_filter_0_1 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.dc_blocker_xx_0_0 = filter.dc_blocker_cc(256, True)
        self.blocks_vector_source_x_0_0_0 = blocks.vector_source_c([1.0086-0.7621j,0.9668-0.8128j,0.9781-0.8767j,0.9430-0.9825j,0.8753-1.0319j,0.8574-1.0759j,0.8097-1.1702j,0.7052-1.2219j,0.6291-1.2288j,0.5623-1.2482j,0.4859-1.2443j,0.4425-1.2353j,0.3860-1.2571j,0.2984-1.2465j,0.2472-1.2099j,0.2007-1.2069j,0.1146-1.1829j,0.0580-1.1095j,0.0472-1.0575j,0.0232-1.0348j,-0.0156-1.0027j,-0.0530-0.9659j,-0.1030-0.9230j,-0.1568-0.8458j,-0.1704-0.7367j,-0.1296-0.6523j,-0.0978-0.6237j,-0.1141-0.5839j,-0.1093-0.4980j,-0.0617-0.4358j,-0.0385-0.3992j,-0.0202-0.3280j,0.0423-0.2591j,0.1028-0.2250j,0.1484-0.1781j,0.2238-0.1205j,0.3226-0.0885j,0.4254-0.0821j,0.5222-0.1036j,0.5940-0.1359j,0.6624-0.1534j,0.7517-0.1794j,0.8460-0.2359j,0.9262-0.3290j,0.9524-0.4448j,0.9339-0.5092j,0.9702-0.5351j,1.0335-0.6359j], True, 1, [])
        self.blocks_null_sink_0_1_0 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_0_1_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.analog_sig_source_x_1_0 = analog.sig_source_c(samp_rate, analog.GR_SIN_WAVE, -250e3, 0.5, 0)
        self.analog_sig_source_x_1 = analog.sig_source_c(samp_rate, analog.GR_SIN_WAVE, 250e3, 0.5, 0)
        self.CogNC_secondary_relay_control_0 = CogNC.secondary_relay_control(1, data_size, 50, 0, 6, 120, 121, 122)
        self.CogNC_flow_filling_0 = CogNC.flow_filling(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_flow_filling_0, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.CogNC_secondary_relay_control_0, 0), (self.s4a_byte_to_bit_0, 0))
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_multiply_xx_0_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.blocks_multiply_xx_0_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.low_pass_filter_0_1, 0))
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.blocks_vector_source_x_0_0_0, 0), (self.s4a_serial_to_parallel_2, 0))
        self.connect((self.dc_blocker_xx_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.s4a_ofdm_demodulator_0_0, 0))
        self.connect((self.low_pass_filter_0_1, 0), (self.s4a_ofdm_demodulator_0, 0))
        self.connect((self.osmosdr_source_0_0, 0), (self.dc_blocker_xx_0_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.CogNC_secondary_relay_control_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.CogNC_secondary_relay_control_0, 1))
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_serial_to_parallel_0, 0))
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_0_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 2), (self.blocks_null_sink_0_0_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 1), (self.blocks_null_sink_0_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 0), (self.s4a_parallel_to_serial_1_0, 0))
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.CogNC_flow_filling_0, 0))
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))
        self.connect((self.s4a_parallel_to_serial_1_0, 0), (self.s4a_bpsk_demapper_0_0, 0))
        self.connect((self.s4a_serial_to_parallel_0, 0), (self.s4a_vfdm_precoder_0, 0))
        self.connect((self.s4a_serial_to_parallel_2, 0), (self.s4a_vfdm_precoder_0, 1))
        self.connect((self.s4a_vfdm_power_scaling_0, 0), (self.s4a_parallel_to_serial_0, 0))
        self.connect((self.s4a_vfdm_precoder_0, 0), (self.s4a_vfdm_power_scaling_0, 0))

    def get_vfdm_pilot_block_rep(self):
        return self.vfdm_pilot_block_rep

    def set_vfdm_pilot_block_rep(self, vfdm_pilot_block_rep):
        self.vfdm_pilot_block_rep = vfdm_pilot_block_rep

    def get_vfdm_data_block_sz(self):
        return self.vfdm_data_block_sz

    def set_vfdm_data_block_sz(self, vfdm_data_block_sz):
        self.vfdm_data_block_sz = vfdm_data_block_sz

    def get_silence_block_rep(self):
        return self.silence_block_rep

    def set_silence_block_rep(self, silence_block_rep):
        self.silence_block_rep = silence_block_rep

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.osmosdr_source_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)
        self.low_pass_filter_0_1.set_taps(firdes.low_pass(1, self.samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.analog_sig_source_x_1_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_1.set_sampling_freq(self.samp_rate)

    def get_precoder_length(self):
        return self.precoder_length

    def set_precoder_length(self, precoder_length):
        self.precoder_length = precoder_length

    def get_preamble_block_rep(self):
        return self.preamble_block_rep

    def set_preamble_block_rep(self, preamble_block_rep):
        self.preamble_block_rep = preamble_block_rep

    def get_pilot_block_t_sz(self):
        return self.pilot_block_t_sz

    def set_pilot_block_t_sz(self, pilot_block_t_sz):
        self.pilot_block_t_sz = pilot_block_t_sz

    def get_ofdm_pilot_block_rep(self):
        return self.ofdm_pilot_block_rep

    def set_ofdm_pilot_block_rep(self, ofdm_pilot_block_rep):
        self.ofdm_pilot_block_rep = ofdm_pilot_block_rep

    def get_ofdm_data_block_sz(self):
        return self.ofdm_data_block_sz

    def set_ofdm_data_block_sz(self, ofdm_data_block_sz):
        self.ofdm_data_block_sz = ofdm_data_block_sz

    def get_occupied_tones(self):
        return self.occupied_tones

    def set_occupied_tones(self, occupied_tones):
        self.occupied_tones = occupied_tones

    def get_fft_length(self):
        return self.fft_length

    def set_fft_length(self, fft_length):
        self.fft_length = fft_length

    def get_data_size(self):
        return self.data_size

    def set_data_size(self, data_size):
        self.data_size = data_size

    def get_data_block_rep(self):
        return self.data_block_rep

    def set_data_block_rep(self, data_block_rep):
        self.data_block_rep = data_block_rep

    def get_cp_length(self):
        return self.cp_length

    def set_cp_length(self, cp_length):
        self.cp_length = cp_length


def main(top_block_cls=top_block, options=None):

    tb = top_block_cls()
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
