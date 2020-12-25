#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Thu Dec 11 16:48:20 2014
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from grc_gnuradio import wxgui as grc_wxgui
from math import *
from optparse import OptionParser
from s4a import *
import osmosdr
import wx

class top_block(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.tx_power = tx_power = 14000
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1000000
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_rep = pilot_block_rep = 35
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_block_sz = data_block_sz = 1
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        _tx_power_sizer = wx.BoxSizer(wx.VERTICAL)
        self._tx_power_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_tx_power_sizer,
        	value=self.tx_power,
        	callback=self.set_tx_power,
        	label="Transmit Power",
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._tx_power_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_tx_power_sizer,
        	value=self.tx_power,
        	callback=self.set_tx_power,
        	minimum=0,
        	maximum=30000,
        	num_steps=100,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_tx_power_sizer)
        self.s4a_zero_padding_1 = zero_padding((fft_length + cp_length)*(preamble_block_rep + pilot_block_rep + data_block_sz*data_block_rep + silence_block_rep), (fft_length + cp_length)*(preamble_block_rep + pilot_block_rep + data_block_sz*data_block_rep + silence_block_rep) + 128 - ((fft_length + cp_length)*(preamble_block_rep + pilot_block_rep + data_block_sz*data_block_rep + silence_block_rep) % 128))
        	
        self.s4a_serial_to_parallel_1 = serial_to_parallel(fft_length + cp_length)
        	
        self.s4a_serial_to_parallel_0 = serial_to_parallel((occupied_tones*data_block_sz))
        	
        self.s4a_primary_user_rx_controller_0 = primary_user_rx_controller(occupied_tones,
                  ((occupied_tones*data_block_sz*data_block_rep)/8), 0,
                  0, 170, 187,
                  10, 4,
                  1, 10, 1)
        	
        self.s4a_parallel_to_serial_0_0 = parallel_to_serial(fft_length + cp_length)
        	
        self.s4a_parallel_to_serial_0 = parallel_to_serial((occupied_tones*data_block_sz))
        	
        self.s4a_ofdm_sync_0 = ofdm_sync(fft_length, occupied_tones, cp_length,
                          preamble_block_rep, pilot_block_rep,
                          data_block_sz, data_block_rep,
                          silence_block_rep, 1,
                          1, 0)
        	
        self.s4a_ofdm_power_scaling_0 = ofdm_power_scaling(fft_length, 1/9.6571, 1/13.9017,
                          1/48.0, preamble_block_rep, pilot_block_rep,
                          data_block_sz, data_block_rep, silence_block_rep)
          
        self.s4a_ofdm_metrics_controller_0 = ofdm_metrics_controller(occupied_tones*data_block_sz, (occupied_tones))
        	
        self.s4a_ofdm_equalizer_0 = ofdm_equalizer(fft_length,occupied_tones,cp_length,
                               preamble_block_rep, pilot_block_rep,
                               data_block_sz, data_block_rep,
                                silence_block_rep,0)
        	
        self.s4a_frame_generation_0 = frame_generation(fft_length, occupied_tones, preamble_block_rep,
                                pilot_block_rep, data_block_sz,
                                data_block_rep, silence_block_rep)
        	
        self.s4a_cyclic_prefix_removal_0 = cyclic_prefix_removal(fft_length,cp_length)
        	
        self.s4a_cyclic_prefix_insertion_0 = cyclic_prefix_insertion(fft_length,cp_length)
        	
        self.s4a_byte_to_bit_0 = byte_to_bit()
        	
        self.s4a_bpsk_mapper_0 = bpsk_mapper()
        	
        self.s4a_bpsk_demapper_0 = bpsk_demapper()
        	
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_source_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0.set_center_freq(2.422e9, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(2, 0)
        self.osmosdr_source_0.set_iq_balance_mode(2, 0)
        self.osmosdr_source_0.set_gain_mode(True, 0)
        self.osmosdr_source_0.set_gain(6, 0)
        self.osmosdr_source_0.set_if_gain(30, 0)
        self.osmosdr_source_0.set_bb_gain(3, 0)
        self.osmosdr_source_0.set_antenna("", 0)
        self.osmosdr_source_0.set_bandwidth(1.5e6, 0)
          
        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2422e6, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(6, 0)
        self.osmosdr_sink_0.set_if_gain(30, 0)
        self.osmosdr_sink_0.set_bb_gain(3, 0)
        self.osmosdr_sink_0.set_antenna("", 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)
          
        self.fft_vxx_0_0 = fft.fft_vcc(fft_length, False, (), True, 1)
        self.fft_vxx_0 = fft.fft_vcc(fft_length, True, (), True, 1)
        self.dc_blocker_xx_1 = filter.dc_blocker_cc(256, True)
        self.blocks_vector_to_stream_0 = blocks.vector_to_stream(gr.sizeof_char*1, ((occupied_tones*data_block_sz*data_block_rep)/8))
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_char*1, ((occupied_tones*data_block_sz*data_block_rep)/8))
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vcc((25000, ))

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_stream_to_vector_0, 0), (self.s4a_primary_user_rx_controller_0, 0))
        self.connect((self.s4a_primary_user_rx_controller_0, 1), (self.s4a_ofdm_metrics_controller_0, 1))
        self.connect((self.s4a_ofdm_sync_0, 0), (self.s4a_serial_to_parallel_1, 0))
        self.connect((self.s4a_serial_to_parallel_1, 0), (self.s4a_cyclic_prefix_removal_0, 0))
        self.connect((self.s4a_ofdm_equalizer_0, 0), (self.s4a_parallel_to_serial_0, 0))
        self.connect((self.s4a_ofdm_equalizer_0, 1), (self.s4a_primary_user_rx_controller_0, 1))
        self.connect((self.s4a_ofdm_equalizer_0, 2), (self.s4a_primary_user_rx_controller_0, 2))
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.s4a_bpsk_demapper_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.blocks_stream_to_vector_0, 0))
        self.connect((self.s4a_cyclic_prefix_removal_0, 0), (self.fft_vxx_0, 0))
        self.connect((self.fft_vxx_0, 0), (self.s4a_ofdm_equalizer_0, 0))
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))
        self.connect((self.blocks_vector_to_stream_0, 0), (self.s4a_byte_to_bit_0, 0))
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_serial_to_parallel_0, 0))
        self.connect((self.s4a_serial_to_parallel_0, 0), (self.s4a_ofdm_metrics_controller_0, 0))
        self.connect((self.s4a_primary_user_rx_controller_0, 0), (self.blocks_vector_to_stream_0, 0))
        self.connect((self.s4a_frame_generation_0, 0), (self.fft_vxx_0_0, 0))
        self.connect((self.fft_vxx_0_0, 0), (self.s4a_ofdm_power_scaling_0, 0))
        self.connect((self.s4a_ofdm_metrics_controller_0, 0), (self.s4a_frame_generation_0, 0))
        self.connect((self.s4a_ofdm_power_scaling_0, 0), (self.s4a_cyclic_prefix_insertion_0, 0))
        self.connect((self.s4a_zero_padding_1, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.s4a_cyclic_prefix_insertion_0, 0), (self.s4a_parallel_to_serial_0_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.dc_blocker_xx_1, 0))
        self.connect((self.dc_blocker_xx_1, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.s4a_ofdm_sync_0, 0))
        self.connect((self.s4a_zero_padding_1, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.s4a_parallel_to_serial_0_0, 0), (self.s4a_zero_padding_1, 0))



    def get_tx_power(self):
        return self.tx_power

    def set_tx_power(self, tx_power):
        self.tx_power = tx_power
        self._tx_power_slider.set_value(self.tx_power)
        self._tx_power_text_box.set_value(self.tx_power)

    def get_silence_block_rep(self):
        return self.silence_block_rep

    def set_silence_block_rep(self, silence_block_rep):
        self.silence_block_rep = silence_block_rep

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.osmosdr_source_0.set_sample_rate(self.samp_rate)
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

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = top_block()
    tb.Start(True)
    tb.Wait()
