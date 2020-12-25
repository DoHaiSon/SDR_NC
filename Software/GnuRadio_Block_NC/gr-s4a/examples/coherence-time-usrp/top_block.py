#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Thu Jun 28 17:20:35 2012
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from grc_gnuradio import usrp as grc_usrp
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
from s4a import *
import wx

class top_block(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="Top Block")

		##################################################
		# Variables
		##################################################
		self.usrp_max = usrp_max = 32768
		self.silence_block_rep = silence_block_rep = 1
		self.samp_rate = samp_rate = 3200000
		self.preamble_block_rep = preamble_block_rep = 1
		self.pilot_block_rep = pilot_block_rep = 1
		self.occupied_tones = occupied_tones = 48
		self.fft_length = fft_length = 128
		self.data_block_sz = data_block_sz = 1
		self.data_block_rep = data_block_rep = 1
		self.cp_length = cp_length = 16

		##################################################
		# Blocks
		##################################################
		self.gr_fft_vxx_1 = gr.fft_vcc(fft_length, True, ([]), True)
		self.gr_null_sink_0_0 = gr.null_sink(gr.sizeof_int*1)
		self.gr_null_sink_0_0_0 = gr.null_sink(gr.sizeof_gr_complex*128)
		self.gr_null_sink_0_2 = gr.null_sink(gr.sizeof_float*1)
		self.gr_null_sink_0_2_0 = gr.null_sink(gr.sizeof_gr_complex*48)
		self.gr_null_sink_0_2_1 = gr.null_sink(gr.sizeof_float*1)
		self.s4a_coherence_time_estimator_0 = coherence_time_estimator(1, 2,
		          occupied_tones*data_block_sz, 48, 1, 1,
		          0, 7, (fft_length + cp_length)*(preamble_block_rep + pilot_block_rep + data_block_rep + silence_block_rep),
		          1000000, 16, 0,
		          32, 1, 0)
			
		self.s4a_cyclic_prefix_removal_0 = cyclic_prefix_removal(fft_length,cp_length)
			
		self.s4a_ofdm_equalizer_0 = ofdm_equalizer(fft_length,occupied_tones,cp_length,
		                       preamble_block_rep, pilot_block_rep,
		                       data_block_sz, data_block_rep,
		                        silence_block_rep,0)
			
		self.s4a_ofdm_sync_0 = ofdm_sync(fft_length, occupied_tones, cp_length,
		                  preamble_block_rep, pilot_block_rep,
		                  data_block_sz, data_block_rep,
		                  silence_block_rep, 1,
		                  1000, 0)
			
		self.s4a_serial_to_parallel_1 = serial_to_parallel(fft_length + cp_length)
			
		self.usrp_simple_source_x_0 = grc_usrp.simple_source_c(which=0, side="A", rx_ant="TX/RX")
		self.usrp_simple_source_x_0.set_decim_rate(64)
		self.usrp_simple_source_x_0.set_frequency(2422000e3, verbose=True)
		self.usrp_simple_source_x_0.set_gain(20)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_fft_vxx_1, 0), (self.s4a_ofdm_equalizer_0, 0))
		self.connect((self.gr_fft_vxx_1, 0), (self.gr_null_sink_0_0_0, 0))
		self.connect((self.s4a_ofdm_equalizer_0, 2), (self.gr_null_sink_0_2_1, 0))
		self.connect((self.s4a_coherence_time_estimator_0, 0), (self.gr_null_sink_0_0, 0))
		self.connect((self.s4a_coherence_time_estimator_0, 2), (self.gr_null_sink_0_2, 0))
		self.connect((self.s4a_cyclic_prefix_removal_0, 0), (self.gr_fft_vxx_1, 0))
		self.connect((self.s4a_coherence_time_estimator_0, 1), (self.gr_null_sink_0_2_0, 0))
		self.connect((self.s4a_serial_to_parallel_1, 0), (self.s4a_cyclic_prefix_removal_0, 0))
		self.connect((self.s4a_ofdm_sync_0, 0), (self.s4a_serial_to_parallel_1, 0))
		self.connect((self.s4a_ofdm_equalizer_0, 0), (self.s4a_coherence_time_estimator_0, 0))
		self.connect((self.s4a_ofdm_equalizer_0, 1), (self.s4a_coherence_time_estimator_0, 1))
		self.connect((self.s4a_ofdm_equalizer_0, 2), (self.s4a_coherence_time_estimator_0, 2))
		self.connect((self.usrp_simple_source_x_0, 0), (self.s4a_ofdm_sync_0, 0))

	def set_usrp_max(self, usrp_max):
		self.usrp_max = usrp_max

	def set_silence_block_rep(self, silence_block_rep):
		self.silence_block_rep = silence_block_rep

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate

	def set_preamble_block_rep(self, preamble_block_rep):
		self.preamble_block_rep = preamble_block_rep

	def set_pilot_block_rep(self, pilot_block_rep):
		self.pilot_block_rep = pilot_block_rep

	def set_occupied_tones(self, occupied_tones):
		self.occupied_tones = occupied_tones

	def set_fft_length(self, fft_length):
		self.fft_length = fft_length

	def set_data_block_sz(self, data_block_sz):
		self.data_block_sz = data_block_sz

	def set_data_block_rep(self, data_block_rep):
		self.data_block_rep = data_block_rep

	def set_cp_length(self, cp_length):
		self.cp_length = cp_length

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = top_block()
	tb.Run(True)

