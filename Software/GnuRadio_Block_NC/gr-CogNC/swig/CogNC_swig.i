/* -*- c++ -*- */

#define COGNC_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "CogNC_swig_doc.i"

%{
#include "CogNC/delay_packet.h"
#include "CogNC/flow_filling.h"
#include "CogNC/primary_rx_control.h"
#include "CogNC/primary_tx_control.h"
#include "CogNC/primary_source_control.h"
#include "CogNC/primary_destination_control.h"
#include "CogNC/secondary_endnode_control.h"
#include "CogNC/secondary_relay_control.h"
#include "CogNC/hybrid_stream_control.h"
#include "CogNC/vfdm_metrics_control.h"
#include "CogNC/hybrid_stream_controller.h"
#include "CogNC/vfdm_header_insertion.h"
#include "CogNC/vfdm_header_removal.h"
#include "CogNC/test_block.h"
#include "CogNC/vfdm_adaptive_precoder.h"
#include "CogNC/radom_complex_generator.h"
%}


%include "CogNC/delay_packet.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, delay_packet);
%include "CogNC/flow_filling.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, flow_filling);
%include "CogNC/primary_rx_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, primary_rx_control);
%include "CogNC/primary_tx_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, primary_tx_control);
%include "CogNC/primary_source_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, primary_source_control);
%include "CogNC/primary_destination_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, primary_destination_control);
%include "CogNC/secondary_endnode_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, secondary_endnode_control);
%include "CogNC/secondary_relay_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, secondary_relay_control);
%include "CogNC/hybrid_stream_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, hybrid_stream_control);
%include "CogNC/vfdm_metrics_control.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, vfdm_metrics_control);
%include "CogNC/hybrid_stream_controller.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, hybrid_stream_controller);
%include "CogNC/vfdm_header_insertion.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, vfdm_header_insertion);
%include "CogNC/vfdm_header_removal.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, vfdm_header_removal);
%include "CogNC/test_block.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, test_block);
%include "CogNC/vfdm_adaptive_precoder.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, vfdm_adaptive_precoder);
%include "CogNC/radom_complex_generator.h"
GR_SWIG_BLOCK_MAGIC2(CogNC, radom_complex_generator);
