/***************************************************************************//**
* @file ad5766_sdz.c
* @brief Implementation of Main Function.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2016(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xil_cache.h>
#include <xparameters.h>
#include "platform_drivers.h"
#include "spi_engine.h"
#include "ad5766_core.h"
#include "ad5766.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define GPIO_DEVICE_ID		XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET			54 + 32
#define GPIO_RESET			GPIO_OFFSET + 0

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

 	ad5766_init_param default_init_param = {
		/* SPI */
		0,					// spi_chip_select
		SPI_MODE_1,			// spi_mode
		SPI_ENGINE,			// spi_type
		0,					// spi_device_id
		/* GPIO */
		PS7_GPIO,			// gpio_type
		GPIO_DEVICE_ID,		// gpio_device_id
		GPIO_RESET,			// gpio_reset
		/* Device Settings */
		AD5766_DISABLE,		// daisy_chain_en
		AD5766_ZERO,		// clr
		AD5766_M_10V_TO_P_10V,	// span
		0,					// pwr_dac_setting
		0,					// pwr_dither_setting
		0,					// dither_signal_setting
		0,					// inv_dither_setting
		0,					// dither_scale_setting
	};
	ad5766_core_init_param default_core_init_param = {
		XPAR_SPI_AXI_AD5766_BASEADDR,		// core_baseaddr
		XPAR_AXI_AD5766_DAC_DMA_BASEADDR,	// dma_baseaddr
		XPAR_DDR_MEM_BASEADDR + 0xA000000,	// dma_source_addr
		800000,								// rate_hz
		50000000,							// spi_clk_hz
	};
	ad5766_dev *dev;
	ad5766_core *core;

	ad5766_setup(&dev, default_init_param);

	ad5766_core_setup(&core, default_core_init_param);

	xil_printf("Done\n");

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
