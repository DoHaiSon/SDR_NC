/* -*- c++ -*- */

#define HOANG_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "hoang_swig_doc.i"

%{
#include "hoang/qam16_mapper.h"
#include "hoang/qam16_demapper.h"
#include "hoang/psk_mapper.h"
#include "hoang/psk_demapper.h"
#include "hoang/sequence_combination.h"
#include "hoang/sequence_inversion.h"
#include "hoang/sequence_splitting.h"
#include "hoang/signal_gain.h"
#include "hoang/snr_check.h"
#include "hoang/image_text_source.h"
#include "hoang/image_text_sink.h"
#include "hoang/int_to_char.h"
#include "hoang/char_to_int.h"
#include "hoang/image_vector_source.h"
#include "hoang/image_vector_sink.h"
%}


%include "hoang/qam16_mapper.h"
GR_SWIG_BLOCK_MAGIC2(hoang, qam16_mapper);
%include "hoang/qam16_demapper.h"
GR_SWIG_BLOCK_MAGIC2(hoang, qam16_demapper);
%include "hoang/psk_mapper.h"
GR_SWIG_BLOCK_MAGIC2(hoang, psk_mapper);
%include "hoang/psk_demapper.h"
GR_SWIG_BLOCK_MAGIC2(hoang, psk_demapper);
%include "hoang/sequence_combination.h"
GR_SWIG_BLOCK_MAGIC2(hoang, sequence_combination);
%include "hoang/sequence_inversion.h"
GR_SWIG_BLOCK_MAGIC2(hoang, sequence_inversion);
%include "hoang/sequence_splitting.h"
GR_SWIG_BLOCK_MAGIC2(hoang, sequence_splitting);
%include "hoang/signal_gain.h"
GR_SWIG_BLOCK_MAGIC2(hoang, signal_gain);
%include "hoang/snr_check.h"
GR_SWIG_BLOCK_MAGIC2(hoang, snr_check);
%include "hoang/image_text_source.h"
GR_SWIG_BLOCK_MAGIC2(hoang, image_text_source);
%include "hoang/image_text_sink.h"
GR_SWIG_BLOCK_MAGIC2(hoang, image_text_sink);
%include "hoang/int_to_char.h"
GR_SWIG_BLOCK_MAGIC2(hoang, int_to_char);
%include "hoang/char_to_int.h"
GR_SWIG_BLOCK_MAGIC2(hoang, char_to_int);
%include "hoang/image_vector_source.h"
GR_SWIG_BLOCK_MAGIC2(hoang, image_vector_source);
%include "hoang/image_vector_sink.h"
GR_SWIG_BLOCK_MAGIC2(hoang, image_vector_sink);
