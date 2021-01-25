#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Mon Mar  7 20:31:57 2016
##################################################

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from s4a import *
import CogNC
import DNC2
import display
import osmosdr
import sip
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
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_rep = pilot_block_rep = 25
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_size = data_size = 120
        self.data_block_sz = data_block_sz = 21
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.show_image_0 = display.show_image(
        	128,  # Picture width
        	128,  # Picture height
        )
        self._show_image_0_win = sip.wrapinstance(self.show_image_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._show_image_0_win)
        self.s4a_parallel_to_serial_1 = parallel_to_serial(data_block_sz*occupied_tones)
        	
        self.s4a_ofdm_modulator_0 = ofdm_modulator(fft_length, occupied_tones, cp_length,
                               preamble_block_rep, 1,
                               1, data_block_rep,
                               silence_block_rep, 1/9.6571,
                               1/13.9017, 1/48.0)
        	
        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(fft_length, occupied_tones, 
                                cp_length, preamble_block_rep,
                                pilot_block_rep, data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)
        	
        self.s4a_byte_to_bit_0 = byte_to_bit()
        	
        self.s4a_bpsk_mapper_0 = bpsk_mapper()
        	
        self.s4a_bpsk_demapper_0 = bpsk_demapper()
        	
        self.qtgui_time_sink_x_0 = qtgui.time_sink_c(
        	20000, #size
        	samp_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0.set_y_axis(-1, 1)
        
        self.qtgui_time_sink_x_0.set_y_label("Amplitude", "")
        
        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0.enable_grid(False)
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(2*1):
            if len(labels[i]) == 0:
                if(i % 2 == 0):
                    self.qtgui_time_sink_x_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_win)
        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_source_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0.set_center_freq(2.435e9, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(0, 0)
        self.osmosdr_source_0.set_iq_balance_mode(0, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(6, 0)
        self.osmosdr_source_0.set_if_gain(30, 0)
        self.osmosdr_source_0.set_bb_gain(3, 0)
        self.osmosdr_source_0.set_antenna("", 0)
        self.osmosdr_source_0.set_bandwidth(1.5e6, 0)
          
        self.osmosdr_sink_1 = osmosdr.sink( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_sink_1.set_sample_rate(samp_rate)
        self.osmosdr_sink_1.set_center_freq(2.435e9, 0)
        self.osmosdr_sink_1.set_freq_corr(0, 0)
        self.osmosdr_sink_1.set_gain(6, 0)
        self.osmosdr_sink_1.set_if_gain(30, 0)
        self.osmosdr_sink_1.set_bb_gain(3, 0)
        self.osmosdr_sink_1.set_antenna("", 0)
        self.osmosdr_sink_1.set_bandwidth(1.5e6, 0)
          
        self.dc_blocker_xx_0_0 = filter.dc_blocker_cc(256, True)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_char*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.DNC2_zero_elimination_0_0 = DNC2.zero_elimination(170, 6)
        self.DNC2_zero_elimination_0 = DNC2.zero_elimination(170, data_size)
        self.CogNC_primary_destination_control_0 = CogNC.primary_destination_control(data_size, 187, 170, 20)
        self.CogNC_flow_filling_0 = CogNC.flow_filling(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_flow_filling_0, 0), (self.osmosdr_sink_1, 0))    
        self.connect((self.CogNC_primary_destination_control_0, 1), (self.DNC2_zero_elimination_0, 0))    
        self.connect((self.CogNC_primary_destination_control_0, 0), (self.DNC2_zero_elimination_0_0, 0))    
        self.connect((self.DNC2_zero_elimination_0, 0), (self.blocks_null_sink_0_0_0, 0))    
        self.connect((self.DNC2_zero_elimination_0, 0), (self.show_image_0, 0))    
        self.connect((self.DNC2_zero_elimination_0_0, 0), (self.s4a_byte_to_bit_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.s4a_ofdm_demodulator_0, 0))    
        self.connect((self.dc_blocker_xx_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))    
        self.connect((self.dc_blocker_xx_0_0, 0), (self.qtgui_time_sink_x_0, 0))    
        self.connect((self.osmosdr_source_0, 0), (self.dc_blocker_xx_0_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.CogNC_primary_destination_control_0, 0))    
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_ofdm_modulator_0, 0))    
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_0_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0_1, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_1, 0))    
        self.connect((self.s4a_ofdm_modulator_0, 0), (self.CogNC_flow_filling_0, 0))    
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))    

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
        self.osmosdr_source_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_1.set_sample_rate(self.samp_rate)
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)

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
