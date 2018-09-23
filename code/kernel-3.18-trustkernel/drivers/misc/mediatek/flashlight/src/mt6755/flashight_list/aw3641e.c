
#include <linux/platform_device.h>
#include "flashlight_list.h"
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/hct_include/hct_project_all_config.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#define PK_DBG(fmt, arg...)    pr_debug(CONFIG_HCT_FLASHLIGHT"%s: "fmt, __func__ , ##arg)

struct gpio_desc *fl_en_gpio = NULL;
struct gpio_desc *fl_mode_gpio = NULL;

#ifndef __HCT_GPIO_FLASHLIGHT_EN_NUM__
	#error "__HCT_GPIO_FLASHLIGHT_EN_NUM__ is not defined in hct_board_config.h"
#endif
#ifndef __HCT_GPIO_FLASHLIGHT_MODE_NUM__
	#error "_HCT_GPIO_FLASHLIGHT_MODE_NUM__ is not defined in hct_board_config.h"
#endif

static	int fl_init(struct platform_device *dev){

	int ret = 0;
	fl_en_gpio = gpio_to_desc(__HCT_GPIO_FLASHLIGHT_EN_NUM__);
	fl_mode_gpio = gpio_to_desc(__HCT_GPIO_FLASHLIGHT_MODE_NUM__);

	ret = gpiod_direction_output(fl_en_gpio, 1);
	if(ret)
		PK_DBG("flashlight gpiod_direction_output failed,gpio_num: %d\n",__HCT_GPIO_FLASHLIGHT_EN_NUM__);
	ret = gpiod_direction_output(fl_mode_gpio, 1);
	if(ret)
		PK_DBG("flashlight gpiod_direction_output failed,gpio_num: %d\n",__HCT_GPIO_FLASHLIGHT_MODE_NUM__);
	gpiod_set_value(fl_en_gpio, 0);
	gpiod_set_value(fl_mode_gpio, 0);
	return 0;
}


static	int fl_enable(int g_duty){
	int i;
	if(g_duty){
		gpiod_set_value(fl_mode_gpio, 1);
		for(i=0; i<9; i++)
		{
			gpiod_set_value(fl_en_gpio, 0);
			udelay(2);
			gpiod_set_value(fl_en_gpio, 1);
			udelay(2);
		}
	}else{
		gpiod_set_value(fl_mode_gpio, 0);
		gpiod_set_value(fl_en_gpio, 1);
	}
	return 0;
}

static	int fl_disable(void){
	gpiod_set_value(fl_en_gpio, 0);
	gpiod_set_value(fl_mode_gpio, 0);
	mdelay(1);
return 0;
}


flashlight_list_t flashlight_list = {
	.fl_init = fl_init,
	.fl_enable = fl_enable,
	.fl_disable = fl_disable,
	.fl_name = CONFIG_HCT_FLASHLIGHT
};