/* -*- c++ -*- */

#define DNC2_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "DNC2_swig_doc.i"

%{
#include "DNC2/hamming_encoder.h"
#include "DNC2/hamming_decoder.h"
#include "DNC2/zero_elimination.h"
#include "DNC2/image_vector_source.h"
#include "DNC2/convolutional_encoder.h"
#include "DNC2/convolutional_decoder.h"
%}


%include "DNC2/hamming_encoder.h"
GR_SWIG_BLOCK_MAGIC2(DNC2, hamming_encoder);
%include "DNC2/hamming_decoder.h"
GR_SWIG_BLOCK_MAGIC2(DNC2, hamming_decoder);
%include "DNC2/zero_elimination.h"
GR_SWIG_BLOCK_MAGIC2(DNC2, zero_elimination);
%include "DNC2/image_vector_source.h"
GR_SWIG_BLOCK_MAGIC2(DNC2, image_vector_source);
%include "DNC2/convolutional_encoder.h"
GR_SWIG_BLOCK_MAGIC2(DNC2, convolutional_encoder);
%include "DNC2/convolutional_decoder.h"
GR_SWIG_BLOCK_MAGIC2(DNC2, convolutional_decoder);
