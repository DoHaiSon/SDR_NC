#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Sat Jan 23 11:42:14 2021
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
import CogNC
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
        self.vfdm_pilot_block_rep = vfdm_pilot_block_rep = 1
        self.vfdm_data_block_sz = vfdm_data_block_sz = 30
        self.silence_block_rep_0 = silence_block_rep_0 = 1
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1e6
        self.precoder_length = precoder_length = 16
        self.preamble_block_rep_0 = preamble_block_rep_0 = 1
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_t_sz = pilot_block_t_sz = 16
        self.pilot_block_rep = pilot_block_rep = 1
        self.ofdm_pilot_block_rep = ofdm_pilot_block_rep = 25
        self.ofdm_data_block_sz = ofdm_data_block_sz = 21
        self.occupied_tones_0 = occupied_tones_0 = 48
        self.occupied_tones = occupied_tones = 48
        self.fft_length_0 = fft_length_0 = 128
        self.fft_length = fft_length = 128
        self.data_size_0 = data_size_0 = 54
        self.data_size = data_size = 54
        self.data_block_sz = data_block_sz = 21
        self.data_block_rep_0 = data_block_rep_0 = 1
        self.data_block_rep = data_block_rep = 1
        self.cp_length_0 = cp_length_0 = 16
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
        self.s4a_parallel_to_serial_1_1 = parallel_to_serial(occupied_tones)

        self.s4a_parallel_to_serial_1 = parallel_to_serial(data_block_sz*occupied_tones)

        self.s4a_ofdm_modulator_0_0 = ofdm_modulator(fft_length, occupied_tones, cp_length,
                               preamble_block_rep, pilot_block_rep,
                               1, data_block_rep,
                               silence_block_rep, 1/9.6571,
                               1/13.9017, 1/48.0)

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

        self.s4a_ofdm_demodulator_0 = ofdm_demodulator(fft_length, occupied_tones,
                                cp_length, preamble_block_rep,
                                pilot_block_rep, data_block_sz,
                                data_block_rep, silence_block_rep,
                                1000, 0, 0)

        self.s4a_byte_to_bit_0_2 = byte_to_bit()

        self.s4a_byte_to_bit_0_0 = byte_to_bit()

        self.s4a_bpsk_mapper_0_2 = bpsk_mapper()

        self.s4a_bpsk_mapper_0_0 = bpsk_mapper()

        self.s4a_bpsk_demapper_0_1 = bpsk_demapper()

        self.s4a_bpsk_demapper_0 = bpsk_demapper()

        self.osmosdr_source_0_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=4fcb' )
        self.osmosdr_source_0_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0_0.set_center_freq(2.435e9, 0)
        self.osmosdr_source_0_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_0_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0_0.set_gain(30, 0)
        self.osmosdr_source_0_0_0.set_if_gain(20, 0)
        self.osmosdr_source_0_0_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_0_0.set_antenna('', 0)
        self.osmosdr_source_0_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_source_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=3144' )
        self.osmosdr_source_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0.set_center_freq(2.435e9, 0)
        self.osmosdr_source_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0.set_gain(30, 0)
        self.osmosdr_source_0_0.set_if_gain(20, 0)
        self.osmosdr_source_0_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_0.set_antenna('', 0)
        self.osmosdr_source_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=3144' )
        self.osmosdr_sink_0_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0_0.set_center_freq(2.435e9, 0)
        self.osmosdr_sink_0_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0_0.set_gain(30, 0)
        self.osmosdr_sink_0_0.set_if_gain(20, 0)
        self.osmosdr_sink_0_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0_0.set_antenna('', 0)
        self.osmosdr_sink_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=4fcb' )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.435e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(30, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna('', 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)

        self.dc_blocker_xx_0_0_0 = filter.dc_blocker_cc(256, True)
        self.dc_blocker_xx_0_0 = filter.dc_blocker_cc(256, True)
        self.blocks_vector_to_stream_0 = blocks.vector_to_stream(gr.sizeof_char*1, data_size+6)
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_char*1, 6)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*occupied_tones)
        self.blocks_null_sink_0_0_0_1 = blocks.null_sink(gr.sizeof_char*1)
        self.blocks_null_sink_0_0_0_0 = blocks.null_sink(gr.sizeof_gr_complex*48)
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_multiply_const_vxx_0_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_multiply_const_vxx_0_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.DNC2_zero_elimination_0_0 = DNC2.zero_elimination(121, 6)
        self.DNC2_zero_elimination_0 = DNC2.zero_elimination(121, data_size)
        self.DNC2_image_vector_source_0_0 = DNC2.image_vector_source('/home/ubuntu/SDR_NC/VFDM/Images/lena_gray_128.txt', 128*128, data_size, False, 1, 1, [])
        self.CogNC_primary_source_control_0 = CogNC.primary_source_control(data_size, 120, 121, 6, 48)
        self.CogNC_primary_destination_control_0 = CogNC.primary_destination_control(data_size_0, 120, 121, 5)
        self.CogNC_flow_filling_0_0 = CogNC.flow_filling(1)
        self.CogNC_flow_filling_0 = CogNC.flow_filling(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_flow_filling_0, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.CogNC_flow_filling_0_0, 0), (self.osmosdr_sink_0_0, 0))
        self.connect((self.CogNC_primary_destination_control_0, 1), (self.DNC2_zero_elimination_0, 0))
        self.connect((self.CogNC_primary_destination_control_0, 0), (self.DNC2_zero_elimination_0_0, 0))
        self.connect((self.CogNC_primary_source_control_0, 1), (self.blocks_null_sink_0_0_0_0, 0))
        self.connect((self.CogNC_primary_source_control_0, 0), (self.blocks_vector_to_stream_0, 0))
        self.connect((self.DNC2_image_vector_source_0_0, 0), (self.CogNC_primary_source_control_0, 2))
        self.connect((self.DNC2_zero_elimination_0, 0), (self.blocks_null_sink_0_0_0_1, 0))
        self.connect((self.DNC2_zero_elimination_0, 0), (self.show_image_0, 0))
        self.connect((self.DNC2_zero_elimination_0_0, 0), (self.s4a_byte_to_bit_0_2, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0_0, 0), (self.s4a_ofdm_demodulator_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0_0_0, 0), (self.s4a_ofdm_demodulator_0_1, 0))
        self.connect((self.blocks_stream_to_vector_0, 0), (self.CogNC_primary_source_control_0, 0))
        self.connect((self.blocks_vector_to_stream_0, 0), (self.s4a_byte_to_bit_0_0, 0))
        self.connect((self.dc_blocker_xx_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0, 0))
        self.connect((self.dc_blocker_xx_0_0_0, 0), (self.blocks_multiply_const_vxx_0_0_0_0, 0))
        self.connect((self.osmosdr_source_0_0, 0), (self.dc_blocker_xx_0_0, 0))
        self.connect((self.osmosdr_source_0_0_0, 0), (self.dc_blocker_xx_0_0_0, 0))
        self.connect((self.s4a_bpsk_demapper_0, 0), (self.CogNC_primary_destination_control_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_1, 0), (self.blocks_stream_to_vector_0, 0))
        self.connect((self.s4a_bpsk_mapper_0_0, 0), (self.s4a_ofdm_modulator_0, 0))
        self.connect((self.s4a_bpsk_mapper_0_2, 0), (self.s4a_ofdm_modulator_0_0, 0))
        self.connect((self.s4a_byte_to_bit_0_0, 0), (self.s4a_bpsk_mapper_0_0, 0))
        self.connect((self.s4a_byte_to_bit_0_2, 0), (self.s4a_bpsk_mapper_0_2, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 2), (self.blocks_null_sink_0_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 1), (self.blocks_null_sink_0_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0, 0), (self.s4a_parallel_to_serial_1, 0))
        self.connect((self.s4a_ofdm_demodulator_0_1, 1), (self.CogNC_primary_source_control_0, 1))
        self.connect((self.s4a_ofdm_demodulator_0_1, 2), (self.blocks_null_sink_0_0_0, 0))
        self.connect((self.s4a_ofdm_demodulator_0_1, 0), (self.s4a_parallel_to_serial_1_1, 0))
        self.connect((self.s4a_ofdm_modulator_0, 0), (self.CogNC_flow_filling_0, 0))
        self.connect((self.s4a_ofdm_modulator_0_0, 0), (self.CogNC_flow_filling_0_0, 0))
        self.connect((self.s4a_parallel_to_serial_1, 0), (self.s4a_bpsk_demapper_0, 0))
        self.connect((self.s4a_parallel_to_serial_1_1, 0), (self.s4a_bpsk_demapper_0_1, 0))

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

    def get_silence_block_rep_0(self):
        return self.silence_block_rep_0

    def set_silence_block_rep_0(self, silence_block_rep_0):
        self.silence_block_rep_0 = silence_block_rep_0

    def get_silence_block_rep(self):
        return self.silence_block_rep

    def set_silence_block_rep(self, silence_block_rep):
        self.silence_block_rep = silence_block_rep

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.osmosdr_source_0_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_source_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)

    def get_precoder_length(self):
        return self.precoder_length

    def set_precoder_length(self, precoder_length):
        self.precoder_length = precoder_length

    def get_preamble_block_rep_0(self):
        return self.preamble_block_rep_0

    def set_preamble_block_rep_0(self, preamble_block_rep_0):
        self.preamble_block_rep_0 = preamble_block_rep_0

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

    def get_ofdm_pilot_block_rep(self):
        return self.ofdm_pilot_block_rep

    def set_ofdm_pilot_block_rep(self, ofdm_pilot_block_rep):
        self.ofdm_pilot_block_rep = ofdm_pilot_block_rep

    def get_ofdm_data_block_sz(self):
        return self.ofdm_data_block_sz

    def set_ofdm_data_block_sz(self, ofdm_data_block_sz):
        self.ofdm_data_block_sz = ofdm_data_block_sz

    def get_occupied_tones_0(self):
        return self.occupied_tones_0

    def set_occupied_tones_0(self, occupied_tones_0):
        self.occupied_tones_0 = occupied_tones_0

    def get_occupied_tones(self):
        return self.occupied_tones

    def set_occupied_tones(self, occupied_tones):
        self.occupied_tones = occupied_tones

    def get_fft_length_0(self):
        return self.fft_length_0

    def set_fft_length_0(self, fft_length_0):
        self.fft_length_0 = fft_length_0

    def get_fft_length(self):
        return self.fft_length

    def set_fft_length(self, fft_length):
        self.fft_length = fft_length

    def get_data_size_0(self):
        return self.data_size_0

    def set_data_size_0(self, data_size_0):
        self.data_size_0 = data_size_0

    def get_data_size(self):
        return self.data_size

    def set_data_size(self, data_size):
        self.data_size = data_size

    def get_data_block_sz(self):
        return self.data_block_sz

    def set_data_block_sz(self, data_block_sz):
        self.data_block_sz = data_block_sz

    def get_data_block_rep_0(self):
        return self.data_block_rep_0

    def set_data_block_rep_0(self, data_block_rep_0):
        self.data_block_rep_0 = data_block_rep_0

    def get_data_block_rep(self):
        return self.data_block_rep

    def set_data_block_rep(self, data_block_rep):
        self.data_block_rep = data_block_rep

    def get_cp_length_0(self):
        return self.cp_length_0

    def set_cp_length_0(self, cp_length_0):
        self.cp_length_0 = cp_length_0

    def get_cp_length(self):
        return self.cp_length

    def set_cp_length(self, cp_length):
        self.cp_length = cp_length


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
