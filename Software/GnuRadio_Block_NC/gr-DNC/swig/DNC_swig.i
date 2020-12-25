/* -*- c++ -*- */

#define DNC_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "DNC_swig_doc.i"

%{
#include "DNC/relay_control.h"
#include "DNC/end_node_control.h"
#include "DNC/delay_packet.h"
#include "DNC/check_received_packet.h"
#include "DNC/transmitter_control_p2p.h"
#include "DNC/receiver_control_p2p.h"
#include "DNC/end_node_controller.h"
#include "DNC/relay_node_controller.h"
#include "DNC/terminal_controller.h"
#include "DNC/relay_controller.h"
#include "DNC/nodeB_controller.h"
#include "DNC/nodeR_controller.h"
#include "DNC/flow_controller_cc.h"
#include "DNC/nodeA_controller.h"
#include "DNC/nodeC_controller.h"
#include "DNC/source1_controller.h"
#include "DNC/flow_controller_bb.h"
#include "DNC/packet_arrangement.h"
#include "DNC/source2_controller.h"
#include "DNC/relay1_controller.h"
#include "DNC/relay2_controller.h"
#include "DNC/destination1_controller.h"
#include "DNC/destination2_controller.h"
#include "DNC/MDC_decoding.h"
#include "DNC/LC_decoding.h"
%}


%include "DNC/relay_control.h"
GR_SWIG_BLOCK_MAGIC2(DNC, relay_control);
%include "DNC/end_node_control.h"
GR_SWIG_BLOCK_MAGIC2(DNC, end_node_control);
%include "DNC/delay_packet.h"
GR_SWIG_BLOCK_MAGIC2(DNC, delay_packet);
%include "DNC/check_received_packet.h"
GR_SWIG_BLOCK_MAGIC2(DNC, check_received_packet);
%include "DNC/transmitter_control_p2p.h"
GR_SWIG_BLOCK_MAGIC2(DNC, transmitter_control_p2p);
%include "DNC/receiver_control_p2p.h"
GR_SWIG_BLOCK_MAGIC2(DNC, receiver_control_p2p);
%include "DNC/end_node_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, end_node_controller);
%include "DNC/relay_node_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, relay_node_controller);
%include "DNC/terminal_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, terminal_controller);
%include "DNC/relay_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, relay_controller);
%include "DNC/nodeB_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, nodeB_controller);
%include "DNC/nodeR_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, nodeR_controller);
%include "DNC/flow_controller_cc.h"
GR_SWIG_BLOCK_MAGIC2(DNC, flow_controller_cc);
%include "DNC/nodeA_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, nodeA_controller);
%include "DNC/nodeC_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, nodeC_controller);
%include "DNC/source1_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, source1_controller);
%include "DNC/flow_controller_bb.h"
GR_SWIG_BLOCK_MAGIC2(DNC, flow_controller_bb);
%include "DNC/packet_arrangement.h"
GR_SWIG_BLOCK_MAGIC2(DNC, packet_arrangement);
%include "DNC/source2_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, source2_controller);
%include "DNC/relay1_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, relay1_controller);
%include "DNC/relay2_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, relay2_controller);
%include "DNC/destination1_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, destination1_controller);
%include "DNC/destination2_controller.h"
GR_SWIG_BLOCK_MAGIC2(DNC, destination2_controller);
%include "DNC/MDC_decoding.h"
GR_SWIG_BLOCK_MAGIC2(DNC, MDC_decoding);
%include "DNC/LC_decoding.h"
GR_SWIG_BLOCK_MAGIC2(DNC, LC_decoding);
