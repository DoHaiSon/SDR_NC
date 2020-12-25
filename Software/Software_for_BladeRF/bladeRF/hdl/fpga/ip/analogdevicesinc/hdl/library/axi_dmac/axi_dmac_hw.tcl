

package require -exact qsys 13.0
source ../scripts/adi_env.tcl
source ../scripts/adi_ip_alt.tcl

set_module_property NAME axi_dmac
set_module_property DESCRIPTION "AXI DMA Controller"
set_module_property VERSION 1.0
set_module_property GROUP "Analog Devices"
set_module_property DISPLAY_NAME axi_dmac
set_module_property ELABORATION_CALLBACK axi_dmac_elaborate

# files

add_fileset quartus_synth QUARTUS_SYNTH "" "Quartus Synthesis"
set_fileset_property quartus_synth TOP_LEVEL axi_dmac
add_fileset_file axi_dmac.qip OTHER PATH ./axi_dmac.qip

# parameters

add_parameter ID INTEGER 0
set_parameter_property ID DEFAULT_VALUE 0
set_parameter_property ID DISPLAY_NAME ID
set_parameter_property ID TYPE INTEGER
set_parameter_property ID UNITS None
set_parameter_property ID HDL_PARAMETER true

add_parameter DMA_DATA_WIDTH_SRC INTEGER 0
set_parameter_property DMA_DATA_WIDTH_SRC DEFAULT_VALUE 64
set_parameter_property DMA_DATA_WIDTH_SRC DISPLAY_NAME DMA_DATA_WIDTH_SRC
set_parameter_property DMA_DATA_WIDTH_SRC TYPE INTEGER
set_parameter_property DMA_DATA_WIDTH_SRC UNITS None
set_parameter_property DMA_DATA_WIDTH_SRC HDL_PARAMETER true

add_parameter DMA_DATA_WIDTH_DEST INTEGER 0
set_parameter_property DMA_DATA_WIDTH_DEST DEFAULT_VALUE 64
set_parameter_property DMA_DATA_WIDTH_DEST DISPLAY_NAME DMA_DATA_WIDTH_DEST
set_parameter_property DMA_DATA_WIDTH_DEST TYPE INTEGER
set_parameter_property DMA_DATA_WIDTH_DEST UNITS None
set_parameter_property DMA_DATA_WIDTH_DEST HDL_PARAMETER true

add_parameter DMA_LENGTH_WIDTH INTEGER 0
set_parameter_property DMA_LENGTH_WIDTH DEFAULT_VALUE 14
set_parameter_property DMA_LENGTH_WIDTH DISPLAY_NAME DMA_LENGTH_WIDTH
set_parameter_property DMA_LENGTH_WIDTH TYPE INTEGER
set_parameter_property DMA_LENGTH_WIDTH UNITS None
set_parameter_property DMA_LENGTH_WIDTH HDL_PARAMETER true

add_parameter DMA_2D_TRANSFER INTEGER 0
set_parameter_property DMA_2D_TRANSFER DEFAULT_VALUE 1
set_parameter_property DMA_2D_TRANSFER DISPLAY_NAME DMA_2D_TRANSFER
set_parameter_property DMA_2D_TRANSFER TYPE INTEGER
set_parameter_property DMA_2D_TRANSFER UNITS None
set_parameter_property DMA_2D_TRANSFER HDL_PARAMETER true

add_parameter ASYNC_CLK_REQ_SRC INTEGER 0
set_parameter_property ASYNC_CLK_REQ_SRC DEFAULT_VALUE 1
set_parameter_property ASYNC_CLK_REQ_SRC DISPLAY_NAME ASYNC_CLK_REQ_SRC
set_parameter_property ASYNC_CLK_REQ_SRC TYPE INTEGER
set_parameter_property ASYNC_CLK_REQ_SRC UNITS None
set_parameter_property ASYNC_CLK_REQ_SRC HDL_PARAMETER true

add_parameter ASYNC_CLK_SRC_DEST INTEGER 0
set_parameter_property ASYNC_CLK_SRC_DEST DEFAULT_VALUE 1
set_parameter_property ASYNC_CLK_SRC_DEST DISPLAY_NAME ASYNC_CLK_SRC_DEST
set_parameter_property ASYNC_CLK_SRC_DEST TYPE INTEGER
set_parameter_property ASYNC_CLK_SRC_DEST UNITS None
set_parameter_property ASYNC_CLK_SRC_DEST HDL_PARAMETER true

add_parameter ASYNC_CLK_DEST_REQ INTEGER 0
set_parameter_property ASYNC_CLK_DEST_REQ DEFAULT_VALUE 1
set_parameter_property ASYNC_CLK_DEST_REQ DISPLAY_NAME ASYNC_CLK_DEST_REQ
set_parameter_property ASYNC_CLK_DEST_REQ TYPE INTEGER
set_parameter_property ASYNC_CLK_DEST_REQ UNITS None
set_parameter_property ASYNC_CLK_DEST_REQ HDL_PARAMETER true

