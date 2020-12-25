#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Wed Sep  9 08:42:51 2015
##################################################

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from s4a import *
import CogNC
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
        self.usrp_max = usrp_max = 32768
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1000000
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_rep = pilot_block_rep = 25
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_block_sz = data_block_sz = 21
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.s4a_parallel_to_serial_1 = parallel_to_serial(data_block_sz*occupied_tones)
        	
        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(fft_length, occupied_tones, 
                                cp_length, preamble_block_rep,
                                pilot_block_rep, data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)
        	
        self.s4a_console_sink_xx_0 = console_sink_bb("seq",
                                         123,1)
        	
        self.s4a_byte_to_bit_0 = byte_to_bit()
        	
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
        self.blocks_null_sink_0_2 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_char*1)
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, "/home/ssl-lap4/sandbox/matlab-cogNC/primaryRX/ofdm_rx.dump", False)
        self.blocks_file_sink_1_0 = blocks.file_sink(gr.sizeof_char*1, "/home/ssl-lap4/sandbox/matlab-cogNC/primaryRX/ofdm_bit.dump", False)
        self.blocks_file_sink_1_0.set_unbuffered(False)
        self.blocks_file_sink_1 = blocks.file_sink(gr.sizeof_float*1, "/home/ssl-lap4/sandbox/matlab-cogNC/primaryRX/ofdm_snr.dump", False)
        self.blocks_file_sink_1.set_unbuffered(False)
        self.analog_const_source_x_0 = analog.sig_source_f(0, analog.GR_CONST_WAVE, 0, 0, 0)
        self.CogNC_vfdm_header_removal_0 = CogNC.vfdm_header_removal(120, 255, 3)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_vfdm_header_removal_0, 0), (self.s4a_byte_to_bit_0, 0))    
        self.connect((self.CogNC_vfdm_header_removal_0, 0), (self.s4a_console_sink_xx_0, 0))    
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_float_to_complex_0, 1))    
        self.connect((self.blocks_file_source_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))    
        self.connect((self.blocks_file_source_0, 0), (self.qtgui_time_sink_x_0, 0))    
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_null_sink_0_2, 0))    
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.s4a_ofdm_demodulator_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.CogNC_vfdm_header_removal_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.blocks_null_sink_0, 0))    
        self.connect((self.s4a_byte_to_bit_0, 0), (self.blocks_file_sink_1_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_file_sink_1, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_float_to_complex_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_0_0, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0_1, 0))    
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_1, 0))    
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

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
