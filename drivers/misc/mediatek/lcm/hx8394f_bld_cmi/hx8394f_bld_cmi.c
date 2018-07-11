/*
 * No Warranty
 * Parthib24 <sirajammunirparthi24@gmail.com>
 */

#ifdef BUILD_LK
#else
    #include <linux/string.h>
    #if defined(BUILD_UBOOT)
        #include <asm/arch/mt_gpio.h>
    #else
        #include <mt-plat/mt_gpio.h>
    #endif
#endif
#include "lcm_drv.h"

/* Local Constants */

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define LCM_ID                      						(0x94)

#define REGFLAG_DELAY             								(0XFE)
#define REGFLAG_END_OF_TABLE      								(0x100)	// END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

/* Local Variables */

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    									(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 										(lcm_util.udelay(n))
#define MDELAY(n) 										(lcm_util.mdelay(n))

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

/* Local Functions */
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* LCM Driver Implementations */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	params->type   = LCM_TYPE_DSI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode 			= LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

	params->dsi.mode   = SYNC_PULSE_VDO_MODE;	//SYNC_PULSE_VDO_MODE;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine. 
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST; 
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;
	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;
	// Video mode setting		
	params->dsi.intermediat_buffer_num 	= 2;
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
        params->dsi.word_count=720 * 3;
	params->dsi.vertical_sync_active				= 4;//5
	params->dsi.vertical_backporch					= 12;
	params->dsi.vertical_frontporch					= 15;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 
	params->dsi.horizontal_sync_active				= 40;
	params->dsi.horizontal_backporch				= 40;//66
	params->dsi.horizontal_frontporch				= 40;//70
	//params->dsi.horizontal_blanking_pixel		       		= 60;
	params->dsi.horizontal_active_pixel		       		= FRAME_WIDTH;
	// Bit rate calculation

	params->dsi.PLL_CLOCK=205;//227;//254;//254//247  240
}

static unsigned int lcm_init_resgister(void)
{
     unsigned int data_array[16];
  
    data_array[0] = 0x00043902;
    data_array[1] = 0x9483ffb9;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0x00033902;
    data_array[1] = 0x8372ba;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0x103902;
    data_array[1] = 0x11116cb1;
    data_array[2] = 0xF1110434;
    data_array[3] = 0x239EE280;
    data_array[4] = 0x58D2C080;
    dsi_set_cmdq(data_array, 5, 1);
    MDELAY(1);
    
    data_array[0] = 0xC3902;
    data_array[1] = 0x564C0B2;
    data_array[2] = 0x81C3207;
    data_array[3] = 0x4D1C08;
    dsi_set_cmdq(data_array, 4, 1);
    MDELAY(1);

    data_array[0] = 0x23902;
    data_array[1] = 0x7BC;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
   
    data_array[0] = 0x43902;
    data_array[1] = 0x10E41BF;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0xD3902;
    data_array[1] = 0x3FF00B4;
    data_array[2] = 0x35A035A;
    data_array[3] = 0x16A015A;
    data_array[4] = 0x6A;
    dsi_set_cmdq(data_array, 5, 1);
    MDELAY(1);
   
    data_array[0] = 0x1F3902;
    data_array[1] = 0x100740;
    data_array[2] = 0x4153200;
    data_array[3] = 0x15320405;
    data_array[4] = 0x37270528;
    data_array[5] = 0x370B0033;
    data_array[6] = 0x70710;
    data_array[7] = 0xD3;
    data_array[8] = 0x1032;
    dsi_set_cmdq(data_array, 9, 1);
    MDELAY(1);

    data_array[0] = 0x2D3902;
    data_array[1] = 0x181818D5;
    data_array[2] = 0x18181818;
    data_array[3] = 0x18181818;
    data_array[4] = 0x25181818;
    data_array[5] = 0x18262724;
    data_array[6] = 0x1040518;
    data_array[7] = 0x3060700;
    data_array[8] = 0x47424302;
    data_array[9] = 0x41444546;
    data_array[10] = 0x23202140;
    data_array[11] = 0x18181822;
    data_array[12] = 0x18;
    dsi_set_cmdq(data_array, 13, 1);
    MDELAY(1);

    data_array[0] = 0x23902;
    data_array[1] = 0x9CC;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
   
    data_array[0] = 0x33902;
    data_array[1] = 0x1430C0;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);    
      
    data_array[0] = 0x33902;
    data_array[1] = 0x3D3DB6;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0x2B3902;
    data_array[1] = 0x211A05E0;
    data_array[2] = 0x303F3735;
    data_array[3] = 0xC0A074A;
    data_array[4] = 0x17151017;
    data_array[5] = 0x11061514;
    data_array[6] = 0x17051714;
    data_array[7] = 0x3F39381B;
    data_array[8] = 0xB074F2D;
    data_array[9] = 0x120F180D;
    data_array[10] = 0xB131214;
    data_array[11] = 0x191716;
    dsi_set_cmdq(data_array, 12, 1);
    MDELAY(1);

    data_array[0] = 0x53902;
    data_array[1] = 0x40C000C7;
    data_array[2] = 0xC0;
    dsi_set_cmdq(data_array, 3, 1);
    MDELAY(1);
   
    data_array[0] = 0x110500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(150);
    
    data_array[0] = 0x290500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);
    return 0;
};


