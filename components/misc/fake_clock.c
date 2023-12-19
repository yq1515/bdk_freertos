#include "fake_clock_pub.h"
#include "pwm_pub.h"
#include "bk_timer_pub.h"
#include "icu_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"

//#include "power_save_pub.h"
//#include "mcu_ps_pub.h"
#include "BkDriverWdg.h"

static volatile UINT64 jiffies = 0;
static volatile UINT32 current_seconds = 0;
static BK_HW_TIMER_INDEX fclk_id = BK_PWM_TIMER_ID0;
UINT32 use_cal_net = 0;

extern void mcu_ps_increase_clr(void);
extern uint32_t preempt_delayed_schedule_get_flag(void);
extern void preempt_delayed_schedule_clear_flag(void);
static inline UINT64 fclk_freertos_get_tick64(void);

#define INT_WDG_FEED_PERIOD_TICK ((BK_MS_TO_TICKS(CFG_INT_WDG_PERIOD_MS)) >> 4)
#define TASK_WDG_PERIOD_TICK (BK_MS_TO_TICKS(CFG_TASK_WDG_PERIOD_MS))

#if (CFG_INT_WDG_ENABLED)
void int_watchdog_feed(void)
{
	static UINT64 s_last_int_wdg_feed = 0;
	UINT64 current_tick = fclk_freertos_get_tick64();

	if (current_tick - s_last_int_wdg_feed >= INT_WDG_FEED_PERIOD_TICK) {
		bk_wdg_reload();
		s_last_int_wdg_feed = current_tick;
		//os_printf("feed interrupt watchdog\n");
	}
}
#endif

void fclk_hdl(UINT8 param)
{
	jiffies++;
#if (CFG_INT_WDG_ENABLED)
	int_watchdog_feed();
#endif
}

static inline UINT32 fclk_freertos_get_tick32(void)
{
	return (UINT32)jiffies;
}

static inline UINT64 fclk_freertos_get_tick64(void)
{
	return jiffies;
}

UINT64 fclk_get_tick(void)
{
	UINT64 fclk;
	fclk = fclk_freertos_get_tick32();
	return fclk;
}

UINT32 fclk_get_second(void)
{
	return fclk_freertos_get_tick32() / FCLK_SECOND;
}

UINT32 fclk_from_sec_to_tick(UINT32 sec)
{
	return sec * FCLK_SECOND;
}

void fclk_reset_count(void)
{
	jiffies = 0;
	current_seconds = 0;
}

UINT32 fclk_cal_endvalue(UINT32 mode)
{
	UINT32 value = 1;

	if (PWM_CLK_32K == mode) {
		/*32k clock*/
		value = FCLK_DURATION_MS * 32;
	} else if (PWM_CLK_26M == mode) {
		/*26m clock*/
		value = FCLK_DURATION_MS * 26000;
	}

	return value;
}

BK_HW_TIMER_INDEX fclk_get_tick_id(void)
{
	return fclk_id;
}

/*timer_id:BK_PWM_TIMER_ID0 or BK_TIMER_ID3*/
void fclk_timer_hw_init(BK_HW_TIMER_INDEX timer_id)
{
	UINT32 ret;

#if (CFG_SOC_NAME == SOC_BK7231)
	ASSERT(timer_id >= BK_PWM_TIMER_ID0);
#endif

	fclk_id = timer_id;

	if (fclk_id >= BK_PWM_TIMER_ID0) {
		//pwm timer
		pwm_param_t param;

		/*init pwm*/
		param.channel         = (fclk_id - PWM0);
		param.cfg.bits.en     = PWM_ENABLE;
		param.cfg.bits.int_en = PWM_INT_EN;
		param.cfg.bits.mode   = PWM_TIMER_MODE;

#if(CFG_RUNNING_PLATFORM == FPGA_PLATFORM)  // FPGA:PWM0-2-32kCLK, pwm3-5-24CLK
		param.cfg.bits.clk    = PWM_CLK_32K;
#else
		param.cfg.bits.clk    = PWM_CLK_26M;
#endif

		param.p_Int_Handler   = fclk_hdl;
#if (CFG_SOC_NAME == SOC_BK7231N)
		param.duty_cycle1     = 0;
#else
		param.duty_cycle      = 0;
#endif
		param.end_value       = fclk_cal_endvalue((UINT32)param.cfg.bits.clk);

		sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);

	} else {
		//timer
		timer_param_t param;
		param.channel = fclk_id;
		param.div = 1;
		param.period = FCLK_DURATION_MS;
		param.t_Int_Handler = fclk_hdl;

		ret = sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param);
		ASSERT(BK_TIMER_SUCCESS == ret);
		UINT32 timer_channel;
		timer_channel = param.channel;
		sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_ENABLE, &timer_channel);

	}
}

void fclk_init(void)
{
#if (CFG_SOC_NAME == SOC_BK7231)
	fclk_timer_hw_init(BK_PWM_TIMER_ID0);
#else
	fclk_timer_hw_init(BK_TIMER_ID3);
#endif
}

// eof


