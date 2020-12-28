#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Mon Dec 28 11:54:05 2020
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
import DNC
import DNC2
import display
import osmosdr
import sip
import sys
import time
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        qtgui.util.check_set_qss()
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
        self.samp_rate = samp_rate = 1e6

        ##################################################
        # Blocks
        ##################################################
        self.show_image_0 = display.show_image(
        	256,  # Picture width
        	256,  # Picture height
        )
        self._show_image_0_win = sip.wrapinstance(self.show_image_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._show_image_0_win)
        self.show_0 = display.show_image(
        	256,  # Picture width
        	256,  # Picture height
        )
        self._show_0_win = sip.wrapinstance(self.show_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._show_0_win, 1,1,1,1)
        self.s4a_parallel_to_serial_0_2_0 = parallel_to_serial(1008)

        self.s4a_parallel_to_serial_0_2 = parallel_to_serial(1008)

        self.s4a_ofdm_modulator_0_1_0 = ofdm_modulator(128, 48, 16,
                               1, 1,
                               21, 1,
                               1, 1/9.6571,
                               1/13.9017, 1/48.0)

        self.s4a_ofdm_modulator_0_1 = ofdm_modulator(128, 48, 16,
                               1, 1,
                               21, 1,
                               1, 1/9.6571,
                               1/13.9017, 1/48.0)

        self.s4a_ofdm_demodulator_0_2_0 = ofdm_demodulator(128, 48,
                                16, 1,
                                1, 21,
                                1, 1,
                                1000, 0, 0)

        self.s4a_ofdm_demodulator_0_2 = ofdm_demodulator(128, 48,
                                16, 1,
                                1, 21,
                                1, 1,
                                1000, 0, 0)

        self.s4a_byte_to_bit_0_1_0 = byte_to_bit()

        self.s4a_byte_to_bit_0_1 = byte_to_bit()

        self.s4a_bpsk_mapper_0_1_0 = bpsk_mapper()

        self.s4a_bpsk_mapper_0_1 = bpsk_mapper()

        self.s4a_bpsk_demapper_0_2_0 = bpsk_demapper()

        self.s4a_bpsk_demapper_0_2 = bpsk_demapper()

        self.qtgui_time_sink_x_0 = qtgui.time_sink_c(
        	1024, #size
        	samp_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.2)
        self.qtgui_time_sink_x_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0.enable_control_panel(False)

        if not True:
          self.qtgui_time_sink_x_0.disable_legend()

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
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

        for i in xrange(2):
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
        self.osmosdr_source_0_1_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=3627' )
        self.osmosdr_source_0_1_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_1_0.set_center_freq(2.425e9, 0)
        self.osmosdr_source_0_1_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_1_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_1_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_1_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_1_0.set_gain(30, 0)
        self.osmosdr_source_0_1_0.set_if_gain(20, 0)
        self.osmosdr_source_0_1_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_1_0.set_antenna('', 0)
        self.osmosdr_source_0_1_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_source_0_1 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=2a45' )
        self.osmosdr_source_0_1.set_sample_rate(samp_rate)
        self.osmosdr_source_0_1.set_center_freq(2.425e9, 0)
        self.osmosdr_source_0_1.set_freq_corr(0, 0)
        self.osmosdr_source_0_1.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_1.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_1.set_gain_mode(False, 0)
        self.osmosdr_source_0_1.set_gain(30, 0)
        self.osmosdr_source_0_1.set_if_gain(20, 0)
        self.osmosdr_source_0_1.set_bb_gain(20, 0)
        self.osmosdr_source_0_1.set_antenna('', 0)
        self.osmosdr_source_0_1.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0_1_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=3627' )
        self.osmosdr_sink_0_1_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0_1_0.set_center_freq(2.41525e9, 0)
        self.osmosdr_sink_0_1_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0_1_0.set_gain(30, 0)
        self.osmosdr_sink_0_1_0.set_if_gain(20, 0)
        self.osmosdr_sink_0_1_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0_1_0.set_antenna('', 0)
        self.osmosdr_sink_0_1_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0_1 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=2a45' )
        self.osmosdr_sink_0_1.set_sample_rate(samp_rate)
        self.osmosdr_sink_0_1.set_center_freq(2.41475e9, 0)
        self.osmosdr_sink_0_1.set_freq_corr(0, 0)
        self.osmosdr_sink_0_1.set_gain(30, 0)
        self.osmosdr_sink_0_1.set_if_gain(20, 0)
        self.osmosdr_sink_0_1.set_bb_gain(20, 0)
        self.osmosdr_sink_0_1.set_antenna('', 0)
        self.osmosdr_sink_0_1.set_bandwidth(1.5e6, 0)

        self.dc_blocker_xx_0_1_0 = filter.dc_blocker_cc(256, True)
        self.dc_blocker_xx_0_1 = filter.dc_blocker_cc(256, True)
        self.blocks_null_sink_1_1_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_1_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_1_0 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_multiply_const_vxx_0_1_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_multiply_const_vxx_0_1 = blocks.multiply_const_vcc((25e3, ))
        self.DNC_end_node_control_0_0 = DNC.end_node_control(64, 120, 122)
        self.DNC_end_node_control_0 = DNC.end_node_control(64, 121, 122)
        self.DNC_check_received_packet_0_0 = DNC.check_received_packet(64, 122)
        self.DNC_check_received_packet_0 = DNC.check_received_packet(64, 122)
        self.DNC2_zero_elimination_0_0 = DNC2.zero_elimination(121, 64)
        self.DNC2_zero_elimination_0 = DNC2.zero_elimination(120, 64)
        self.DNC2_image_vector_source_0_0_0 = DNC2.image_vector_source('/home/ubuntu/Desktop/SDR_NC/3 Nodes/Without_Channel_Coding/Control/barbara_gray_256.txt', 256*256, 64, False, 0, 1, [])
        self.DNC2_image_vector_source_0_0 = DNC2.image_vector_source('/home/ubuntu/Desktop/SDR_NC/3 Nodes/Without_Channel_Coding/Control/lena_gray_256.txt', 256*256, 64, False, 0, 1, [])

        ##################################################
        # Connections
        ##################################################
        self.connect((self.DNC2_image_vector_source_0_0, 0), (self.DNC_end_node_control_0_0, 1))
        self.connect((self.DNC2_image_vector_source_0_0_0, 0), (self.DNC_end_node_control_0, 1))
        self.connect((self.DNC2_zero_elimination_0, 0), (self.show_image_0, 0))
        self.connect((self.DNC2_zero_elimination_0_0, 0), (self.show_0, 0))
        self.connect((self.DNC_check_received_packet_0, 0), (self.DNC_end_node_control_0, 0))
        self.connect((self.DNC_check_received_packet_0_0, 0), (self.DNC_end_node_control_0_0, 0))
        self.connect((self.DNC_end_node_control_0, 1), (self.DNC2_zero_elimination_0_0, 0))
        self.connect((self.DNC_end_node_control_0, 0), (self.s4a_byte_to_bit_0_1_0, 0))
        self.connect((self.DNC_end_node_control_0_0, 1), (self.DNC2_zero_elimination_0, 0))
        self.connect((self.DNC_end_node_control_0_0, 0), (self.s4a_byte_to_bit_0_1, 0))
        self.connect((self.blocks_multiply_const_vxx_0_1, 0), (self.s4a_ofdm_demodulator_0_2, 0))
        self.connect((self.blocks_multiply_const_vxx_0_1_0, 0), (self.s4a_ofdm_demodulator_0_2_0, 0))
        self.connect((self.dc_blocker_xx_0_1, 0), (self.blocks_multiply_const_vxx_0_1, 0))
        self.connect((self.dc_blocker_xx_0_1_0, 0), (self.blocks_multiply_const_vxx_0_1_0, 0))
        self.connect((self.dc_blocker_xx_0_1_0, 0), (self.qtgui_time_sink_x_0, 0))
        self.connect((self.osmosdr_source_0_1, 0), (self.dc_blocker_xx_0_1, 0))
        self.connect((self.osmosdr_source_0_1_0, 0), (self.dc_blocker_xx_0_1_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_2, 0), (self.DNC_check_received_packet_0_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_2_0, 0), (self.DNC_check_received_packet_0, 0))
        self.connect((self.s4a_bpsk_mapper_0_1, 0), (self.s4a_ofdm_modulator_0_1, 0))
        self.connect((self.s4a_bpsk_mapper_0_1_0, 0), (self.s4a_ofdm_modulator_0_1_0, 0))
        self.connect((self.s4a_byte_to_bit_0_1, 0), (self.s4a_bpsk_mapper_0_1, 0))
        self.connect((self.s4a_byte_to_bit_0_1_0, 0), (self.s4a_bpsk_mapper_0_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_2, 1), (self.blocks_null_sink_0_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0_2, 2), (self.blocks_null_sink_1_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0_2, 0), (self.s4a_parallel_to_serial_0_2, 0))
        self.connect((self.s4a_ofdm_demodulator_0_2_0, 1), (self.blocks_null_sink_0_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_2_0, 2), (self.blocks_null_sink_1_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_2_0, 0), (self.s4a_parallel_to_serial_0_2_0, 0))
        self.connect((self.s4a_ofdm_modulator_0_1, 0), (self.osmosdr_sink_0_1, 0))
        self.connect((self.s4a_ofdm_modulator_0_1_0, 0), (self.osmosdr_sink_0_1_0, 0))
        self.connect((self.s4a_parallel_to_serial_0_2, 0), (self.s4a_bpsk_demapper_0_2, 0))
        self.connect((self.s4a_parallel_to_serial_0_2_0, 0), (self.s4a_bpsk_demapper_0_2_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)
        self.osmosdr_source_0_1_0.set_sample_rate(self.samp_rate)
        self.osmosdr_source_0_1.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0_1_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0_1.set_sample_rate(self.samp_rate)


def main(top_block_cls=top_block, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
