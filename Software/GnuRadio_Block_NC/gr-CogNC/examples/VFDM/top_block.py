#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Tue May  5 16:27:34 2015
##################################################

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from s4a import *
import CogNC
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
                                         data_block_sz*2,1)
        	
        self.s4a_byte_to_bit_0 = byte_to_bit()
        	
        self.s4a_bpsk_mapper_0 = bpsk_mapper()
        	
        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()
        	
        self.qtgui_time_sink_x_0 = qtgui.time_sink_c(
        	200000, #size
        	samp_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(1)
        self.qtgui_time_sink_x_0.set_y_axis(-1, 1)
        
        self.qtgui_time_sink_x_0.set_y_label("Amplitude", "")
        
        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0.enable_grid(True)
        
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
        self.qtgui_number_sink_0_0 = qtgui.number_sink(
                gr.sizeof_float,
                0,
                qtgui.NUM_GRAPH_HORIZ,
        	1
        )
        self.qtgui_number_sink_0_0.set_update_time(0.10)
        self.qtgui_number_sink_0_0.set_title("")
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        units = ["", "", "", "", "",
                  "", "", "", "", ""]
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_0_0.set_min(i, -1)
            self.qtgui_number_sink_0_0.set_max(i, 1)
            self.qtgui_number_sink_0_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0_0.set_label(i, labels[i])
            self.qtgui_number_sink_0_0.set_unit(i, units[i])
            self.qtgui_number_sink_0_0.set_factor(i, factor[i])
        
        self.qtgui_number_sink_0_0.enable_autoscale(False)
        self._qtgui_number_sink_0_0_win = sip.wrapinstance(self.qtgui_number_sink_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_0_0_win)
        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.415e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(6, 0)
        self.osmosdr_sink_0.set_if_gain(30, 0)
        self.osmosdr_sink_0.set_bb_gain(3, 0)
        self.osmosdr_sink_0.set_antenna("", 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)
          
        self.blocks_vector_source_x_1_0_0 = blocks.vector_source_b([0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,7,8,8,8,8,8,8,9,9,9,9,9,9], True, 1, [])
        self.blocks_vector_source_x_0_0_0 = blocks.vector_source_c([1.0086-0.7621j,0.9668-0.8128j,0.9781-0.8767j,0.9430-0.9825j,0.8753-1.0319j,0.8574-1.0759j,0.8097-1.1702j,0.7052-1.2219j,0.6291-1.2288j,0.5623-1.2482j,0.4859-1.2443j,0.4425-1.2353j,0.3860-1.2571j,0.2984-1.2465j,0.2472-1.2099j,0.2007-1.2069j,0.1146-1.1829j,0.0580-1.1095j,0.0472-1.0575j,0.0232-1.0348j,-0.0156-1.0027j,-0.0530-0.9659j,-0.1030-0.9230j,-0.1568-0.8458j,-0.1704-0.7367j,-0.1296-0.6523j,-0.0978-0.6237j,-0.1141-0.5839j,-0.1093-0.4980j,-0.0617-0.4358j,-0.0385-0.3992j,-0.0202-0.3280j,0.0423-0.2591j,0.1028-0.2250j,0.1484-0.1781j,0.2238-0.1205j,0.3226-0.0885j,0.4254-0.0821j,0.5222-0.1036j,0.5940-0.1359j,0.6624-0.1534j,0.7517-0.1794j,0.8460-0.2359j,0.9262-0.3290j,0.9524-0.4448j,0.9339-0.5092j,0.9702-0.5351j,1.0335-0.6359j], True, 1, [])
        self.blocks_throttle_0_0_0 = blocks.throttle(gr.sizeof_gr_complex*occupied_tones, samp_rate,True)
        self.blocks_throttle_0_0 = blocks.throttle(gr.sizeof_gr_complex*precoder_length, samp_rate,True)
        self.blocks_null_sink_0_2_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_1_1_0 = blocks.null_sink(gr.sizeof_gr_complex*(precoder_length*(fft_length+cp_length)))
        self.CogNC_flow_filling_0 = CogNC.flow_filling((fft_length+precoder_length)*(preamble_block_rep+data_block_rep*data_block_sz+pilot_block_rep*pilot_block_t_sz+silence_block_rep))

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_flow_filling_0, 0), (self.osmosdr_sink_0, 0))    
        self.connect((self.CogNC_flow_filling_0, 0), (self.qtgui_time_sink_x_0, 0))    
        self.connect((self.CogNC_flow_filling_0, 0), (self.s4a_vfdm_sync_0, 0))    
        self.connect((self.blocks_throttle_0_0, 0), (self.s4a_vfdm_precoder_0, 0))    
        self.connect((self.blocks_throttle_0_0_0, 0), (self.s4a_vfdm_precoder_0, 1))    
        self.connect((self.blocks_vector_source_x_0_0_0, 0), (self.s4a_serial_to_parallel_2, 0))    
        self.connect((self.blocks_vector_source_x_1_0_0, 0), (self.s4a_byte_to_bit_0, 0))    
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.s4a_console_sink_xx_0_0_0, 0))    
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_serial_to_parallel_0, 0))    
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))    
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.CogNC_flow_filling_0, 0))    
        self.connect((self.s4a_parallel_to_serial_1_0, 0), (self.s4a_bpsk_demapper_0_0, 0))    
        self.connect((self.s4a_serial_to_parallel_0, 0), (self.blocks_throttle_0_0, 0))    
        self.connect((self.s4a_serial_to_parallel_1, 0), (self.s4a_vfdm_equalizer_0, 0))    
        self.connect((self.s4a_serial_to_parallel_2, 0), (self.blocks_throttle_0_0_0, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 1), (self.blocks_null_sink_0_1_1_0, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 2), (self.blocks_null_sink_0_2_1, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 2), (self.qtgui_number_sink_0_0, 0))    
        self.connect((self.s4a_vfdm_equalizer_0, 0), (self.s4a_parallel_to_serial_1_0, 0))    
        self.connect((self.s4a_vfdm_power_scaling_0, 0), (self.s4a_parallel_to_serial_0, 0))    
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
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)
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
