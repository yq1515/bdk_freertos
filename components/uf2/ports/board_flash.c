/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Zhaqian
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "board_api.h"
#include "uart_pub.h"
#include "drv_model_pub.h"
#include "flash_pub.h"

//--------------------------------------------------------------------+
// FLASH
//--------------------------------------------------------------------+
#define FLASH_BASE_ADDR 0x0

#define SECTOR_SIZE BOARD_SECTOR_SIZE
#define SECTOR_COUNT BOARD_SECTOR_COUNT

static uint8_t erased_sectors[SECTOR_COUNT] = {0};
static DD_HANDLE flash_handle;
static int protect_type;
static int flash_unlocked = false;

static void flash_sector_erase(uint32_t addr)
{
    // os_printf("e:%x\n", addr);
    ddev_control(flash_handle, CMD_FLASH_ERASE_SECTOR, (void *)&addr);
}

#if 0
static bool is_blank(uint32_t addr, uint32_t size) {
    for (uint32_t i = 0; i < size; i += sizeof(uint32_t)) {
        if (*(uint32_t *)(addr + i) != 0xffffffff) {
            return false;
        }
    }
    return true;
}
#endif

static bool flash_erase(uint32_t addr) {
#if 0
    uint32_t sector_addr = FLASH_BASE_ADDR;
    bool     erased      = false;

    uint32_t sector = 0;
    uint32_t size   = 0;
    (void)sector;
    for (uint32_t i = 0; i < SECTOR_COUNT; i++) {
        size = SECTOR_SIZE;
        if (sector_addr + size > addr) {
            sector            = i;
            erased            = erased_sectors[i];
            erased_sectors[i] = 1; // don't erase anymore - we will continue writing here!
            break;
        }
        sector_addr += size;
    }

    if (!erased && !is_blank(sector_addr, size)) {
        flash_sector_erase(sector_addr);
    }
#else
    uint32_t idx = addr >> 12 ;
    if (!erased_sectors[idx])  {
        erased_sectors[idx] = 1;
        flash_sector_erase(addr & ~(SECTOR_SIZE - 1));
    }
#endif

    return true;
}

static void flash_unlock()
{
    int param;

    if (flash_unlocked)
        return;

    if (protect_type != FLASH_PROTECT_NONE)
    {
        param = FLASH_PROTECT_NONE;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }

    flash_unlocked = true;
}

static void flash_lock()
{
    if (protect_type != FLASH_PROTECT_NONE)
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&protect_type);

    flash_unlocked = false;
}

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+
__attribute__((weak)) void board_flash_init(void)
{
    UINT32 status;
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_control(flash_handle, CMD_FLASH_GET_PROTECT, (void *)&protect_type);
}

__attribute__((weak)) uint32_t board_flash_size(void) {
    return BOARD_FLASH_SIZE;
}

__attribute__((weak)) void board_flash_read(uint32_t addr, void *buffer, uint32_t len) {
    memcpy(buffer, (void *)addr, len);
}

__attribute__((weak)) void board_flash_flush(void)
{
    flash_lock();
}

__attribute__((weak)) void board_flash_write(uint32_t addr, void const *data, uint32_t len) {
    // os_printf("w:%x\n", addr);
    flash_unlock();
    flash_erase(addr);
    flash_write((char *)data, len, addr);
    // flash_lock();  // When full firmware write complete, lock flash
}

__attribute__((weak)) void board_flash_erase_app(void) {}

#ifdef CHERRYUF2_SELF_UPDATE
__attribute__((weak)) void board_self_update(const uint8_t *bootloader_bin, uint32_t bootloader_len) {
    (void)bootloader_bin;
    (void)bootloader_len;
}
#endif

