/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) Beken Corp 2011-2020
 *
 ****************************************************************************************
 */
#include "include.h"
#include "driver_pub.h"
#include "func_pub.h"
#include "start_type_pub.h"
#include "fake_clock_pub.h"
#include "gpio_pub.h"
#include "portmacro.h"
#include "icu_pub.h"
#include "uart_pub.h"

#define APPLICATION_START_ADDR 0x10000

extern int Ymodem_Receive_Main(void);
extern void delay(int num);
extern void ymodem_init();

bool dfu_mode;
int dfu_gpio_port = GPIO36;
volatile bool dfu_complete = false;
uint64_t dfu_complete_tick;
int button_pressed_count = 0;
int gpio_last_value = -1;

void board_dfu_complete(void)
{
    dfu_complete = true;
    dfu_complete_tick = fclk_get_tick();
}

void dfu_gpio_init(void)
{
    uint32_t param;

    param = GPIO_CFG_PARAM(dfu_gpio_port, GMODE_INPUT_PULLUP);

    gpio_ctrl(CMD_GPIO_CFG, &param);
}

void dfu_mode_check(void)
{
    uint32_t ret;
    uint32_t param;

    param = dfu_gpio_port;
    ret = gpio_ctrl(CMD_GPIO_INPUT, &param);

    dfu_mode = !!(ret == 0);
    os_printf("dfu_mode: %d\n", dfu_mode);
}

bool dfu_button_pressed()
{
    uint32_t ret;
    uint32_t param;

    param = dfu_gpio_port;
    ret = gpio_ctrl(CMD_GPIO_INPUT, &param);

    return ret == 0;
}

bool dfu_enabled()
{
    return dfu_mode;
}

void gpio_button_check()
{
    uint32_t ret;
    uint32_t param;

    param = dfu_gpio_port;
    ret = gpio_ctrl(CMD_GPIO_INPUT, &param);

    if (gpio_last_value == 1 && ret == 0)
        button_pressed_count++;
    gpio_last_value = ret;

    delay(1000);
}

int dfu_pressed_count()
{
    return button_pressed_count;
}

void dfu_app_init(void)
{
    dfu_gpio_init();
    dfu_mode_check();
}

void jump_pc_address(uint32_t addr)
{
    void (*start_entry)(void) = (void (*)(void))addr;
    start_entry();
}

void uart0_wait_tx_finish(void)
{
    while (!uart_is_tx_fifo_empty(0))
    {
    }
}

static void application_start(void)
{
    UINT32 param;

    // disable core irq
    portDISABLE_FIQ();
    portDISABLE_IRQ();

    param = ~0;
    sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_DISABLE, &param);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
    sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_DISABLE, &param);

    // clear interrupt status
    param = 0;
    sddev_control(ICU_DEV_NAME, CMD_CLR_INTR_RAW_STATUS, &param);
    sddev_control(ICU_DEV_NAME, CMD_CLR_INTR_STATUS, &param);

    //
    os_printf("jump to 0x%x\n", APPLICATION_START_ADDR);
    uart0_wait_tx_finish();

    jump_pc_address(APPLICATION_START_ADDR);

    while(1) {
        os_printf("fatal\n");
    }
}

void entry_main(void)
{
    driver_init();
    dfu_app_init();
    fclk_init();
    func_init_basic();
#if CFG_YMODEM
    ymodem_init();
#endif

    portENABLE_FIQ();
    portENABLE_IRQ();

    if (dfu_enabled()) {
        while (1) {
            // If DFU complete, wait for 10 ticks
            if (dfu_complete) {
                if (fclk_get_tick() - dfu_complete_tick > 10) {
                    os_printf("DFU complete\n");
                    application_start();
                }
            }

#if CFG_YMODEM
            gpio_button_check();
            if (dfu_pressed_count() >= 1) {
                if (Ymodem_Receive_Main() == 0) {
                    os_printf("DFU complete 1\n");
                    application_start();
                } else {
                    os_printf("DFU failed 1\n");
                }
            }
#endif
        }
    } else {
        application_start();
    }
}
// eof

