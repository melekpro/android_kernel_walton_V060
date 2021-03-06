/*
 * Fake LCM
 * Tricking the LK to load the kernel,
 * with a broken and fake lcm.
 * And letting the LK to handle the lcm.
 * This only works if you'r LK have the lcm that works with your device.
 * Parthib24 <sirajammunirparthi24@gmail.com>
 */

#include <linux/string.h>
#include <mt-plat/mt_gpio.h>
#include "lcm_drv.h"

#define FRAME_WIDTH  	(720)
#define FRAME_HEIGHT 	(1280)

#define LCM_ID          (0x94)

/* We need these hooks to fix lcm detection error in lk */

static LCM_UTIL_FUNCS lcm_util = {0};

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	params->type = LCM_TYPE_DSI;
	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

  params->dsi.mode = BURST_VDO_MODE;


	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;
	// Highly depends on LCD driver capability.

	params->dsi.intermediat_buffer_num = 0;	//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage


	// Video mode setting
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.word_count = 720 * 3;

    params->dsi.vertical_sync_active				= 2;
    params->dsi.vertical_backporch					= 11;
    params->dsi.vertical_frontporch					= 14;
    params->dsi.vertical_active_line                = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active              = 8;
    params->dsi.horizontal_backporch				= 80;
    params->dsi.horizontal_frontporch				= 80;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	/***********************/
  params->dsi.PLL_CLOCK = 221;
#ifdef VANZO_LCM_ESD_CHECK_SUPPORT
    params->dsi.noncont_clock = TRUE;
	params->dsi.noncont_clock_period =1;
  /***************************/
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
#endif
}

static void lcm_init(void)
{

	
}

/* LCM Driver Hooks */
LCM_DRIVER r69339_sampl = 
{
	.name		= "r69339_sampl",
	.set_util_funcs = lcm_set_util_funcs, 
	.get_params     = lcm_get_params,
	.init           = lcm_init,
//	.suspend        = lcm_suspend,
//	.resume         = lcm_resume,	
//	.compare_id     = lcm_compare_id,	
//	.esd_check      = lcm_esd_check,	
//	.esd_recover    = lcm_esd_recover,	
//	.update         = lcm_update,
};

