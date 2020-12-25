/***************************************************************************//**
 *   @file   ad5766.h
 *   @brief  Header file of AD5766 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef AD5766_H_
#define AD5766_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD5766_CMD_NOP_MUX_OUT			0x00
#define AD5766_CMD_SDO_CNTRL			0x01
#define AD5766_CMD_WR_IN_REG(x)			(0x10 | ((x) & 0xF))
#define AD5766_CMD_WR_DAC_REG(x)		(0x20 | ((x) & 0xF))
#define AD5766_CMD_SW_LDAC				0x30
#define AD5766_CMD_SPAN_REG				0x40
#define AD5766_CMD_WR_PWR_DAC			0x50
#define AD5766_CMD_WR_PWR_DITHER		0x51
#define AD5766_CMD_WR_DAC_REG_ALL		0x60
#define AD5766_CMD_SW_FULL_RESET		0x70
#define AD5766_CMD_READBACK_REG(x)		(0x80 | ((x) & 0xF))
#define AD5766_CMD_DITHER_SIG_1			0x90
#define AD5766_CMD_DITHER_SIG_2			0xA0
#define AD5766_CMD_INV_DITHER			0xB0
#define AD5766_CMD_DITHER_SCALE_1		0xC0
#define AD5766_CMD_DITHER_SCALE_2		0xD0

/* AD5766_CMD_SDO_CNTRL */
#define AD5766_SDO_EN					(1 << 0)

/* AD5766_CMD_SW_LDAC */
#define AD5766_LDAC(x)					(1 << ((x) & 0xF))

/* AD5766_CMD_SPAN_REG */
#define AD5766_CFG_CLR(x)				(((x) & 0x3) << 3)
#define AD5766_SPAN(x)					(((x) & 0x7) << 0)

/* AD5766_CMD_WR_PWR_DAC, AD5766_CMD_WR_PWR_DITHER */
#define AD5766_PWDN(x)					(1 << ((x) & 0xF))

/* AD5766_CMD_SW_FULL_RESET */
#define AD5766_RESET					0x1234

/* AD5766_CMD_DITHER_SIG_x */
#define AD5766_N0(x)					(1 << (2 * ((x) & 0xF)))
#define AD5766_N1(x)					(2 << (2 * ((x) & 0xF)))

/* AD5766_CMD_INV_DITHER */
#define AD5766_INV_D(x)					(1 << ((x) & 0xF))

/* AD5766_CMD_DITHER_SCALE_x */
#define AD5766_75(x)					(1 << (2 * ((x) & 0xF)))
#define AD5766_50(x)					(2 << (2 * ((x) & 0xF)))
#define AD5766_25(x)					(3 << (2 * ((x) & 0xF)))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	AD5766_ENABLE,
	AD5766_DISABLE,
} ad5766_state;

typedef enum {
	AD5766_DAC_0,
	AD5766_DAC_1,
	AD5766_DAC_2,
	AD5766_DAC_3,
	AD5766_DAC_4,
	AD5766_DAC_5,
	AD5766_DAC_6,
	AD5766_DAC_7,
	AD5766_DAC_8,
	AD5766_DAC_9,
	AD5766_DAC_10,
	AD5766_DAC_11,
	AD5766_DAC_12,
	AD5766_DAC_13,
	AD5766_DAC_14,
	AD5766_DAC_15,
} ad5766_dac;

typedef enum {
	AD5766_M_20V_TO_0V,
	AD5766_M_16V_TO_0V,
	AD5766_M_10V_TO_0V,
	AD5766_M_12V_TO_P_14V,
	AD5766_M_16V_TO_P_10V,
	AD5766_M_5V_TO_P_6V,
	AD5766_M_10V_TO_P_10V,
} ad5766_span;

typedef enum {
	AD5766_ZERO,
	AD5766_MID,
	AD5766_FULL,
} ad5766_clr;

typedef struct {
	/* SPI */
	spi_device		spi_dev;
	/* GPIO */
	gpio_device		gpio_dev;
	int8_t			gpio_reset;
	/* Device Settings */
	ad5766_state	daisy_chain_en;
} ad5766_dev;

typedef struct {
	/* SPI */
	uint8_t			spi_chip_select;
	spi_mode		spi_mode;
	spi_type		spi_type;
	uint32_t		spi_device_id;
	/* GPIO */
	gpio_type		gpio_type;
	uint32_t		gpio_device_id;
	int8_t			gpio_reset;
	/* Device Settings */
	ad5766_state	daisy_chain_en;
	ad5766_clr		clr;
	ad5766_span		span;
	uint16_t		pwr_dac_setting;
	uint16_t		pwr_dither_setting;
	uint32_t		dither_signal_setting;
	uint16_t		inv_dither_setting;
	uint32_t		dither_scale_setting;
} ad5766_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* SPI command write to device. */
int32_t ad5766_spi_cmd_write(ad5766_dev *dev,
							 uint8_t cmd,
							 uint16_t data);
/* SPI readback register from device. */
int32_t ad5766_spi_readback_reg(ad5766_dev *dev,
								ad5766_dac dac,
								uint32_t *data);
/* Set software LDAC for the selected channels. */
int32_t ad5766_set_sw_ldac(ad5766_dev *dev,
						   uint16_t setting);
/* Set clear code and span settings. */
int32_t ad5766_set_clr_span(ad5766_dev *dev,
							ad5766_clr clr,
							ad5766_span span);
/* Power down the selected channels. */
int32_t ad5766_set_pwr_dac(ad5766_dev *dev,
						   uint16_t setting);
/* Power down the dither block for the selected channels. */
int32_t ad5766_set_pwr_dither(ad5766_dev *dev,
							  uint16_t setting);
/* Enable the dither signal for the selected channels. */
int32_t ad5766_set_dither_signal(ad5766_dev *dev,
								 uint32_t setting);
/* Invert the dither signal for the selected channels. */
int32_t ad5766_set_inv_dither(ad5766_dev *dev,
							  uint16_t setting);
/* Enable the dither scaling for the selected channels. */
int32_t ad5766_set_dither_scale(ad5766_dev *dev,
								uint32_t setting);
/* Do a software reset. */
int32_t ad5766_do_soft_reset(ad5766_dev *dev);
/* Set the input register for the selected channel. */
int32_t ad5766_set_in_reg(ad5766_dev *dev,
						  ad5766_dac dac,
						  uint16_t data);
/* Set the DAC register for the selected channel. */
int32_t ad5766_set_dac_reg(ad5766_dev *dev,
							  ad5766_dac dac,
							  uint16_t data);
/* Set the DAC register for all channels. */
int32_t ad5766_set_dac_reg_all(ad5766_dev *dev,
							   ad5766_dac dac,
							   uint16_t data);
/* Initialize the device. */
int32_t ad5766_setup(ad5766_dev **device,
					 ad5766_init_param init_param);
#endif // AD5766_H_
