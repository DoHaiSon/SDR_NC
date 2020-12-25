/**************************************************************************//**
*   @file   ad5110.h
*   @brief  Header file of ad5110 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
******************************************************************************/
#ifndef _ad5110_H_
#define _ad5110_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
/*!< ad5110 Device Address */
#define ad5110_I2C_ADDR         0x2F
#define ad5110_1_I2C_ADDR       0x2C

/******************************************************************************/
/************************* Input shift register *******************************/
/******************************************************************************/

/*!< Command position in transmitted bytes */
#define COMMAND                 8
/*!< Available Commands */
#define CMD_NOP                 0
#define CMD_WR_RDAC_EEPROM      1
#define CMD_WR_RDAC             2
#define CMD_SHUT_DOWN           3
#define CMD_RESET               4
#define CMD_RD_RDAC             5
#define CMD_RD_EEPROM           6
/*!< Shutdown modes */
#define SHUT_DOWN_OFF           0
#define SHUT_DOWN_ON            1
/*!< Read modes */
#define WIPER_POSITION          0
#define RESISTOR_TOLERANCE      1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*!< Initializes the communication with the device. */
char ad5110_Init(unsigned char devAddr);

/*!< Write the content of serial register data to RDAC. */
void ad5110_WriteRdac(unsigned char rdacValue);

/*!< Read the content of RDAC register. */
unsigned char ad5110_ReadRdac(void);

/*!< Write the content of RDAC register to EEPROM. */
void ad5110_WriteRdacEeprom(void);

/*!< Read wiper position from EEPROM. */
unsigned char ad5110_ReadWiper(void);

/*!< Read resistor tolerance from EEPROM. */
unsigned char ad5110_ReadResTolerance(void);

/*!< Software reset; makes a refresh of RDAC register with EEPROM. */
void ad5110_Reset(void);

/*!< Software shutdown. */
void ad5110_ShutDown(unsigned char value);

#endif  // _ad5110_H_
