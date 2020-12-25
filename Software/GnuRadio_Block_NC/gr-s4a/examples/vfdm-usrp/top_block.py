#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Fri Dec 12 16:02:14 2014
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import filter
from gnuradio import gr
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.wxgui import fftsink2
from grc_gnuradio import wxgui as grc_wxgui
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
        self.usrp_max = usrp_max = 32768
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1e6
        self.precoder_length = precoder_length = 16
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_t_sz = pilot_block_t_sz = 20
        self.pilot_block_rep = pilot_block_rep = 1
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_block_sz = data_block_sz = 3
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.wxgui_fftsink2_0 = fftsink2.fft_sink_c(
        	self.GetWin(),
        	baseband_freq=0,
        	y_per_div=10,
        	y_divs=10,
        	ref_level=0,
        	ref_scale=2.0,
        	sample_rate=samp_rate,
        	fft_size=1024,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title="FFT Plot",
        	peak_hold=False,
        )
        self.Add(self.wxgui_fftsink2_0.win)
        self.s4a_vfdm_sync_0 = vfdm_sync(fft_length, occupied_tones, cp_length, 
                          precoder_length, preamble_block_rep, 
                          pilot_block_t_sz, pilot_block_rep, 
                          data_block_sz, data_block_rep,
                          silence_block_rep, 
                          1, 
                          0)
        	
        self.s4a_vfdm_equalizer_0 = vfdm_equalizer((fft_length+cp_length),occupied_tones, cp_length,
                              cp_length, preamble_block_rep,
                              pilot_block_t_sz, pilot_block_rep,
                              data_block_sz, data_block_rep,
                              silence_block_rep, 0)
        	
        self.s4a_serial_to_parallel_1 = serial_to_parallel(fft_length + cp_length)
        	
        self.s4a_parallel_to_serial_1 = parallel_to_serial((data_block_sz*precoder_length))
        	
        self.s4a_console_sink_xx_0 = console_sink_bb("rx_bits",
                                         6,1)
        	
        self.s4a_bpsk_demapper_0 = bpsk_demapper()
        	
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_source_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0.set_center_freq(2.415e9, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(2, 0)
        self.osmosdr_source_0.set_iq_balance_mode(2, 0)
        self.osmosdr_source_0.set_gain_mode(True, 0)
        self.osmosdr_source_0.set_gain(6, 0)
        self.osmosdr_source_0.set_if_gain(30, 0)
        self.osmosdr_source_0.set_bb_gain(3, 0)
        self.osmosdr_source_0.set_antenna("", 0)
        self.osmosdr_source_0.set_bandwidth(1.5e6, 0)
          
        self.fft_vxx_0 = fft.fft_vcc((fft_length + cp_length), True, (), True, 1)
        self.dc_blocker_xx_1 = filter.dc_blocker_cc(256, True)
        self.blocks_null_sink_0_1_0 = blocks.null_sink(gr.sizeof_char*1)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*precoder_length*(fft_length+cp_length))
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((25000, ))

        ##################################################
        # Connections
        ##################################################
        self.connect((self.s4a_vfdm_equalizer_0, 0), (self.s4a_parallel_to_serial_1, 0))
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))
        self.connect((self.dc_blocker_xx_1, 0), (self.wxgui_fftsink2_0, 0))
        self.connect((self.fft_vxx_0, 0), (self.s4a_vfdm_equalizer_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.s4a_vfdm_sync_0, 0))
        self.connect((self.s4a_vfdm_equalizer_0, 1), (self.blocks_null_sink_0_1, 0))
        self.connect((self.dc_blocker_xx_1, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.blocks_null_sink_0_1_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.s4a_console_sink_xx_0, 0))
        self.connect((self.s4a_vfdm_sync_0, 0), (self.s4a_serial_to_parallel_1, 0))
        self.connect((self.s4a_serial_to_parallel_1, 0), (self.fft_vxx_0, 0))
        self.connect((self.s4a_vfdm_equalizer_0, 2), (self.blocks_null_sink_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.dc_blocker_xx_1, 0))



    def get_usrp_max(self):
        return self.usrp_max

    def set_usrp_max(self, usrp_max):
        self.usrp_max = usrp_max

    def get_silence_block_rep(self):
        return self.silence_block_rep

    def set_silence_block_rep(self, silence_block_rep):
        self.silence_block_rep = silence_block_rep

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate)
        self.osmosdr_source_0.set_sample_rate(self.samp_rate)

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
