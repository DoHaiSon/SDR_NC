/***************************************************************************//**
 *   @file   AD9548_cfg.h
 *   @brief  Header file of AD9548 Driver Configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#ifndef __AD9548_CFG_H__
#define __AD9548_CFG_H__

#define AD9548_SPI_3_WIRE 1

struct ad9548_reference_spec ad9548_references[] =  
{
	{ //Ref A
		0,          //ref_num
		30000000,   //ref_freq
		0,          //power_down_en
		0x00,       //logic_family
		1,          //manual_profile_en
		0,          //manual_profile
	}
};

struct ad9548_channel_spec ad9548_channels[] = 
{
    {    
        0,      //channel_num
        0x03,   //channel_divider
        0,      //output_dis
        0,      //low_power_mode_en
        0,      //low_power_sync_en
        0x04,   //driver_mode
        0,      //cmos_mode_phase_invert
		0,      //polarity_invert_en
        0,      //drive_strength
        0,      //sync_ignore_en
        "CH0_OUTPUT"    //extended_name
    },
    {    
        1,      //channel_num
        0x00,   //channel_divider
        0,      //output_dis
        0,      //low_power_mode_en
        0,      //low_power_sync_en
        0x00,   //driver_mode
		0,      //cmos_mode_phase_invert
        0,      //polarity_invert_en
        1,      //drive_strength
        0,      //sync_ignore_en
        "CH1_OUTPUT"    //extended_name
    }
};

struct ad9548_platform_data ad9548_pdata_lpc = 
{
    /* System clock settings */
    0,          //sys_clk_ext_loop_filter_en
    0,          //sys_clk_charge_pump_manual_mode_en
    0,          //sys_clk_pll_lock_detect_timer_dis
    0x03,       //sys_clk_charge_pump_current
    0x00,       //sys_clk_pll_lock_detect_timer
    0x28,       //sys_clk_fedback_div
    0,          //sys_clk_m_div
    0,          //sys_clk_2x_mul_en
    1,          //sys_clk_pll_en
    1,          //sys_clk_source
    0x13de43,   //sys_clk_period
    0x000001,   //sys_clk_stability
    
    /* DPLL Setting */
    0x29,       //dpll_tunning_word0
    0x5c,       //dpll_tunning_word1
    0x8f,       //dpll_tunning_word2
    0xc2,       //dpll_tunning_word3
    0xf5,       //dpll_tunning_word4
	0x28,       //dpll_tunning_word5
    0x000000,   //dpll_pull_in_range_limit_low
    0xffffff,   //dpll_pull_in_range_limit_high
    0x00,       //dpll_dds_phase_offset
    0x0000,     //dpll_closed_loop_phase_lock_offset_low
	0x0000,     //dpll_closed_loop_phase_lock_offset_high
    0x03e8,     //dpll_incremental_phase_lock_offset
    0x0000,     //dpll_phase_slew_limit
    0x7530,     //dpll_history_acc_timer
    0x00,       //dpll_history_mode
    120000000,  //dpll_out_freq

    /* Clock distribution synchronization */
    0,          //clock_distr_ext_resistor
    0,          //clock_distr_high_freq_mode
    0x00,       //clock_distr_sync_source
    0x02,       //clock_distr_automatic_sync_mode
   
    /* General settings */    
    0x00,       //watchdog_timer
    0x1ff,      //aux_dac_full_scale_current
    0,          //aux_dac_shutdown
    0x01,       //ref_profile_sel_ssm_startup

    /* Output Channel Configuration */
    2,          //num_channels
    ad9548_channels, //channels

    /* Reference inputs */
    1,          //num_references
    ad9548_references, //references

    /* Reference profiles */
    0,          //num_ref_profiles
    0,          //ref_profiles

    "ad9548-lpc" //name
};

static const uint16_t ad9548_profiles[][2] = {
        {0x0600, 0x00}, /* Profiles are 0x0600-0x07FF */
        {0x0601, 0x55}, /* Profile 0 */
        {0x0602, 0xA0}, /* 30MHz input from FPGA, 122.880MHz output clock */
        {0x0603, 0xFC},
        {0x0604, 0x01},
        {0x0605, 0x00},
        {0x0606, 0x00},
        {0x0607, 0x00},
        {0x0608, 0xE8},
        {0x0609, 0x03},
        {0x060A, 0x00},
        {0x060B, 0xE8},
        {0x060C, 0x03},
        {0x060D, 0x00},
        {0x060E, 0x88},
        {0x060F, 0x13},
        {0x0610, 0x88},
        {0x0611, 0x13},
        {0x0612, 0x0E},
        {0x0613, 0xB2},
        {0x0614, 0x08},
        {0x0615, 0x82},
        {0x0616, 0x62},
        {0x0617, 0x42},
        {0x0618, 0xD8},
        {0x0619, 0x47},
        {0x061A, 0x21},
        {0x061B, 0xCB},
        {0x061C, 0xC4},
        {0x061D, 0x05},
        {0x061E, 0x7F},
        {0x061F, 0x00},
        {0x0620, 0x00},
        {0x0621, 0x00},
        {0x0622, 0x0B},
        {0x0623, 0x02},
        {0x0624, 0x00},
        {0x0625, 0x00},
        {0x0626, 0x26},
        {0x0627, 0xB0},
        {0x0628, 0x00},
        {0x0629, 0x10},
        {0x062A, 0x27},
        {0x062B, 0x20},
        {0x062C, 0x44},
        {0x062D, 0xF4},
        {0x062E, 0x01},
        {0x062F, 0x00},
        {0x0630, 0x20},
        {0x0631, 0x44},
};

#endif // __AD9548_CFG_H__
