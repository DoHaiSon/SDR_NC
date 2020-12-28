#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Mon Dec 28 08:00:19 2020
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

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
from s4a import *
import DNC
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
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1e6
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_rep = pilot_block_rep = 1
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_size = data_size = 2048/2
        self.data_block_sz = data_block_sz = 7
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.s4a_parallel_to_serial_1_0 = parallel_to_serial(data_block_sz*occupied_tones)

        self.s4a_parallel_to_serial_1 = parallel_to_serial(data_block_sz*occupied_tones)

        self.s4a_ofdm_modulator_0 = ofdm_modulator(fft_length, occupied_tones, cp_length,
                               preamble_block_rep, pilot_block_rep,
                               data_block_sz, data_block_rep,
                               silence_block_rep, 1/9.6571,
                               1/13.9017, 1/48.0)

        self.s4a_ofdm_demodulator_0_0 = ofdm_demodulator(fft_length, occupied_tones,
                                cp_length, preamble_block_rep,
                                pilot_block_rep, data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)

        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(fft_length, occupied_tones,
                                cp_length, preamble_block_rep,
                                pilot_block_rep, data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)

        self.s4a_byte_to_bit_0 = byte_to_bit()

        self.s4a_bpsk_mapper_0 = bpsk_mapper()

        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()

        self.s4a_bpsk_demapper_0 = bpsk_demapper()

        self.osmosdr_source_0_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=1' )
        self.osmosdr_source_0_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0_0.set_center_freq(2.415e9, 0)
        self.osmosdr_source_0_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_0_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0_0.set_gain(30, 0)
        self.osmosdr_source_0_0_0.set_if_gain(20, 0)
        self.osmosdr_source_0_0_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_0_0.set_antenna('', 0)
        self.osmosdr_source_0_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_source_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=0' )
        self.osmosdr_source_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0.set_center_freq(2.405e9, 0)
        self.osmosdr_source_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0.set_gain(30, 0)
        self.osmosdr_source_0_0.set_if_gain(20, 0)
        self.osmosdr_source_0_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_0.set_antenna('', 0)
        self.osmosdr_source_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=1' )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.425e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(30, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna('', 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)

        self.dc_blocker_xx_0_0_0 = filter.dc_blocker_cc(256, True)
        self.dc_blocker_xx_0_0 = filter.dc_blocker_cc(256, True)
        self.blocks_null_sink_0_1_0 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_const_vxx_0_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.DNC_nodeR_controller_0 = DNC.nodeR_controller(127, data_size, 6, 10, 20, 30, 40, 1, 1.0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.DNC_nodeR_controller_0, 0), (self.s4a_byte_to_bit_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.s4a_ofdm_demodulator_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0_0_0, 0), (self.s4a_ofdm_demodulator_0_0, 0))
        self.connect((self.dc_blocker_xx_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))
        self.connect((self.dc_blocker_xx_0_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0_0, 0))
        self.connect((self.osmosdr_source_0_0, 0), (self.dc_blocker_xx_0_0, 0))
        self.connect((self.osmosdr_source_0_0_0, 0), (self.dc_blocker_xx_0_0_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.DNC_nodeR_controller_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.DNC_nodeR_controller_0, 1))
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_ofdm_modulator_0, 0))
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_0_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 2), (self.blocks_null_sink_0_0_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 1), (self.blocks_null_sink_0_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 0), (self.s4a_parallel_to_serial_1_0, 0))
        self.connect((self.s4a_ofdm_modulator_0, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))
        self.connect((self.s4a_parallel_to_serial_1_0, 0), (self.s4a_bpsk_demapper_0_0, 0))

    def get_silence_block_rep(self):
        return self.silence_block_rep

    def set_silence_block_rep(self, silence_block_rep):
        self.silence_block_rep = silence_block_rep

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.osmosdr_source_0_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_source_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)

    def get_preamble_block_rep(self):
        return self.preamble_block_rep

    def set_preamble_block_rep(self, preamble_block_rep):
        self.preamble_block_rep = preamble_block_rep

    def get_pilot_block_rep(self):
        return self.pilot_block_rep

    def set_pilot_block_rep(self, pilot_block_rep):
        self.pilot_block_rep = pilot_block_rep

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

    def get_data_block_sz(self):
        return self.data_block_sz

    def set_data_block_sz(self, data_block_sz):
        self.data_block_sz = data_block_sz

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
