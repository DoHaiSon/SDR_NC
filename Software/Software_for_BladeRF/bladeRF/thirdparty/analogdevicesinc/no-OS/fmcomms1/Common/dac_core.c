/**************************************************************************//**
*   @file   dac_core.c
*   @brief  DAC core functions implementation.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "dac_core.h"
#include "xparameters.h"
#include "xil_io.h"
#include "test.h"

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
static uint32_t dacCoreAxiAddr;

/**************************************************************************//**
* @brief Initializes the DAC Core.
*
* @param fmcPort - Set to 0 for LPC, set to 1 for HPC
******************************************************************************/
void DAC_Core_Init(uint32_t fmcPort)
{
#ifdef XPAR_AXI_DAC_4D_2C_1_BASEADDR
	dacCoreAxiAddr = fmcPort == 0 ? CFAD9122_0_BASEADDR : CFAD9122_1_BASEADDR;
#else
	dacCoreAxiAddr = CFAD9122_0_BASEADDR;
#endif
}

/**************************************************************************//**
* @brief Reads data from the DAC Core.
*
* @param regAddr - Address of the DAC Core register to be read.
* @param data    - Buffer to store the register content.
******************************************************************************/
void DAC_Core_Read(uint32_t regAddr, uint32_t *data)
{
    *data = Xil_In32(dacCoreAxiAddr + 0x4000 + regAddr);
}

/**************************************************************************//**
* @brief Writes data to the DAC Core.
*
* @param regAddr - Address of the DAC Core register to be written.
* @param data    - Buffer to store the register content to be transmitted.
******************************************************************************/
void DAC_Core_Write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(dacCoreAxiAddr + 0x4000 + regAddr, data);
}
