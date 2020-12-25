#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Fri Jul 12 11:11:36 2013
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
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
		self.samp_rate = samp_rate = 32e2
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
		self.s4a_serial_to_parallel_1 = serial_to_parallel(fft_length + cp_length)
			
		self.s4a_serial_to_parallel_0 = serial_to_parallel(occupied_tones)
			
		self.s4a_parallel_to_serial_0 = parallel_to_serial(fft_length + cp_length)
			
		self.s4a_ofdm_sync_0 = ofdm_sync(fft_length, occupied_tones, cp_length,
		                  preamble_block_rep, pilot_block_rep,
		                  data_block_sz, data_block_rep,
		                  silence_block_rep, 1,
		                  1000, 0)
			
		self.s4a_ofdm_power_scaling_0 = ofdm_power_scaling(fft_length, 1/9.6571, 1/13.9017,
		                  1/48.0, preamble_block_rep, pilot_block_rep,
		                  1, data_block_rep, silence_block_rep)
		  
		self.s4a_ofdm_equalizer_0 = ofdm_equalizer(fft_length,occupied_tones,cp_length,
		                       preamble_block_rep, pilot_block_rep,
		                       1, data_block_rep,
		                        silence_block_rep,0)
			
		self.s4a_frame_generation_0 = frame_generation(fft_length, occupied_tones, preamble_block_rep,
		                        pilot_block_rep, 1,
		                        data_block_rep, silence_block_rep)
			
		self.s4a_cyclic_prefix_removal_0 = cyclic_prefix_removal(fft_length,cp_length)
			
		self.s4a_cyclic_prefix_insertion_0 = cyclic_prefix_insertion(fft_length,cp_length)
			
		self.s4a_coherence_time_estimator_0 = coherence_time_estimator(2, 2,
		          occupied_tones, occupied_tones, 1, 1,
		          0, 7, (fft_length + cp_length)*(preamble_block_rep + pilot_block_rep + data_block_rep + silence_block_rep),
		          1000000, 16, 1,
		          32, 1, 0)
			
		self.s4a_coherence_time_counter_generator_0 = coherence_time_counter_generator(16,
		                    1, 32,
		                    1, 0)
			
		self.s4a_bpsk_mapper_0 = bpsk_mapper()
			
		self.fft_vxx_1 = fft.fft_vcc(fft_length, False, (window.blackmanharris(1024)), True, 1)
		self.fft_vxx_0 = fft.fft_vcc(fft_length, True, (window.blackmanharris(1024)), True, 1)
		self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate)
		self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
		self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
		self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_int*1)
		self.blocks_add_xx_0 = blocks.add_vcc(1)
		self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, 0, 0)

		##################################################
		# Connections
		##################################################
		self.connect((self.s4a_coherence_time_counter_generator_0, 0), (self.s4a_bpsk_mapper_0, 0))
		self.connect((self.s4a_ofdm_sync_0, 0), (self.s4a_serial_to_parallel_1, 0))
		self.connect((self.s4a_serial_to_parallel_1, 0), (self.s4a_cyclic_prefix_removal_0, 0))
		self.connect((self.s4a_ofdm_equalizer_0, 0), (self.s4a_coherence_time_estimator_0, 0))
		self.connect((self.s4a_ofdm_equalizer_0, 1), (self.s4a_coherence_time_estimator_0, 1))
		self.connect((self.s4a_ofdm_equalizer_0, 2), (self.s4a_coherence_time_estimator_0, 2))
		self.connect((self.s4a_ofdm_power_scaling_0, 0), (self.s4a_cyclic_prefix_insertion_0, 0))
		self.connect((self.s4a_cyclic_prefix_insertion_0, 0), (self.s4a_parallel_to_serial_0, 0))
		self.connect((self.s4a_serial_to_parallel_0, 0), (self.s4a_frame_generation_0, 0))
		self.connect((self.s4a_bpsk_mapper_0, 0), (self.blocks_throttle_0, 0))
		self.connect((self.blocks_throttle_0, 0), (self.s4a_serial_to_parallel_0, 0))
		self.connect((self.s4a_frame_generation_0, 0), (self.fft_vxx_0, 0))
		self.connect((self.fft_vxx_0, 0), (self.s4a_ofdm_power_scaling_0, 0))
		self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 0))
		self.connect((self.s4a_parallel_to_serial_0, 0), (self.blocks_add_xx_0, 1))
		self.connect((self.blocks_add_xx_0, 0), (self.s4a_ofdm_sync_0, 0))
		self.connect((self.s4a_cyclic_prefix_removal_0, 0), (self.fft_vxx_1, 0))
		self.connect((self.fft_vxx_1, 0), (self.s4a_ofdm_equalizer_0, 0))
		self.connect((self.s4a_coherence_time_estimator_0, 0), (self.blocks_null_sink_0, 0))
		self.connect((self.s4a_coherence_time_estimator_0, 1), (self.blocks_null_sink_0_0_0, 0))
		self.connect((self.s4a_coherence_time_estimator_0, 2), (self.blocks_null_sink_0_0, 0))


# QT sink close method reimplementation

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
		self.blocks_throttle_0.set_sample_rate(self.samp_rate)

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
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = top_block()
	tb.Start(True)
        tb.Wait()