static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    lcm_init_resgister();
}

static void lcm_suspend(void)
{
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(30);
	SET_RESET_PIN(1);
	MDELAY(120);
}

static unsigned int lcm_compare_id(void);

static void lcm_resume(void)
{
	lcm_init();
}

/* Get LCM Driver Hooks */
LCM_DRIVER hx8394d_bld_cmi = 
{
	.name			= "hx8394d_bld_cmi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
//	.compare_id     = lcm_compare_id,	
//	.esd_check      = lcm_esd_check,	
//	.esd_recover    = lcm_esd_recover,	
//	.update         = lcm_update,
};
#ifdef BUILD_LK
#else
    #include <linux/string.h>
    #if defined(BUILD_UBOOT)
        #include <asm/arch/mt_gpio.h>
    #else
        #include <mt-plat/mt_gpio.h>
    #endif
#endif
#include "lcm_drv.h"

/* Local Constants */

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define LCM_ID                      						(0x94)

#define REGFLAG_DELAY             								(0XFE)
#define REGFLAG_END_OF_TABLE      								(0x100)	// END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

/* Local Variables */

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    									(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 										(lcm_util.udelay(n))
#define MDELAY(n) 										(lcm_util.mdelay(n))

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

/* Local Functions */
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* LCM Driver Implementations */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	params->type   = LCM_TYPE_DSI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode 			= LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

	params->dsi.mode   = SYNC_PULSE_VDO_MODE;	//SYNC_PULSE_VDO_MODE;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine. 
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST; 
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;
	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;
	// Video mode setting		
	params->dsi.intermediat_buffer_num 	= 2;
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
        params->dsi.word_count=720 * 3;
	params->dsi.vertical_sync_active				= 4;//5
	params->dsi.vertical_backporch					= 12;
	params->dsi.vertical_frontporch					= 15;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 
	params->dsi.horizontal_sync_active				= 40;
	params->dsi.horizontal_backporch				= 40;//66
	params->dsi.horizontal_frontporch				= 40;//70
	//params->dsi.horizontal_blanking_pixel		       		= 60;
	params->dsi.horizontal_active_pixel		       		= FRAME_WIDTH;
	// Bit rate calculation

	params->dsi.PLL_CLOCK=205;//227;//254;//254//247  240
}

