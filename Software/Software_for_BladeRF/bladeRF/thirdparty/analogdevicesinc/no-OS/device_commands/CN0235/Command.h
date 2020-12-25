/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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

#ifndef __COMMAND_H__
#define __COMMAND_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define NULL        ((void *)0)
#define SUCCESS      0
#define ERROR       -1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef void (*cmdFunction)(double* param, char paramNo);

struct cmd_info {
    char* name;
    char* description;
    char* acceptedValue;
    char* example;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initializes the device. */
char DoDeviceInit(void);

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Displays the output ADC code of one channel from selected device. */
void GetAdcCode(double* param, char paramNo);

/* Displays the input voltage of one channel from selected device. */
void GetVoltage(double* param, char paramNo);

/* Displays the content of one register from selected device. */
void GetRegister(double* param, char paramNo);

/* Sets the content of one register from selected device. */
void SetRegister(double* param, char paramNo);

/* Performs the self test for both AD7280A devices on the board (one master and
 one slave). */
void DoSelfTestAD7280A(double* param, char paramNo);

/* Reads the status of Alert Pin from AD7280A. */
void GetAlertPinAD7280A(double* param, char paramNo);

/* Enables/disables the AD8280 device. */
void SetEnableAD8280(double* param, char paramNo);

/* Reads the status of Overvoltage Alarm Pin from AD8280. */
void GetAlarmPinOvAD8280(double* param, char paramNo);

/* Reads the status of Undervoltage Alarm Pin from AD8280. */
void GetAlarmPinUvAD8280(double* param, char paramNo);

/* Performs the self test for both AD8280 devices on the board (one master and
 one slave). */
void DoSelfTestAD8280(double* param, char paramNo);

#endif  // __COMMAND_H__