add_parameter AXI_SLICE_DEST INTEGER 0
set_parameter_property AXI_SLICE_DEST DEFAULT_VALUE 0
set_parameter_property AXI_SLICE_DEST DISPLAY_NAME AXI_SLICE_DEST
set_parameter_property AXI_SLICE_DEST TYPE INTEGER
set_parameter_property AXI_SLICE_DEST UNITS None
set_parameter_property AXI_SLICE_DEST HDL_PARAMETER true

add_parameter AXI_SLICE_SRC INTEGER 0
set_parameter_property AXI_SLICE_SRC DEFAULT_VALUE 0
set_parameter_property AXI_SLICE_SRC DISPLAY_NAME AXI_SLICE_SRC
set_parameter_property AXI_SLICE_SRC TYPE INTEGER
set_parameter_property AXI_SLICE_SRC UNITS None
set_parameter_property AXI_SLICE_SRC HDL_PARAMETER true

add_parameter SYNC_TRANSFER_START INTEGER 0
set_parameter_property SYNC_TRANSFER_START DEFAULT_VALUE 0
set_parameter_property SYNC_TRANSFER_START DISPLAY_NAME SYNC_TRANSFER_START
set_parameter_property SYNC_TRANSFER_START TYPE INTEGER
set_parameter_property SYNC_TRANSFER_START UNITS None
set_parameter_property SYNC_TRANSFER_START HDL_PARAMETER true

add_parameter CYCLIC INTEGER 0
set_parameter_property CYCLIC DEFAULT_VALUE 1
set_parameter_property CYCLIC DISPLAY_NAME CYCLIC
set_parameter_property CYCLIC TYPE INTEGER
set_parameter_property CYCLIC UNITS None
set_parameter_property CYCLIC HDL_PARAMETER true

add_parameter DMA_TYPE_DEST INTEGER 0
set_parameter_property DMA_TYPE_DEST DEFAULT_VALUE 0
set_parameter_property DMA_TYPE_DEST DISPLAY_NAME DMA_TYPE_DEST
set_parameter_property DMA_TYPE_DEST TYPE INTEGER
set_parameter_property DMA_TYPE_DEST UNITS None
set_parameter_property DMA_TYPE_DEST HDL_PARAMETER true

add_parameter DMA_TYPE_SRC INTEGER 0
set_parameter_property DMA_TYPE_SRC DEFAULT_VALUE 2
set_parameter_property DMA_TYPE_SRC DISPLAY_NAME DMA_TYPE_SRC
set_parameter_property DMA_TYPE_SRC TYPE INTEGER
set_parameter_property DMA_TYPE_SRC UNITS None
set_parameter_property DMA_TYPE_SRC HDL_PARAMETER true

add_parameter FIFO_SIZE INTEGER 0 "In bursts"
set_parameter_property FIFO_SIZE DEFAULT_VALUE 4
set_parameter_property FIFO_SIZE DISPLAY_NAME FIFO_SIZE
set_parameter_property FIFO_SIZE TYPE INTEGER
set_parameter_property FIFO_SIZE UNITS None
set_parameter_property FIFO_SIZE HDL_PARAMETER true

# axi4 slave

add_interface s_axi_clock clock end
add_interface_port s_axi_clock s_axi_aclk clk Input 1

add_interface s_axi_reset reset end
set_interface_property s_axi_reset associatedClock s_axi_clock
add_interface_port s_axi_reset s_axi_aresetn reset_n Input 1

add_interface s_axi axi4lite end
set_interface_property s_axi associatedClock s_axi_clock
set_interface_property s_axi associatedReset s_axi_reset
add_interface_port s_axi s_axi_awvalid awvalid Input 1
add_interface_port s_axi s_axi_awaddr awaddr Input 14
add_interface_port s_axi s_axi_awready awready Output 1
add_interface_port s_axi s_axi_wvalid wvalid Input 1
add_interface_port s_axi s_axi_wdata wdata Input 32
add_interface_port s_axi s_axi_wstrb wstrb Input 4
add_interface_port s_axi s_axi_wready wready Output 1
add_interface_port s_axi s_axi_bvalid bvalid Output 1
add_interface_port s_axi s_axi_bresp bresp Output 2
add_interface_port s_axi s_axi_bready bready Input 1
add_interface_port s_axi s_axi_arvalid arvalid Input 1
add_interface_port s_axi s_axi_araddr araddr Input 14
add_interface_port s_axi s_axi_arready arready Output 1
add_interface_port s_axi s_axi_rvalid rvalid Output 1
add_interface_port s_axi s_axi_rresp rresp Output 2
add_interface_port s_axi s_axi_rdata rdata Output 32
add_interface_port s_axi s_axi_rready rready Input 1
add_interface_port s_axi s_axi_awprot awprot Input 3
add_interface_port s_axi s_axi_arprot arprot Input 3

