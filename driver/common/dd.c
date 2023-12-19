#include "include.h"
#include "arm_arch.h"

#include "dd_pub.h"

#include "sys_ctrl_pub.h"
#include "uart_pub.h"
#include "gpio_pub.h"
#include "icu_pub.h"
#include "wdt_pub.h"
#include "pwm_pub.h"
#include "flash_pub.h"
#include "bk_timer_pub.h"

static DD_INIT_S dd_init_tbl[] =
{
    /* name*/              /* init function*/          /* exit function*/
    {SCTRL_DEV_NAME,        sctrl_init,                 sctrl_exit},
    {ICU_DEV_NAME,          icu_init,                   icu_exit},
    {WDT_DEV_NAME,          wdt_init,                   wdt_exit},
    {GPIO_DEV_NAME,         gpio_init,                  gpio_exit},
    {UART2_DEV_NAME,        uart2_init,                 uart2_exit},
    {UART1_DEV_NAME,        uart1_init,                 uart1_exit},
    {FLASH_DEV_NAME,        flash_init,                 flash_exit},

#if (CFG_SOC_NAME != SOC_BK7231)
    {TIMER_DEV_NAME,        bk_timer_init,              bk_timer_exit},
#endif
    {NULL,                  NULLPTR,                    NULLPTR}
};

void g_dd_init(void)
{
    UINT32 i;
    UINT32 tbl_count;
    DD_INIT_S *dd_element;

    tbl_count = sizeof(dd_init_tbl) / sizeof(DD_INIT_S);
    for(i = 0; i < tbl_count; i ++)
    {
        dd_element = &dd_init_tbl[i];
        if(dd_element->dev_name && dd_element->init)
        {
            (dd_element->init)();
        }
        else
        {
            return;
        }
    }
}

void g_dd_exit(void)
{
    UINT32 i;
    UINT32 tbl_count;
    DD_INIT_S *dd_element;

    tbl_count = sizeof(dd_init_tbl) / sizeof(DD_INIT_S);
    for(i = 0; i < tbl_count; i ++)
    {
        dd_element = &dd_init_tbl[i];
        if(dd_element->dev_name && dd_element->exit)
        {
            (dd_element->exit)();
        }
        else
        {
            return;
        }
    }
}

// EOF
