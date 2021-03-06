#include "finger.h"
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/completion.h>
#include <linux/delay.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#endif  

#ifdef CONFIG_OF
static const struct of_device_id fp_of_match[] = {
	{.compatible = "mediatek,hct_finger",},
	{},
};
#endif

#if 0
//------------------------------------------------------------------------------
//GPIO
#define  GPIO_SW_PWR_PIN        GPIO_FIGERPRINT_PWR_EN_PIN
#define  GPIO_SW_PWR_M_GPIO     GPIO_MODE_00

#define  GPIO_SW_IRQ_NUM        CUST_EINT_FIGERPRINT_INT_NUM//4      
#define  GPIO_SW_INT_PIN        GPIO_FIGERPRINT_INT /*GPIO3*/   //(GPIO4 | 0x80000000)

#define  GPIO_SW_RST_PIN        GPIO_FIGERPRINT_RST   /*GPIO19*/   //(GPIO44 | 0x80000000)
#define  GPIO_SW_RST_PIN_M_GPIO GPIO_MODE_00

#define  GPIO_SW_RST_M_DAIPCMOUT   GPIO_MODE_01

#define  GPIO_SW_EINT_PIN_M_GPIO   GPIO_FIGERPRINT_INT_M_GPIO //GPIO_MODE_00
#define  GPIO_SW_EINT_PIN_M_EINT   GPIO_FIGERPRINT_INT_M_EINT //GPIO_MODE_00
#endif
//------------------------------------------------------------------------------
#define SW_DBG(fmt, args...)    printk("sunwave_fp_kernel:" fmt, ##args);

/**
 * this function olny use thread ex.open
 *
 */
int mt6735_reset(struct spi_device**  spi)
{
//    mt_set_gpio_out(GPIO_SW_RST_PIN, 1);
    hct_finger_set_reset(1);
    msleep(10);
    hct_finger_set_reset(0);
    msleep(20);
    hct_finger_set_reset(1);
    return 0;
}

static int  mt6735_gpio_init(struct spi_device**   spi)
{
    sunwave_sensor_t*  sunwave = spi_to_sunwave(spi);
    struct device_node *node = NULL;
    int sw_detect_irq= -1;
    /* set power pin to high */
    hct_waite_for_finger_dts_paser();

    hct_finger_set_power(1);
    msleep(1);

    /*set reset pin to high*/
    hct_finger_set_reset(1);
    SW_DBG("sw_driver_mtk probe\n");


    //irq
    //mt_set_gpio_mode(GPIO_SW_INT_PIN,GPIO_SUNWAVE_IRQ_M_EINT);
    //mt_set_gpio_dir(GPIO_SW_INT_PIN,GPIO_DIR_IN);
    // mt_set_gpio_pull_enable(GPIO_SW_INT_PIN, GPIO_PULL_DISABLE);
    //mt_set_gpio_pull_enable(GPIO_SW_INT_PIN, GPIO_PULL_ENABLE);
    //mt_set_gpio_pull_select(GPIO_SW_INT_PIN, GPIO_PULL_UP);

    hct_finger_set_spi_mode(1); // set to spi mode
    hct_finger_set_eint(1);
    //sunwave->finger->gpio_rst = GPIO_SW_RST_PIN;


 
    node = of_find_matching_node(node, fp_of_match);
 
    if (node){
           sw_detect_irq = irq_of_parse_and_map(node, 0);

    }else{
           printk("fingerprint request_irq can not find fp eint device node!.");
           return -1;
     }

    sunwave->standby_irq = sw_detect_irq;
    //sunwave->standby_irq = gpio_to_irq(sw_detect_irq);

    return 0;
}


static int  mt6735_irq_hander(struct spi_device** spi)
{
    spi_to_sunwave(spi);
    //mt_eint_mask(sunwave->standby_irq); //disable interrupt
    //mt_eint_unmask(sunwave->standby_irq); //enable interrupt
    return 0;
}


static int mt6735_speed(struct spi_device**    spi,unsigned int speed)
{

#define SPI_MODULE_CLOCK   100000000//    134300

struct mt_chip_conf *config;

unsigned int	time = SPI_MODULE_CLOCK/speed;

config=(struct mt_chip_conf *)(*spi)->controller_data;

config->low_time=time/2;
config->high_time=time/2;

if(time%2)
{
config->high_time=config->high_time+1;
}

//(chip_config->low_time + chip_config->high_time);

return 0;
}

static finger_t  finger_sensor = {
    .ver                    = 0,
    .attribute              = DEVICE_ATTRIBUTE_NONE,
    .write_then_read        = 0,
    .irq_hander             = mt6735_irq_hander,
    .irq_request            = 0,
    .irq_free               = 0,
    .init                   = mt6735_gpio_init,
    .reset                  = mt6735_reset,
    .speed		    = mt6735_speed,
};


void mt6735_register_finger(sunwave_sensor_t*    sunwave)
{
    sunwave->finger = &finger_sensor;
}
EXPORT_SYMBOL_GPL(mt6735_register_finger);


void mt6735_register_platform(void)
{
}
EXPORT_SYMBOL_GPL(mt6735_register_platform);
