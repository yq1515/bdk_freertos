#include "include.h"
#include "func_pub.h"
#include "intc.h"
#include "uart_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "BkDriverWdg.h"
#include "sys_config.h"

#if CFG_UART_DEBUG
#include "uart_debug_pub.h"
#endif

#include "start_type_pub.h"

#if CFG_UF2
extern void msc_init(void);
extern void uf2_init(void);
#endif

#if CFG_UF2
extern void msc_init(void);
extern void uf2_init(void);
void sctrl_overclock(int enable);
void board_flash_init(void);
bool dfu_enabled();
#endif

UINT32 func_init_basic(void)
{
    intc_init();

#if CFG_UF2
    if (dfu_enabled()) {
        sctrl_overclock(1);
        board_flash_init();
        uf2_init();
        msc_init();
    }
#endif

    return 0;
}

// eof
