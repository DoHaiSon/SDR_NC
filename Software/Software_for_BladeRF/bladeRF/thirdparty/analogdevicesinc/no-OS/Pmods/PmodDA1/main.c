/***************************************************************************//**
*   @file   main.c
*   @brief  Main file for AD7303 driver
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
********************************************************************************
*   SVN Revision: $WCREV$
*******************************************************************************/
#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "ad7303.h"

#define DDR_BASEADDR  		XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define VDMA_BASEADDR 		XPAR_AXI_VDMA_1_BASEADDR
#define PMOD_DATA_BASEADDR	DDR_BASEADDR + 0xA000000
#define CF_BASEADDR			XPAR_AXI_AD7303_0_BASEADDR

int main()
{
	int i = 0;
	int sendValDac0 = 0;
	int sendValDac1 = 0;

	Xil_DCacheFlush();
	Xil_DCacheDisable();

	xil_printf("PmodDA1 Demonstration Project\n\r");

	xil_printf("Preparing DAC Data...");

	for(i = 0; i < 256; i++)
	{
	  sendValDac0 = (0x0300 | (sinetable[i%256] & 0xFF)) << 16;
	  sendValDac1 = (0x0300 | (triangletable[i%256] & 0xFF));

	  Xil_Out32(PMOD_DATA_BASEADDR + i * 0x04, sendValDac0 | sendValDac1);
	}

	xil_printf("Done!\n\r");

	xil_printf("Programming VDMA Core...");

	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000004); // reset
	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000003); // enable circular mode
	while((Xil_In32(VDMA_BASEADDR + 0x04) & 0x01) == 0x01);
	Xil_Out32((VDMA_BASEADDR + 0x018), 0x03); // frm store
	Xil_Out32((VDMA_BASEADDR + 0x05c), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x060), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x064), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x058), 256 * 4); // h offset (2048 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x054), 256 * 4); // h size (1920 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x050), 1); // v size (1080)

	xil_printf("Done!\n\r");

	xil_printf("Enabling AD7303 Core...");

	Xil_Out32(CF_BASEADDR + 0x4040, 0x01);
	Xil_Out32(CF_BASEADDR + 0x4044, 0x01);

	xil_printf("Done!\n\r");


	while(1)
	{
	}

    return 0;
}
