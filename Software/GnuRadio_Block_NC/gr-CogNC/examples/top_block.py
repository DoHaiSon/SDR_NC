#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Sun Mar  6 16:53:18 2016
##################################################

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from s4a import *
import CogNC
import DNC2
import osmosdr
import sys

from distutils.version import StrictVersion
class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        try:
             self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
             pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


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
        self.ofdm_pilot_block_rep = ofdm_pilot_block_rep = 25
        self.ofdm_data_block_sz = ofdm_data_block_sz = 21
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_size = data_size = 120
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.s4a_vfdm_power_scaling_0 = vfdm_power_scaling(fft_length, precoder_length,
                                   1/9.6571, 1/0.8964, 
                                   1/0.8964, preamble_block_rep, 
                                   pilot_block_t_sz, vfdm_pilot_block_rep, 
                                   vfdm_data_block_sz, data_block_rep, 
                                   silence_block_rep)
          
        self.s4a_serial_to_parallel_1_0_0 = serial_to_parallel((fft_length+cp_length)*(preamble_block_rep+ofdm_data_block_sz*data_block_rep+ofdm_pilot_block_rep+silence_block_rep))
        	
        self.s4a_serial_to_parallel_1_0 = serial_to_parallel((fft_length+cp_length)*(preamble_block_rep+ofdm_data_block_sz*data_block_rep+ofdm_pilot_block_rep+silence_block_rep))
        	
        self.s4a_serial_to_parallel_0_0 = serial_to_parallel(16)
        	
        self.s4a_parallel_to_serial_1_1 = parallel_to_serial(occupied_tones)
        	
        self.s4a_parallel_to_serial_1_0 = parallel_to_serial((ofdm_data_block_sz*occupied_tones))
        	
        self.s4a_parallel_to_serial_1 = parallel_to_serial(ofdm_data_block_sz*occupied_tones)
        	
        self.s4a_parallel_to_serial_0 = parallel_to_serial((precoder_length + fft_length))
        	
        self.s4a_ofdm_modulator_0 = ofdm_modulator(fft_length, occupied_tones, cp_length,
                               preamble_block_rep, ofdm_pilot_block_rep,
                               ofdm_data_block_sz, data_block_rep,
                               silence_block_rep, 1/9.6571,
                               1/13.9017, 1/48.0)
        	
        self.s4a_ofdm_demodulator_0_1 = ofdm_demodulator(fft_length, occupied_tones, 
                                cp_length, preamble_block_rep,
                                1, 1,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)
        	
        self.s4a_ofdm_demodulator_0_0 = ofdm_demodulator(fft_length, occupied_tones, 
                                cp_length, preamble_block_rep,
                                1, ofdm_data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)
        	
        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(fft_length, occupied_tones, 
                                cp_length, preamble_block_rep,
                                1, ofdm_data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)
        	
        self.s4a_byte_to_bit_0_0 = byte_to_bit()
        	
        self.s4a_byte_to_bit_0 = byte_to_bit()
        	
        self.s4a_bpsk_mapper_0_0 = bpsk_mapper()
        	
        self.s4a_bpsk_mapper_0 = bpsk_mapper()
        	
        self.s4a_bpsk_demapper_0_1 = bpsk_demapper()
        	
        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()
        	
        self.s4a_bpsk_demapper_0 = bpsk_demapper()
        	
        self.osmosdr_source_0_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + "bladerf=0" )
        self.osmosdr_source_0_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0_0.set_center_freq(2.435e9, 0)
        self.osmosdr_source_0_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0_0.set_dc_offset_mode(0, 0)
        self.osmosdr_source_0_0_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0_0.set_gain(6, 0)
        self.osmosdr_source_0_0_0.set_if_gain(30, 0)
        self.osmosdr_source_0_0_0.set_bb_gain(3, 0)
        self.osmosdr_source_0_0_0.set_antenna("", 0)
        self.osmosdr_source_0_0_0.set_bandwidth(1.5e6, 0)
          
        self.osmosdr_source_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + "bladerf=1" )
        self.osmosdr_source_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0.set_center_freq(2.415e9, 0)
        self.osmosdr_source_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0.set_dc_offset_mode(0, 0)
        self.osmosdr_source_0_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0.set_gain(6, 0)
        self.osmosdr_source_0_0.set_if_gain(30, 0)
        self.osmosdr_source_0_0.set_bb_gain(3, 0)
        self.osmosdr_source_0_0.set_antenna("", 0)
        self.osmosdr_source_0_0.set_bandwidth(1.5e6, 0)
          
        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + "bladerf=1" )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.435e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(6, 0)
        self.osmosdr_sink_0.set_if_gain(30, 0)
        self.osmosdr_sink_0.set_bb_gain(3, 0)
        self.osmosdr_sink_0.set_antenna("", 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)
          
        self.low_pass_filter_0_1 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.dc_blocker_xx_0_0_0 = filter.dc_blocker_cc(256, True)
        self.dc_blocker_xx_0_0 = filter.dc_blocker_cc(256, True)
        self.blocks_vector_to_stream_0 = blocks.vector_to_stream(gr.sizeof_char*1, data_size+6)
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_char*1, 6)
        self.blocks_null_sink_0_1_0 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_0_1_1_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_0_1_1 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_null_sink_0_0_1_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_0_1 = blocks.null_sink(gr.sizeof_char*data_size+6)
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_0_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_sig_source_x_1_0 = analog.sig_source_c(samp_rate, analog.GR_SIN_WAVE, -250e3, 0.5, 0)
        self.analog_sig_source_x_1 = analog.sig_source_c(samp_rate, analog.GR_SIN_WAVE, 250e3, 0.5, 0)
        self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, 0, 0)
        self.DNC2_image_vector_source_0 = DNC2.image_vector_source("/home/ssl-lap1/sandbox/gnuradio-builds/Images/lena_gray_128.txt", 128*128, data_size, True, 1, 1, [])
        self.CogNC_vfdm_adaptive_precoder_0 = CogNC.vfdm_adaptive_precoder(500, 128, 48, 16,
                              48, 1,
                              16, 
                              1,30, 1,
                              1)
          
        self.CogNC_secondary_relay_control_0 = CogNC.secondary_relay_control(1, 54, 255, 0, 6, 120, 121, 122)
        self.CogNC_primary_source_control_0 = CogNC.primary_source_control(data_size, 187, 170, 6, 48)
        self.CogNC_hybrid_stream_controller_0 = CogNC.hybrid_stream_controller(0, (fft_length+cp_length)*(preamble_block_rep+ofdm_data_block_sz*data_block_rep+ofdm_pilot_block_rep+silence_block_rep), (fft_length+precoder_length)*(preamble_block_rep+vfdm_data_block_sz*data_block_rep+pilot_block_t_sz*vfdm_pilot_block_rep+silence_block_rep))
        self.CogNC_flow_filling_0 = CogNC.flow_filling(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_flow_filling_0, 0), (self.osmosdr_sink_0, 0))    
        self.connect((self.CogNC_hybrid_stream_controller_0, 0), (self.blocks_add_xx_0, 0))    
        self.connect((self.CogNC_primary_source_control_0, 1), (self.CogNC_vfdm_adaptive_precoder_0, 1))    
        self.connect((self.CogNC_primary_source_control_0, 0), (self.blocks_null_sink_0_0_1, 0))    
        self.connect((self.CogNC_primary_source_control_0, 1), (self.blocks_null_sink_0_0_1_1, 0))    
        self.connect((self.CogNC_primary_source_control_0, 0), (self.blocks_vector_to_stream_0, 0))    
        self.connect((self.CogNC_secondary_relay_control_0, 0), (self.s4a_byte_to_bit_0, 0))    
        self.connect((self.CogNC_vfdm_adaptive_precoder_0, 0), (self.s4a_vfdm_power_scaling_0, 0))    
        self.connect((self.DNC2_image_vector_source_0, 0), (self.CogNC_primary_source_control_0, 2))    
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 1))    
        self.connect((self.analog_sig_source_x_1, 0), (self.blocks_multiply_xx_0, 0))    
        self.connect((self.analog_sig_source_x_1_0, 0), (self.blocks_multiply_xx_0_0, 1))    
        self.connect((self.blocks_add_xx_0, 0), (self.CogNC_flow_filling_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.blocks_multiply_xx_0, 1))    
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.blocks_multiply_xx_0_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0_0_0_0, 0), (self.s4a_ofdm_demodulator_0_1, 0))    
        self.connect((self.blocks_multiply_xx_0, 0), (self.low_pass_filter_0_1, 0))    
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.low_pass_filter_0, 0))    
        self.connect((self.blocks_stream_to_vector_0, 0), (self.CogNC_primary_source_control_0, 0))    
        self.connect((self.blocks_vector_to_stream_0, 0), (self.s4a_byte_to_bit_0_0, 0))    
        self.connect((self.dc_blocker_xx_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))    
        self.connect((self.dc_blocker_xx_0_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0_0, 0))    
        self.connect((self.low_pass_filter_0, 0), (self.s4a_ofdm_demodulator_0_0, 0))    
        self.connect((self.low_pass_filter_0_1, 0), (self.s4a_ofdm_demodulator_0, 0))    
        self.connect((self.osmosdr_source_0_0, 0), (self.dc_blocker_xx_0_0, 0))    
        self.connect((self.osmosdr_source_0_0_0, 0), (self.dc_blocker_xx_0_0_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.CogNC_secondary_relay_control_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.CogNC_secondary_relay_control_0, 1))    
        self.connect((self.s4a_bpsk_demapper_0_1, 0), (self.blocks_stream_to_vector_0, 0))    
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_serial_to_parallel_0_0, 0))    
        self.connect((self.s4a_bpsk_mapper_0_0, 0), (self.s4a_ofdm_modulator_0, 0))    
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))    
        self.connect((self.s4a_byte_to_bit_0_0, 0), (self.s4a_bpsk_mapper_0_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_0_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0_1, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_1, 0))    
        self.connect((self.s4a_ofdm_demodulator_0_0, 2), (self.blocks_null_sink_0_0_1_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0_0, 1), (self.blocks_null_sink_0_1_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0_0, 0), (self.s4a_parallel_to_serial_1_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0_1, 1), (self.CogNC_primary_source_control_0, 1))    
        self.connect((self.s4a_ofdm_demodulator_0_1, 2), (self.blocks_null_sink_0_0_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0_1, 0), (self.s4a_parallel_to_serial_1_1, 0))    
        self.connect((self.s4a_ofdm_modulator_0, 0), (self.s4a_serial_to_parallel_1_0_0, 0))    
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.blocks_null_sink_0_0_1_1_0, 0))    
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.s4a_serial_to_parallel_1_0, 0))    
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))    
        self.connect((self.s4a_parallel_to_serial_1_0, 0), (self.s4a_bpsk_demapper_0_0, 0))    
        self.connect((self.s4a_parallel_to_serial_1_1, 0), (self.s4a_bpsk_demapper_0_1, 0))    
        self.connect((self.s4a_serial_to_parallel_0_0, 0), (self.CogNC_vfdm_adaptive_precoder_0, 0))    
        self.connect((self.s4a_serial_to_parallel_1_0, 0), (self.CogNC_hybrid_stream_controller_0, 1))    
        self.connect((self.s4a_serial_to_parallel_1_0_0, 0), (self.CogNC_hybrid_stream_controller_0, 0))    
        self.connect((self.s4a_vfdm_power_scaling_0, 0), (self.s4a_parallel_to_serial_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

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
        self.osmosdr_source_0_0_0.set_sample_rate(self.samp_rate)
        self.analog_sig_source_x_1_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_1.set_sampling_freq(self.samp_rate)
        self.low_pass_filter_0_1.set_taps(firdes.low_pass(1, self.samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, 0.2e6, 0.05e6, firdes.WIN_HAMMING, 6.76))
        self.osmosdr_source_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)

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
    if(StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0")):
        Qt.QApplication.setGraphicsSystem(gr.prefs().get_string('qtgui','style','raster'))
    qapp = Qt.QApplication(sys.argv)
    tb = top_block()
    tb.start()
    tb.show()
    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None #to clean up Qt widgets
