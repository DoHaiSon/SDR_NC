# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

install(FILES
    hoang_qam16_mapper.xml
    hoang_qam16_demapper.xml
    hoang_psk_mapper.xml
    hoang_psk_demapper.xml
    hoang_sequence_combination.xml
    hoang_sequence_inversion.xml
    hoang_sequence_splitting.xml
    hoang_signal_gain.xml
    hoang_snr_check.xml
    hoang_image_text_source.xml
    hoang_image_text_sink.xml
    hoang_int_to_char.xml
    hoang_char_to_int.xml
    hoang_image_vector_source.xml
    hoang_image_vector_sink.xml DESTINATION share/gnuradio/grc/blocks
)
