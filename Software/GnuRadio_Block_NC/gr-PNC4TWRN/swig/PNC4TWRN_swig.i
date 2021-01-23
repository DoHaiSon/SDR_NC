/* -*- c++ -*- */

#define PNC4TWRN_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "PNC4TWRN_swig_doc.i"

%{
#include "PNC4TWRN/Relay_PNC_Controller.h"
#include "PNC4TWRN/End_Node_PNC_Controller.h"
%}


%include "PNC4TWRN/Relay_PNC_Controller.h"
GR_SWIG_BLOCK_MAGIC2(PNC4TWRN, Relay_PNC_Controller);
%include "PNC4TWRN/End_Node_PNC_Controller.h"
GR_SWIG_BLOCK_MAGIC2(PNC4TWRN, End_Node_PNC_Controller);
