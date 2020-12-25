/***************************************************************************//**
 *   @file   AD7303.h
 *   @brief  Header file of AD7303 Driver.
 *   @author Mihai Bancisor(Mihai.Bancisor@analog.com)
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
#ifndef __AD7303_H__
#define __AD7303_H__

/******************************************************************************/
/******************************** AD7303 **************************************/
/******************************************************************************/

/* SPI slave device ID */
#define AD7303_SLAVE_ID     1

/* Control Bits */
#define AD7303_INT       (0 << 7)    // Selects internal reference.
#define AD7303_EXT       (1 << 7)    // Selects external reference.
#define AD7303_LDAC      (1 << 5)    // Load DAC bit.
#define AD7303_PDB       (1 << 4)    // Power-down DAC B.
#define AD7303_PDA       (1 << 3)    // Power-down DAC A.
#define AD7303_A         (0 << 2)    // Address bit to select DAC A.
#define AD7303_B         (1 << 2)    // Address bit to select DAC B.
#define AD7303_CR1       (1 << 1)    // Control Bit 1.
#define AD7303_CR0       (1 << 0)    // Control Bit 0.

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes SPI communication. */
char AD7303_Init(void);

/*! Sends data to AD7303. */
void AD7303_Write(unsigned char controlReg, unsigned char dataReg);

#endif /* __AD7303_H__ */
