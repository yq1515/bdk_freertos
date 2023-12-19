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
#include "portmacro.h"

void entry_main(void)
{
    driver_init();
	func_init_basic();
    fclk_init();

	portENABLE_FIQ();
	portENABLE_IRQ();

	while (1) {
		__asm ("nop");
	}
}
// eof

