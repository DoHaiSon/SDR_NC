# Copyright 2011,2012 Free Software Foundation, Inc.
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

########################################################################
# Install public header files
########################################################################
install(FILES
    api.h
    qam16_mapper.h
    qam16_demapper.h
    psk_mapper.h
    psk_demapper.h
    sequence_combination.h
    sequence_inversion.h
    sequence_splitting.h
    signal_gain.h
    snr_check.h
    image_text_source.h
    image_text_sink.h
    int_to_char.h
    char_to_int.h
    image_vector_source.h
    image_vector_sink.h DESTINATION include/hoang
)
