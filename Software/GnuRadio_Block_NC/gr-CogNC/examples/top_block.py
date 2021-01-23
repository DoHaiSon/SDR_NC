#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Sat Jan 23 09:21:21 2021
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
        self.silence_block_rep = silence_block_rep = 1
        self.samp_rate = samp_rate = 1e6
        self.precoder_length = precoder_length = 16
        self.preamble_block_rep = preamble_block_rep = 1
        self.pilot_block_t_sz = pilot_block_t_sz = 16
        self.ofdm_pilot_block_rep = ofdm_pilot_block_rep = 1
        self.ofdm_data_block_sz = ofdm_data_block_sz = 21
        self.occupied_tones = occupied_tones = 48
        self.fft_length = fft_length = 128
        self.data_size = data_size = 54
        self.data_block_rep = data_block_rep = 1
        self.cp_length = cp_length = 16

        ##################################################
        # Blocks
        ##################################################
        self.show_image_0_1 = display.show_image(
        	256,  # Picture width
        	256,  # Picture height
        )
        self._show_image_0_1_win = sip.wrapinstance(self.show_image_0_1.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._show_image_0_1_win)
        self.show_image_0 = display.show_image(
        	256,  # Picture width
        	256,  # Picture height
        )
        self._show_image_0_win = sip.wrapinstance(self.show_image_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._show_image_0_win)
        self.s4a_vfdm_sync_0_0 = vfdm_sync(fft_length, occupied_tones, cp_length,
                          precoder_length, preamble_block_rep,
                          pilot_block_t_sz, vfdm_pilot_block_rep,
                          vfdm_data_block_sz, data_block_rep,
                          silence_block_rep,
                          1,
        		  1000,
                          0)

        self.s4a_vfdm_sync_0 = vfdm_sync(fft_length, occupied_tones, cp_length,
                          precoder_length, preamble_block_rep,
                          pilot_block_t_sz, vfdm_pilot_block_rep,
                          vfdm_data_block_sz, data_block_rep,
                          silence_block_rep,
                          1,
        		  1000,
                          0)

        self.s4a_vfdm_equalizer_0_0 = vfdm_equalizer((fft_length+cp_length),occupied_tones, cp_length,
                              precoder_length, preamble_block_rep,
                              pilot_block_t_sz, vfdm_pilot_block_rep,
                              vfdm_data_block_sz, data_block_rep,
                              silence_block_rep, 0)

        self.s4a_vfdm_equalizer_0 = vfdm_equalizer((fft_length+cp_length),occupied_tones, cp_length,
                              precoder_length, preamble_block_rep,
                              pilot_block_t_sz, vfdm_pilot_block_rep,
                              vfdm_data_block_sz, data_block_rep,
                              silence_block_rep, 0)

        self.s4a_serial_to_parallel_1_0 = serial_to_parallel(fft_length + cp_length)

        self.s4a_serial_to_parallel_1 = serial_to_parallel(fft_length + cp_length)

        self.s4a_parallel_to_serial_1_0_1 = parallel_to_serial((precoder_length*vfdm_data_block_sz))

        self.s4a_parallel_to_serial_1_0 = parallel_to_serial((precoder_length*vfdm_data_block_sz))

        self.s4a_ofdm_modulator_0_0_0 = ofdm_modulator(fft_length, occupied_tones, cp_length,
                               preamble_block_rep, ofdm_pilot_block_rep,
                               ofdm_data_block_sz, data_block_rep,
                               silence_block_rep, 1/9.6571,
                               1/13.9017, 1/48.0)

        self.s4a_ofdm_modulator_0_0 = ofdm_modulator(fft_length, occupied_tones, cp_length,
                               preamble_block_rep, ofdm_pilot_block_rep,
                               ofdm_data_block_sz, data_block_rep,
                               silence_block_rep, 1/9.6571,
                               1/13.9017, 1/48.0)

        self.s4a_byte_to_bit_0_0 = byte_to_bit()

        self.s4a_byte_to_bit_0 = byte_to_bit()

        self.s4a_bpsk_mapper_0_0 = bpsk_mapper()

        self.s4a_bpsk_mapper_0 = bpsk_mapper()

        self.s4a_bpsk_demapper_0_0_0 = bpsk_demapper()

        self.s4a_bpsk_demapper_0_0 = bpsk_demapper()

        self.qtgui_time_sink_x_0_0 = qtgui.time_sink_c(
        	20000, #size
        	samp_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0_0.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0_0.enable_autoscale(True)
        self.qtgui_time_sink_x_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0_0.enable_control_panel(False)

        if not True:
          self.qtgui_time_sink_x_0_0.disable_legend()

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
                    self.qtgui_time_sink_x_0_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_0_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_0_win)
        self.qtgui_time_sink_x_0 = qtgui.time_sink_c(
        	20000, #size
        	samp_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
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
        self.osmosdr_source_0_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=ca4c' )
        self.osmosdr_source_0_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0_0.set_center_freq(2.425e9, 0)
        self.osmosdr_source_0_0.set_freq_corr(0, 0)
        self.osmosdr_source_0_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0_0.set_gain_mode(False, 0)
        self.osmosdr_source_0_0.set_gain(30, 0)
        self.osmosdr_source_0_0.set_if_gain(20, 0)
        self.osmosdr_source_0_0.set_bb_gain(20, 0)
        self.osmosdr_source_0_0.set_antenna('', 0)
        self.osmosdr_source_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + 'bladerf=2a40' )
        self.osmosdr_source_0.set_sample_rate(samp_rate)
        self.osmosdr_source_0.set_center_freq(2.425e9, 0)
        self.osmosdr_source_0.set_freq_corr(0, 0)
        self.osmosdr_source_0.set_dc_offset_mode(1, 0)
        self.osmosdr_source_0.set_iq_balance_mode(1, 0)
        self.osmosdr_source_0.set_gain_mode(False, 0)
        self.osmosdr_source_0.set_gain(30, 0)
        self.osmosdr_source_0.set_if_gain(20, 0)
        self.osmosdr_source_0.set_bb_gain(20, 0)
        self.osmosdr_source_0.set_antenna('', 0)
        self.osmosdr_source_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=ca4c' )
        self.osmosdr_sink_0_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0_0.set_center_freq(2.415e9+250e3, 0)
        self.osmosdr_sink_0_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0_0.set_gain(30, 0)
        self.osmosdr_sink_0_0.set_if_gain(20, 0)
        self.osmosdr_sink_0_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0_0.set_antenna('', 0)
        self.osmosdr_sink_0_0.set_bandwidth(1.5e6, 0)

        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + 'bladerf=2a40' )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(2.415e9-250e3, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(30, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna('', 0)
        self.osmosdr_sink_0.set_bandwidth(1.5e6, 0)

        self.dc_blocker_xx_0_0 = filter.dc_blocker_cc(256, True)
        self.dc_blocker_xx_0 = filter.dc_blocker_cc(256, True)
        self.blocks_null_sink_0_2_1_0 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_2_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0_1_1_0_1 = blocks.null_sink(gr.sizeof_gr_complex*(precoder_length*(fft_length+cp_length)))
        self.blocks_null_sink_0_1_1_0 = blocks.null_sink(gr.sizeof_gr_complex*(precoder_length*(fft_length+cp_length)))
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_vcc((25e3, ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((25e3, ))
        self.DNC2_zero_elimination_0_0 = DNC2.zero_elimination(121, data_size)
        self.DNC2_zero_elimination_0 = DNC2.zero_elimination(120, data_size)
        self.DNC2_image_vector_source_0_0 = DNC2.image_vector_source('/home/ubuntu/SDR_NC/3 Nodes/Without_Channel_Coding/Controller/lena_gray_256.txt', 256*256, data_size, False, 0, 1, [])
        self.DNC2_image_vector_source_0 = DNC2.image_vector_source('/home/ubuntu/SDR_NC/3 Nodes/Without_Channel_Coding/Controller/barbara_gray_256.txt', 256*256, data_size, False, 0, 1, [])
        self.CogNC_secondary_endnode_control_0_0 = CogNC.secondary_endnode_control(data_size, 50, 0, 6, 121, 122)
        self.CogNC_secondary_endnode_control_0 = CogNC.secondary_endnode_control(data_size, 50, 0, 6, 120, 122)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.CogNC_secondary_endnode_control_0, 1), (self.DNC2_zero_elimination_0, 0))
        self.connect((self.CogNC_secondary_endnode_control_0, 0), (self.s4a_byte_to_bit_0, 0))
        self.connect((self.CogNC_secondary_endnode_control_0_0, 1), (self.DNC2_zero_elimination_0_0, 0))
        self.connect((self.CogNC_secondary_endnode_control_0_0, 0), (self.s4a_byte_to_bit_0_0, 0))
        self.connect((self.DNC2_image_vector_source_0, 0), (self.CogNC_secondary_endnode_control_0, 1))
        self.connect((self.DNC2_image_vector_source_0_0, 0), (self.CogNC_secondary_endnode_control_0_0, 1))
        self.connect((self.DNC2_zero_elimination_0, 0), (self.show_image_0, 0))
        self.connect((self.DNC2_zero_elimination_0_0, 0), (self.show_image_0_1, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.s4a_vfdm_sync_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.s4a_vfdm_sync_0_0, 0))
        self.connect((self.dc_blocker_xx_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.dc_blocker_xx_0, 0), (self.qtgui_time_sink_x_0, 0))
        self.connect((self.dc_blocker_xx_0_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.dc_blocker_xx_0_0, 0), (self.qtgui_time_sink_x_0_0, 0))
        self.connect((self.osmosdr_source_0, 0), (self.dc_blocker_xx_0, 0))
        self.connect((self.osmosdr_source_0_0, 0), (self.dc_blocker_xx_0_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_0, 0), (self.CogNC_secondary_endnode_control_0, 0))
        self.connect((self.s4a_bpsk_demapper_0_0_0, 0), (self.CogNC_secondary_endnode_control_0_0, 0))
        self.connect((self.s4a_bpsk_mapper_0, 0), (self.s4a_ofdm_modulator_0_0, 0))
        self.connect((self.s4a_bpsk_mapper_0_0, 0), (self.s4a_ofdm_modulator_0_0_0, 0))
        self.connect((self.s4a_byte_to_bit_0, 0), (self.s4a_bpsk_mapper_0, 0))
        self.connect((self.s4a_byte_to_bit_0_0, 0), (self.s4a_bpsk_mapper_0_0, 0))
        self.connect((self.s4a_ofdm_modulator_0_0, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.s4a_ofdm_modulator_0_0_0, 0), (self.osmosdr_sink_0_0, 0))
        self.connect((self.s4a_parallel_to_serial_1_0, 0), (self.s4a_bpsk_demapper_0_0, 0))
        self.connect((self.s4a_parallel_to_serial_1_0_1, 0), (self.s4a_bpsk_demapper_0_0_0, 0))
        self.connect((self.s4a_serial_to_parallel_1, 0), (self.s4a_vfdm_equalizer_0, 0))
        self.connect((self.s4a_serial_to_parallel_1_0, 0), (self.s4a_vfdm_equalizer_0_0, 0))
        self.connect((self.s4a_vfdm_equalizer_0, 1), (self.blocks_null_sink_0_1_1_0, 0))
        self.connect((self.s4a_vfdm_equalizer_0, 2), (self.blocks_null_sink_0_2_1, 0))
        self.connect((self.s4a_vfdm_equalizer_0, 0), (self.s4a_parallel_to_serial_1_0, 0))
        self.connect((self.s4a_vfdm_equalizer_0_0, 1), (self.blocks_null_sink_0_1_1_0_1, 0))
        self.connect((self.s4a_vfdm_equalizer_0_0, 2), (self.blocks_null_sink_0_2_1_0, 0))
        self.connect((self.s4a_vfdm_equalizer_0_0, 0), (self.s4a_parallel_to_serial_1_0_1, 0))
        self.connect((self.s4a_vfdm_sync_0, 0), (self.s4a_serial_to_parallel_1, 0))
        self.connect((self.s4a_vfdm_sync_0_0, 0), (self.s4a_serial_to_parallel_1_0, 0))

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
        self.qtgui_time_sink_x_0_0.set_samp_rate(self.samp_rate)
        self.qtgui_time_sink_x_0.set_samp_rate(self.samp_rate)
        self.osmosdr_source_0_0.set_sample_rate(self.samp_rate)
        self.osmosdr_source_0.set_sample_rate(self.samp_rate)
        self.osmosdr_sink_0_0.set_sample_rate(self.samp_rate)
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