static unsigned int lcm_init_resgister(void)
{
     unsigned int data_array[16];
  
    data_array[0] = 0x00043902;
    data_array[1] = 0x9483ffb9;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0x00033902;
    data_array[1] = 0x8372ba;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0x103902;
    data_array[1] = 0x11116cb1;
    data_array[2] = 0xF1110434;
    data_array[3] = 0x239EE280;
    data_array[4] = 0x58D2C080;
    dsi_set_cmdq(data_array, 5, 1);
    MDELAY(1);
    
    data_array[0] = 0xC3902;
    data_array[1] = 0x564C0B2;
    data_array[2] = 0x81C3207;
    data_array[3] = 0x4D1C08;
    dsi_set_cmdq(data_array, 4, 1);
    MDELAY(1);

    data_array[0] = 0x23902;
    data_array[1] = 0x7BC;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
   
    data_array[0] = 0x43902;
    data_array[1] = 0x10E41BF;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0xD3902;
    data_array[1] = 0x3FF00B4;
    data_array[2] = 0x35A035A;
    data_array[3] = 0x16A015A;
    data_array[4] = 0x6A;
    dsi_set_cmdq(data_array, 5, 1);
    MDELAY(1);
   
    data_array[0] = 0x1F3902;
    data_array[1] = 0x100740;
    data_array[2] = 0x4153200;
    data_array[3] = 0x15320405;
    data_array[4] = 0x37270528;
    data_array[5] = 0x370B0033;
    data_array[6] = 0x70710;
    data_array[7] = 0xD3;
    data_array[8] = 0x1032;
    dsi_set_cmdq(data_array, 9, 1);
    MDELAY(1);

    data_array[0] = 0x2D3902;
    data_array[1] = 0x181818D5;
    data_array[2] = 0x18181818;
    data_array[3] = 0x18181818;
    data_array[4] = 0x25181818;
    data_array[5] = 0x18262724;
    data_array[6] = 0x1040518;
    data_array[7] = 0x3060700;
    data_array[8] = 0x47424302;
    data_array[9] = 0x41444546;
    data_array[10] = 0x23202140;
    data_array[11] = 0x18181822;
    data_array[12] = 0x18;
    dsi_set_cmdq(data_array, 13, 1);
    MDELAY(1);

    data_array[0] = 0x23902;
    data_array[1] = 0x9CC;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
   
    data_array[0] = 0x33902;
    data_array[1] = 0x1430C0;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);    
      
    data_array[0] = 0x33902;
    data_array[1] = 0x3D3DB6;
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(1);
    
    data_array[0] = 0x2B3902;
    data_array[1] = 0x211A05E0;
    data_array[2] = 0x303F3735;
    data_array[3] = 0xC0A074A;
    data_array[4] = 0x17151017;
    data_array[5] = 0x11061514;
    data_array[6] = 0x17051714;
    data_array[7] = 0x3F39381B;
    data_array[8] = 0xB074F2D;
    data_array[9] = 0x120F180D;
    data_array[10] = 0xB131214;
    data_array[11] = 0x191716;
    dsi_set_cmdq(data_array, 12, 1);
    MDELAY(1);

    data_array[0] = 0x53902;
    data_array[1] = 0x40C000C7;
    data_array[2] = 0xC0;
    dsi_set_cmdq(data_array, 3, 1);
    MDELAY(1);
   
    data_array[0] = 0x110500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(150);
    
    data_array[0] = 0x290500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);
    return 0;
};


static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    lcm_init_resgister();
}

static void lcm_suspend(void)
{
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(30);
	SET_RESET_PIN(1);
	MDELAY(120);
}

static unsigned int lcm_compare_id(void);

static void lcm_resume(void)
{
	lcm_init();
}

/* Get LCM Driver Hooks */
LCM_DRIVER hx8394d_bld_cmi = 
{
	.name			= "hx8394d_bld_cmi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
//	.compare_id     = lcm_compare_id,	
//	.esd_check      = lcm_esd_check,	
//	.esd_recover    = lcm_esd_recover,	
//	.update         = lcm_update,
};

