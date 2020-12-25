#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Fri Apr 17 17:23:27 2015
##################################################

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from s4a import *
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
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1e6
        self.precoder_length = precoder_length = 16
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_t_sz = pilot_block_t_sz = 16
        self.pilot_block_rep = pilot_block_rep = 1
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_block_sz = data_block_sz = 30
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.s4a_vfdm_sync_0 = vfdm_sync(fft_length, occupied_tones, cp_length, 
                          precoder_length, preamble_block_rep, 
                          pilot_block_t_sz, pilot_block_rep, 
                          data_block_sz, data_block_rep,
                          silence_block_rep, 
                          1, 
        		  1000,
                          0)
        	
        self.s4a_vfdm_precoder_0 = vfdm_precoder(fft_length, occupied_tones, cp_length,
                              precoder_length,
                              48, preamble_block_rep,
                              pilot_block_t_sz, 
                              pilot_block_rep,data_block_sz, data_block_rep,
                              silence_block_rep)
        	
        self.s4a_vfdm_power_scaling_0 = vfdm_power_scaling(fft_length, precoder_length,
                                   1/9.6571, 1/0.8964, 
                                   1/0.8964, preamble_block_rep, 
                                   pilot_block_t_sz, pilot_block_rep, 
                                   data_block_sz, data_block_rep, 
                                   silence_block_rep)
          
        self.s4a_vfdm_equalizer_0 = vfdm_equalizer((fft_length+cp_length),occupied_tones, cp_length,
                              precoder_length, preamble_block_rep,
                              pilot_block_t_sz, pilot_block_rep,
                              data_block_sz, data_block_rep,
                              silence_block_rep, 0)
        	
        self.s4a_serial_to_parallel_2 = serial_to_parallel(occupied_tones)
        	
        self.s4a_serial_to_parallel_1 = serial_to_parallel(fft_length + cp_length)
        	
        self.s4a_serial_to_parallel_0 = serial_to_parallel(precoder_length)
        	
        self.s4a_parallel_to_serial_1_0 = parallel_to_serial((precoder_length*data_block_sz))
        	
        self.s4a_parallel_to_serial_0 = parallel_to_serial((precoder_length + fft_length))
        	
        self.s4a_console_sink_xx_0_0_0 = console_sink_bb("equalized",
                                         60,1)
        	
        self.s4a_byte_to_bit_0 = byte_to_bit()
        	
        self.s4a_bpsk_mapper_0 = bpsk_mapper()
        	
        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()
        	
        self.blocks_vector_source_x_1_0 = blocks.vector_source_b([0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9], True, 1, [])
        self.blocks_vector_source_x_0 = blocks.vector_source_c([0.3964-0.7168j,0.5697+0.0168j,0.0462-0.0352j,-0.0028-0.0045j,-0.0098+0.0060j,0.0032+0.0018j,-0.0104-0.0000j,0.0060-0.0045j,0.0026+0.0094j,0.0101-0.0032j,0.0045+0.0008j,-0.0007-0.0067j,-0.0049-0.0037j,0.0039-0.0059j,-0.0079-0.0039j,0.0001-0.0065j,0.0024-0.0060j,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], True, 1, [])
        self.blocks_throttle_0_0_0 = blocks.throttle(gr.sizeof_gr_complex*48, samp_rate,True)
        self.blocks_throttle_0_0 = blocks.throttle(gr.sizeof_gr_complex*16, samp_rate,True)
        self.blocks_null_sink_0_2_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_2 = blocks.null_sink(gr.sizeof_gr_complex*144)
        self.blocks_null_sink_0_1_1_0 = blocks.null_sink(gr.sizeof_gr_complex*(precoder_length*(fft_length+cp_length)))
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_char*1)
        self.blocks_add_xx_0 = blocks.add_vcc(1)
        self.analog_noise_source_x_0 = analog.noise_source_c(analog.GR_GAUSSIAN, 0, 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_noise_source_x_0, 0), (self.blocks_add_xx_0, 1))    
        self.connect((self.blocks_add_xx_0, 0), (self.s4a_vfdm_sync_0, 0))    
        self.connect((self.blocks_throttle_0_0, 0), (self.s4a_vfdm_precoder_0, 0))    
        self.connect((self.blocks_throttle_0_0_0, 0), (self.s4a_vfdm_precoder_0, 1))    
        self.connect((self.blocks_vector_source_x_0, 0), (self.s4a_serial_to_parallel_2, 0))    
        self.connect((self.blocks_vector_source_x_1_0, 0), (self.s4a_byte_to_bit_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.blocks_null_sink_0_0_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.s4a_console_sink_xx_0_0_0, 0))    
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_serial_to_parallel_0, 0))    
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))    
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.blocks_add_xx_0, 0))    
        self.connect((self.s4a_parallel_to_serial_1_0, 0), (self.s4a_bpsk_demapper_0_0, 0))    
        self.connect((self.s4a_serial_to_parallel_0, 0), (self.blocks_throttle_0_0, 0))    
        self.connect((self.s4a_serial_to_parallel_1, 0), (self.s4a_vfdm_equalizer_0, 0))    
        self.connect((self.s4a_serial_to_parallel_2, 0), (self.blocks_throttle_0_0_0, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 1), (self.blocks_null_sink_0_1_1_0, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 2), (self.blocks_null_sink_0_2_1, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 0), (self.s4a_parallel_to_serial_1_0, 0))    
        self.connect((self.s4a_vfdm_power_scaling_0, 0), (self.s4a_parallel_to_serial_0, 0))    
        self.connect((self.s4a_vfdm_precoder_0, 0), (self.blocks_null_sink_0_2, 0))    
        self.connect((self.s4a_vfdm_precoder_0, 0), (self.s4a_vfdm_power_scaling_0, 0))    
        self.connect((self.s4a_vfdm_sync_0, 0), (self.s4a_serial_to_parallel_1, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_silence_block_rep(self):
        return self.silence_block_rep

    def set_silence_block_rep(self, silence_block_rep):
        self.silence_block_rep = silence_block_rep

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0_0.set_sample_rate(self.samp_rate)
        self.blocks_throttle_0_0_0.set_sample_rate(self.samp_rate)

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