add_interface interrupt_sender interrupt end
set_interface_property interrupt_sender associatedAddressablePoint ""
set_interface_property interrupt_sender associatedClock s_axi_clock
set_interface_property interrupt_sender ENABLED true
set_interface_property interrupt_sender EXPORT_OF ""
set_interface_property interrupt_sender PORT_NAME_MAP ""
set_interface_property interrupt_sender CMSIS_SVD_VARIABLES ""
set_interface_property interrupt_sender SVD_ADDRESS_GROUP ""

add_interface_port interrupt_sender irq irq Output 1

# conditional interface

proc axi_dmac_elaborate {} {

  # axi4 destination/source

  if {[get_parameter_value DMA_TYPE_DEST] == 0} {

    add_interface m_dest_axi_clock clock end
    add_interface_port m_dest_axi_clock m_dest_axi_aclk clk Input 1

    add_interface m_dest_axi_reset reset end
    set_interface_property m_dest_axi_reset associatedClock m_dest_axi_clock
    add_interface_port m_dest_axi_reset m_dest_axi_aresetn reset_n Input 1

    add_interface m_dest_axi axi4 start
    set_interface_property m_dest_axi associatedClock m_dest_axi_clock
    set_interface_property m_dest_axi associatedReset m_dest_axi_reset
    add_interface_port m_dest_axi m_dest_axi_awvalid awvalid Output 1
    add_interface_port m_dest_axi m_dest_axi_awaddr awaddr Output 32
    add_interface_port m_dest_axi m_dest_axi_awready awready Input 1
    add_interface_port m_dest_axi m_dest_axi_wvalid wvalid Output 1
    add_interface_port m_dest_axi m_dest_axi_wdata wdata Output DMA_DATA_WIDTH_DEST
    add_interface_port m_dest_axi m_dest_axi_wstrb wstrb Output DMA_DATA_WIDTH_DEST/8
    add_interface_port m_dest_axi m_dest_axi_wready wready Input 1
    add_interface_port m_dest_axi m_dest_axi_bvalid bvalid Input 1
    add_interface_port m_dest_axi m_dest_axi_bresp bresp Input 2
    add_interface_port m_dest_axi m_dest_axi_bready bready Output 1
    add_interface_port m_dest_axi m_dest_axi_arvalid arvalid Output 1
    add_interface_port m_dest_axi m_dest_axi_araddr araddr Output 32
    add_interface_port m_dest_axi m_dest_axi_arready arready Input 1
    add_interface_port m_dest_axi m_dest_axi_rvalid rvalid Input 1
    add_interface_port m_dest_axi m_dest_axi_rresp rresp Input 2
    add_interface_port m_dest_axi m_dest_axi_rdata rdata Input DMA_DATA_WIDTH_DEST
    add_interface_port m_dest_axi m_dest_axi_rready rready Output 1
    add_interface_port m_dest_axi m_dest_axi_awlen awlen Output 8
    add_interface_port m_dest_axi m_dest_axi_awsize awsize Output 3
    add_interface_port m_dest_axi m_dest_axi_awburst awburst Output 2
    add_interface_port m_dest_axi m_dest_axi_awcache awcache Output 4
    add_interface_port m_dest_axi m_dest_axi_awprot awprot Output 3
    add_interface_port m_dest_axi m_dest_axi_wlast wlast Output 1
    add_interface_port m_dest_axi m_dest_axi_arlen arlen Output 8
    add_interface_port m_dest_axi m_dest_axi_arsize arsize Output 3
    add_interface_port m_dest_axi m_dest_axi_arburst arburst Output 2
    add_interface_port m_dest_axi m_dest_axi_arcache arcache Output 4
    add_interface_port m_dest_axi m_dest_axi_arprot arprot Output 3
  }

  if {[get_parameter_value DMA_TYPE_SRC] == 0} {

    add_interface m_src_axi_clock clock end
    add_interface_port m_src_axi_clock m_src_axi_aclk clk Input 1

    add_interface m_src_axi_reset reset end
    set_interface_property m_src_axi_reset associatedClock m_src_axi_clock
    add_interface_port m_src_axi_reset m_src_axi_aresetn reset_n Input 1

    add_interface m_src_axi axi4 start
    set_interface_property m_src_axi associatedClock m_src_axi_clock
    set_interface_property m_src_axi associatedReset m_src_axi_reset
    add_interface_port m_src_axi m_src_axi_awvalid awvalid Output 1
    add_interface_port m_src_axi m_src_axi_awaddr awaddr Output 32
    add_interface_port m_src_axi m_src_axi_awready awready Input 1
    add_interface_port m_src_axi m_src_axi_wvalid wvalid Output 1
    add_interface_port m_src_axi m_src_axi_wdata wdata Output DMA_DATA_WIDTH_SRC
    add_interface_port m_src_axi m_src_axi_wstrb wstrb Output DMA_DATA_WIDTH_SRC/8
    add_interface_port m_src_axi m_src_axi_wready wready Input 1
    add_interface_port m_src_axi m_src_axi_bvalid bvalid Input 1
    add_interface_port m_src_axi m_src_axi_bresp bresp Input 2
    add_interface_port m_src_axi m_src_axi_bready bready Output 1
    add_interface_port m_src_axi m_src_axi_arvalid arvalid Output 1
    add_interface_port m_src_axi m_src_axi_araddr araddr Output 32
    add_interface_port m_src_axi m_src_axi_arready arready Input 1
    add_interface_port m_src_axi m_src_axi_rvalid rvalid Input 1
    add_interface_port m_src_axi m_src_axi_rresp rresp Input 2
    add_interface_port m_src_axi m_src_axi_rdata rdata Input DMA_DATA_WIDTH_SRC
    add_interface_port m_src_axi m_src_axi_rready rready Output 1
    add_interface_port m_src_axi m_src_axi_awlen awlen Output 8
    add_interface_port m_src_axi m_src_axi_awsize awsize Output 3
    add_interface_port m_src_axi m_src_axi_awburst awburst Output 2
    add_interface_port m_src_axi m_src_axi_awcache awcache Output 4
    add_interface_port m_src_axi m_src_axi_awprot awprot Output 3
    add_interface_port m_src_axi m_src_axi_wlast wlast Output 1
    add_interface_port m_src_axi m_src_axi_arlen arlen Output 8
    add_interface_port m_src_axi m_src_axi_arsize arsize Output 3
    add_interface_port m_src_axi m_src_axi_arburst arburst Output 2
    add_interface_port m_src_axi m_src_axi_arcache arcache Output 4
    add_interface_port m_src_axi m_src_axi_arprot arprot Output 3
  }

  # axis destination/source

  if {[get_parameter_value DMA_TYPE_DEST] == 1} {


    ad_alt_intf clock   m_axis_aclk       input   1                       clk
    ad_alt_intf signal  m_axis_valid      output  1                       valid
    ad_alt_intf signal  m_axis_data       output  DMA_DATA_WIDTH_DEST     data
    ad_alt_intf signal  m_axis_ready      input   1                       ready
    ad_alt_intf signal  m_axis_last       output  1                       last
    ad_alt_intf signal  m_axis_xfer_req   output  1                       xfer_req

  }

  if {[get_parameter_value DMA_TYPE_SRC] == 1} {

    ad_alt_intf clock   s_axis_aclk       input   1                       clk
    ad_alt_intf signal  s_axis_valid      input   1                       valid
    ad_alt_intf signal  s_axis_data       input   DMA_DATA_WIDTH_SRC      data
    ad_alt_intf signal  s_axis_ready      output  1                       ready
    ad_alt_intf signal  s_axis_xfer_req   output  1                       xfer_req
    ad_alt_intf signal  s_axis_user       input   1                       user

    set_port_property s_axis_user termination true
    set_port_property s_axis_user termination_value 1

  }

  # fifo destination/source

  if {[get_parameter_value DMA_TYPE_DEST] == 2} {
    ad_alt_intf clock   fifo_rd_clk       input   1                       clk
    ad_alt_intf signal  fifo_rd_en        input   1                       valid
    ad_alt_intf signal  fifo_rd_valid     output  1                       valid
    ad_alt_intf signal  fifo_rd_dout      output  DMA_DATA_WIDTH_DEST     data
    ad_alt_intf signal  fifo_rd_underflow output  1                       unf
    ad_alt_intf signal  fifo_rd_xfer_req  output  1                       xfer_req
  }

  if {[get_parameter_value DMA_TYPE_SRC] == 2} {
    ad_alt_intf clock   fifo_wr_clk       input   1                       clk
    ad_alt_intf signal  fifo_wr_en        input   1                       valid
    ad_alt_intf signal  fifo_wr_din       input   DMA_DATA_WIDTH_SRC      data
    ad_alt_intf signal  fifo_wr_overflow  output  1                       ovf
    ad_alt_intf signal  fifo_wr_sync      input   1                       sync
    ad_alt_intf signal  fifo_wr_xfer_req  output  1                       xfer_req
  }
}

