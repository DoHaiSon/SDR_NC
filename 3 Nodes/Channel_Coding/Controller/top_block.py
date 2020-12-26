#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Sat Dec 26 07:28:14 2020
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
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from s4a import *
import DNC
import osmosdr
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
        self.s4a_parallel_to_serial_0_0 = parallel_to_serial(1008)

        self.s4a_parallel_to_serial_0 = parallel_to_serial(1008)

        self.s4a_ofdm_modulator_0 = ofdm_modulator(128, 48, 16,
                               1, 1,
                               21, 1,
                               1, 1/9.6571,
                               1/13.9017, 1/48.0)

        self.s4a_ofdm_demodulator_0_0 = ofdm_demodulator(128, 48,
                                16, 1,
                                1, 21,
                                1, 1,
                                1000, 0, 0)

        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(128, 48,
                                16, 1,
                                1, 21,
                                1, 1,
                                1000, 0, 0)

        self.s4a_byte_to_bit_0 = byte_to_bit()

        self.s4a_bpsk_mapper_0 = bpsk_mapper()

        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()

        self.s4a_bpsk_demapper_0 = bpsk_demapper()

        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=ca4c' )
        self.osmosdr_source_0.set_sample_rate(samp_rate )
        self.osmosdr_source_0.set_center_freq(2.415e9, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(30, 0)
        self.osmosdr_source_0.set_if_gain(20, 0)
        self.osmosdr_source_0.set_bb_gain(20, 0)
        self.osmosdr_source_0.set_antenna('', 0)
        self.osmosdr_source_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=ca4c' )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.425e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(30, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna('', 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)

        self.low_pass_filter_0_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 200e3, 50e3, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, 200e3, 50e3, firdes.WIN_HAMMING, 6.76))
        self.dc_blocker_xx_0 = filter.dc_blocker_cc(256, True)
        self.blocks_null_sink_1_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((25e3, ))
        self.analog_sig_source_x_0_0 = analog.sig_source_c(samp_rate, analog.GR_SIN_WAVE, -250e3, 0.5, 0)
        self.analog_sig_source_x_0 = analog.sig_source_c(samp_rate, analog.GR_SIN_WAVE, 250e3, 0.5, 0)
        self.DNC_relay_node_controller_0 = DNC.relay_node_controller(64, 6, 120, 121, 122)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.DNC_relay_node_controller_0, 0), (self.s4a_byte_to_bit_0, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.analog_sig_source_x_0_0, 0), (self.blocks_multiply_xx_0_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_multiply_xx_0_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.low_pass_filter_0_0, 0))
        self.connect((self.dc_blocker_xx_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.low_pass_filter_0, 0), (self.s4a_ofdm_demodulator_0, 0))
        self.connect((self.low_pass_filter_0_0, 0), (self.s4a_ofdm_demodulator_0_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.dc_blocker_xx_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.DNC_relay_node_controller_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.DNC_relay_node_controller_0, 1))
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_ofdm_modulator_0, 0))
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 1), (self.blocks_null_sink_0_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 2), (self.blocks_null_sink_1_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_0, 0), (self.s4a_parallel_to_serial_0_0, 0))
        self.connect((self.s4a_ofdm_modulator_0, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.s4a_parallel_to_serial_0, 0), (self.s4a_bpsk_demapper_0, 0))
        self.connect((self.s4a_parallel_to_serial_0_0, 0), (self.s4a_bpsk_demapper_0_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.osmosdr_source_0.set_sample_rate(self.samp_rate )
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)
        self.low_pass_filter_0_0.set_taps(firdes.low_pass(1, self.samp_rate, 200e3, 50e3, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, 200e3, 50e3, firdes.WIN_HAMMING, 6.76))
        self.analog_sig_source_x_0_0.set_sampling_freq(self.samp_rate)
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate)


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
