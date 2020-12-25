/***************************************************************************//**
 * @file adc_core.c
 * @brief Implementation of ADC Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2015(c) Analog Devices, Inc.
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
#include "adc_core.h"

/***************************************************************************//**
 * @brief adc_read
 *******************************************************************************/
int32_t adc_read(adc_core core,
		uint32_t reg_addr,
		uint32_t *reg_data)
{
	*reg_data = ad_reg_read((core.base_address + reg_addr));

	return 0;
}

/***************************************************************************//**
 * @brief adc_write
 *******************************************************************************/
int32_t adc_write(adc_core core,
		uint32_t reg_addr,
		uint32_t reg_data)
{
	ad_reg_write((core.base_address + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief adc_setup
 *******************************************************************************/
int32_t adc_setup(adc_core core)
{
	uint8_t	 index;
	uint32_t reg_data;
	uint32_t adc_clock;

	adc_read(core, ADC_REG_ID, &reg_data);
	if (reg_data)
		core.master = 1;
	else
		core.master = 0;

	adc_write(core, ADC_REG_RSTN, 0);
	adc_write(core, ADC_REG_RSTN, ADC_MMCM_RSTN | ADC_RSTN);

	for(index = 0; index < core.no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_CNTRL(index), 0x51);
	}

	mdelay(100);

	adc_read(core, ADC_REG_STATUS, &reg_data);
	if(reg_data == 0x0) {
		ad_printf("%s adc core Status errors.\n", __func__);
		return -1;
	}

	adc_read(core, ADC_REG_CLK_FREQ, &adc_clock);
	adc_read(core, ADC_REG_CLK_RATIO, &reg_data);
	adc_clock = (adc_clock * reg_data * 100) + 0x7fff;
	adc_clock = adc_clock >> 16;

	ad_printf("%s adc core initialized (%d MHz).\n", __func__, adc_clock);

	return 0;
}

/***************************************************************************//**
 * @brief adc_set_pnsel
 *******************************************************************************/
int32_t adc_set_pnsel(adc_core core,
		uint8_t channel,
		enum adc_pn_sel sel)
{
	uint32_t reg;

	adc_read(core, ADC_REG_CHAN_CNTRL_3(channel), &reg);
	reg &= ~ADC_ADC_PN_SEL(~0);
	reg |= ADC_ADC_PN_SEL(sel);
	adc_write(core, ADC_REG_CHAN_CNTRL_3(channel), reg);

	return 0;
}

/***************************************************************************//**
 * @brief adc_pn_mon
 *******************************************************************************/
int32_t adc_pn_mon(adc_core core,
		enum adc_pn_sel sel)
{
	uint8_t	index;
	uint32_t reg_data;
	int32_t pn_errors = 0;

	for (index = 0; index < core.no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_CNTRL(index), ADC_ENABLE);
		adc_set_pnsel(core, index, sel);
	}
	mdelay(1);

	for (index = 0; index < core.no_of_channels; index++) {
		adc_write(core, ADC_REG_CHAN_STATUS(index), 0xff);
	}
	mdelay(100);

	for (index = 0; index < core.no_of_channels; index++) {
		adc_read(core, ADC_REG_CHAN_STATUS(index), &reg_data);
		if (reg_data != 0) {
			pn_errors = -1;
			ad_printf("%s ERROR: adc pn OUT OF SYNC: %d, %d, 0x%02x!\n", __func__, index, sel, reg_data);
		}
	}

	return pn_errors;
}

/***************************************************************************//**
 * @brief adc_ramp_test
 *******************************************************************************/
int32_t adc_ramp_test(adc_core core,
		uint32_t no_of_samples,
		uint32_t start_address)
{
	uint8_t err_cnt = 0;
	uint32_t exp_data;
	uint32_t rcv_data;
	uint32_t index;

	// FIXME
	for (index = 0; index < no_of_samples; index++) {
		rcv_data = ad_reg_read(start_address + (index*4)) & 0x3fff3fff;
		if (index == 0)
			exp_data = rcv_data;
		if (rcv_data != exp_data) {
			ad_printf("%s Capture Error[%d]: rcv(%08x) exp(%08x).\n",
					__func__, index, rcv_data, exp_data);
			if (exp_data == 10)
				break;
			exp_data++;
		}
		if ((exp_data & 0x3fff) == 0x3fff)
			exp_data = exp_data & 0xffff0000;
		else
			exp_data = exp_data + 0x00000001;
		if ((exp_data & 0x3fff0000) == 0x3fff0000)
			exp_data = exp_data & 0x0000ffff;
		else
			exp_data = exp_data + 0x00010000;
	}

	if (err_cnt)
		return -1;
	else
		return 0;
}
