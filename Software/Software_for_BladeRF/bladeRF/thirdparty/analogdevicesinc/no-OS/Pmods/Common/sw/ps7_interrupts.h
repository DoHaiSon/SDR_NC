/**************************************************************************//**
*   @file   ps7_interrupts.h
*   @brief  PS7 interrupts configuration header file
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
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
*   SVN Revision: 468
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "system_config.h"

#ifndef PS7_INTERRUPTS_H_
#define PS7_INTERRUPTS_H_

#if(USE_PS7 == 1)
	// Interrupt controller ID
	#define INTC_DEVICE_ID	XPAR_SCUGIC_SINGLE_DEVICE_ID

	// PS7 UART definitions
	#define UART_BASEADDR	XPAR_PS7_UART_1_BASEADDR
	#define UART_DEVICE_ID	XPAR_PS7_UART_1_DEVICE_ID
	#define UART_INTERRUPT	XPAR_XUARTPS_1_INTR

	// PS7 Timer definitions
	#define TIMER_BASEADDR	XPS_TTC0_BASEADDR
	#define TIMER_INTERRUPT XPS_TTC0_0_INT_ID

	// LEDS
	#define LED_BASEADDR    XPS_GPIO_BASEADDR

	// External
	#define EXT_BASEADDR	0

	// Function prototypes
	void Ps7IntcInit(void);
	void Ps7UartIntrEnable(void);
	void Ps7UartIntrHandler(void);
	void Ps7TimerIntrHandler(void);
	void Ps7IntrInit(void);
	void Ps7IntrEnable(int intrNr);
	void Ps7IntrDisable(int intrNr);
	char Ps7UartReadChar(void);
	void Ps7ExtIntrEnable(void);
	void Ps7ExtIntrHandler(void);
#endif

#endif /* PS7_INTERRUPTS_H_ */
