#include "include.h"
#include "arm_arch.h"

#include "sys_ctrl_pub.h"
#include "sys_ctrl.h"
#include "target_util_pub.h"

#include "drv_model_pub.h"

#include "uart_pub.h"
#include "flash_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "start_type_pub.h"

#define DPLL_DIV                0x0
#define DCO_CALIB_26M           0x1
#define DCO_CALIB_60M           0x2
#define DCO_CALIB_80M           0x3
#define DCO_CALIB_120M          0x4
#define DCO_CALIB_180M          0x5

#if (CFG_SOC_NAME == SOC_BK7221U)
#define DCO_CLK_SELECT          DCO_CALIB_180M
#define USE_DCO_CLK_POWON       1

UINT8  calib_charger[3] = {
    0x23,   //vlcf
    0x15,   //icp
    0x1b    //vcv
    };
#else
#define DCO_CLK_SELECT          DCO_CALIB_120M
#define USE_DCO_CLK_POWON       0 //BK7231N could not using DCO as main clock when boot since DCO_AMSEL_BIT should be disable during calibration
#endif

static SDD_OPERATIONS sctrl_op =
{
    sctrl_ctrl
};

#if (CFG_SOC_NAME == SOC_BK7231N)
void sctrl_fix_dpll_div(void);
#endif

/**********************************************************************/
void sctrl_dpll_delay10us(void)
{
    volatile UINT32 i = 0;

    for(i = 0; i < DPLL_DELAY_TIME_10US; i ++)
    {
        ;
    }
}

void sctrl_dpll_delay200us(void)
{
    volatile UINT32 i = 0;

    for(i = 0; i < DPLL_DELAY_TIME_200US; i ++)
    {
        ;
    }
}

void sctrl_ps_dpll_delay(UINT32 time)
{
    volatile UINT32 i = 0;

    for(i = 0; i < time; i ++)
    {
        ;
    }
}

void sctrl_cali_dpll(UINT8 flag)
{
    UINT32 param;

#if (CFG_SOC_NAME == SOC_BK7231N)
    extern void bk7011_update_tx_power_when_cal_dpll(int start_or_stop);

    bk7011_update_tx_power_when_cal_dpll(1);
#endif
    param = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
    param &= ~(SPI_TRIG_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    if(!flag)
        sctrl_dpll_delay10us();
    else
        sctrl_ps_dpll_delay(60);

    param |= (SPI_TRIG_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    param = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
    param &= ~(SPI_DET_EN);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    if(!flag)
        sctrl_dpll_delay200us();
    else
        sctrl_ps_dpll_delay(340);

    param = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
    param |= (SPI_DET_EN);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

#if (CFG_SOC_NAME == SOC_BK7231N)
    bk7011_update_tx_power_when_cal_dpll(0);
#endif
}

void sctrl_dpll_isr(void)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    os_printf("BIAS Cali\r\n");
    bk7011_cal_bias();
#endif
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CLR_DPLL_UNLOOK_INT_BIT, NULL);
    sctrl_cali_dpll(0);

    os_printf("DPLL Unlock\r\n");
}

void sctrl_dpll_int_open(void)
{
    UINT32 param;

    param = (FIQ_DPLL_UNLOCK_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

    #if (CFG_SOC_NAME != SOC_BK7231)
    param = 1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_DPLL_UNLOOK_INT, &param);
    #endif
}

void sctrl_dpll_int_close(void)
{
    UINT32 param;

    #if (CFG_SOC_NAME != SOC_BK7231)
    param = 0;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_DPLL_UNLOOK_INT, &param);
    #endif

    param = (FIQ_DPLL_UNLOCK_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

void sctrl_dco_cali(UINT32 speed)
{
    UINT32 reg_val;

    switch(speed)
    {
        case DCO_CALIB_180M:
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0xDD & DCO_CNTI_MASK) << DCO_CNTI_POSI);
#if (CFG_SOC_NAME != SOC_BK7231N)
        reg_val |= DIV_BYPASS_BIT;
#endif
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);
        break;

        case DCO_CALIB_120M:
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0x127 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);
        break;

        case DCO_CALIB_80M:
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0x0C5 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);
        break;

        case DCO_CALIB_60M:
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0x127 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        reg_val |= ((0x02 & DCO_DIV_MASK) << DCO_DIV_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);
        break;

        default:
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0xC0 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        reg_val |= ((0x03 & DCO_DIV_MASK) << DCO_DIV_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);
        break;
    }

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val &= ~(SPI_RST_BIT);
#if (CFG_SOC_NAME == SOC_BK7231N)
    reg_val &= ~(DCO_AMSEL_BIT);
#endif
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val |= SPI_RST_BIT;
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val |= DCO_TRIG_BIT;
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val &= ~(DCO_TRIG_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);
}

void sctrl_set_cpu_clk_dco(void)
{
    UINT32 reg_val;

    reg_val = REG_READ(SCTRL_CONTROL);
    reg_val &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    reg_val &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);

    reg_val |= ((MCLK_FIELD_DCO&MCLK_MUX_MASK) << MCLK_MUX_POSI);
    reg_val |= HCLK_DIV2_EN_BIT;

    REG_WRITE(SCTRL_CONTROL, reg_val);
    delay(10);
}

void sctrl_init(void)
{
    UINT32 param;

    sddev_register_dev(SCTRL_DEV_NAME, &sctrl_op);

    /*enable blk clk
      Attention: ENABLE 26m xtal block(BLK_BIT_26M_XTAL), for protect 32k circuit
     */
    param = BLK_BIT_26M_XTAL | BLK_BIT_DPLL_480M | BLK_BIT_XTAL2RF | BLK_BIT_DCO;
    sctrl_ctrl(CMD_SCTRL_BLK_ENABLE, &param);

    /*config main clk*/
#if !USE_DCO_CLK_POWON
    param = REG_READ(SCTRL_CONTROL);
    param &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    param &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
#if (CFG_SOC_NAME == SOC_BK7221U)
    /* BK7221U ahb bus max rate is 90MHZ, so ahb bus need div 2 from MCU clock */
    /* AHB bus is very import to AUDIO and DMA */
    param |= HCLK_DIV2_EN_BIT;
#endif // (CFG_SOC_NAME == SOC_BK7221U)
#if CFG_SYS_REDUCE_NORMAL_POWER
    param |= ((MCLK_DIV_7 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param |= ((MCLK_DIV_5 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#elif (CFG_SOC_NAME == SOC_BK7271)
    param |= ((MCLK_DIV_3 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#else // CFG_SYS_REDUCE_NORMAL_POWER
    param |= ((MCLK_DIV_3 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#endif // CFG_SYS_REDUCE_NORMAL_POWER
    param |= ((MCLK_FIELD_DPLL & MCLK_MUX_MASK) << MCLK_MUX_POSI);
    REG_WRITE(SCTRL_CONTROL, param);
#endif // (!USE_DCO_CLK_POWON)

    /*sys_ctrl <0x4c> */
    param = 0x00171710;//0x00151510;    LDO BIAS CALIBRATION
    REG_WRITE(SCTRL_BIAS, param);

    /*sys_ctrl <0x16>, trig spi */
    //170209,from 0x819A54B to 0x819A55B for auto detect dpll unlock
    //170614 from 0x819A55B to 0x819A59B for more easy to trigger
    //181101 xamp:0xf-0 for xtal may dead
#if (CFG_SOC_NAME == SOC_BK7231N)
#if (CFG_XTAL_FREQUENCE == CFG_XTAL_FREQUENCE_40M)
    param = 0x71125B57;
#else
    param = 0x71104953;//wangjian20200918 Reg0x16<3:1>=1 Reg0x16<9>=0 Reg0x16<13:10>=2
#endif
#else
    param = 0x819A59B;
#endif
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    sctrl_cali_dpll(0);

#if (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x3CC019C2;//wangjian20200918 Reg0x17<1>=1
#else
    param = 0x6AC03102;
#endif
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, param);
    /*do dco Calibration*/
    sctrl_dco_cali(DCO_CLK_SELECT);
#if USE_DCO_CLK_POWON
    sctrl_set_cpu_clk_dco();
#endif

#if (CFG_SOC_NAME == SOC_BK7231)
    param = 0x24006000;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x500020E2;//0x400020E0; //wangjian20200822 0x40032030->0x48032030->0x48022032//wangjian20200903<17:16>=0//qunshan20201127<28:23>=20
#elif (CFG_SOC_NAME == SOC_BK7271)
    param = 0x80208B00; //for 32k if enable BLK_BIT_ROSC32K
#else
    param = 0x24006080;   // xtalh_ctune   // 24006080
    param &= ~(XTALH_CTUNE_MASK << XTALH_CTUNE_POSI);
    param |= ((0x10 & XTALH_CTUNE_MASK) << XTALH_CTUNE_POSI);
#endif // (CFG_SOC_NAME == SOC_BK7231)
    sctrl_analog_set(SCTRL_ANALOG_CTRL2, param);

#if (CFG_SOC_NAME == SOC_BK7221U)
    param = CHARGE_ANALOG_CTRL3_CHARGE_DEFAULT_VALUE;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x70000000; //wangjiang20200822 0x00000000->0x70000000
#else
    param = 0x4FE06C50;
#endif
    sctrl_analog_set(SCTRL_ANALOG_CTRL3, param);

    /*sys_ctrl <0x1a> */
#if (CFG_SOC_NAME == SOC_BK7231)
    param = 0x59E04520;
#elif (CFG_SOC_NAME == SOC_BK7221U)
    param = CHARGE_ANALOG_CTRL4_CAL_DEFAULT_VALUE;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x19C04520;
#else
    param = 0x59C04520;  // 0x59E04520
#endif // (CFG_SOC_NAME == SOC_BK7231)
    sctrl_analog_set(SCTRL_ANALOG_CTRL4, param);

    sctrl_sub_reset();

#if (CFG_SOC_NAME == SOC_BK7231N)
    sctrl_fix_dpll_div();
#endif

    /*sys ctrl clk gating, for rx dma dead*/
    REG_WRITE(SCTRL_CLK_GATING, 0x3f);

    /* increase VDD voltage*/
#if (CFG_SOC_NAME == SOC_BK7231N)
    param = 4;//wyg// original=3
#elif CFG_SYS_REDUCE_NORMAL_POWER
    param = 4;
#else
    param = 5;
#endif
    sctrl_ctrl(CMD_SCTRL_SET_VDD_VALUE, &param);

    /*32K Rosc calib*/
    REG_WRITE(SCTRL_ROSC_CAL, 0x7);
}


void sctrl_exit(void)
{
    sddev_unregister_dev(SCTRL_DEV_NAME);
}

void sctrl_modem_core_reset(void)
{
    sctrl_ctrl(CMD_SCTRL_MODEM_CORE_RESET, 0);
}

void sctrl_sub_reset(void)
{
    sctrl_ctrl(CMD_SCTRL_MPIF_CLK_INVERT, 0);
    sctrl_ctrl(CMD_SCTRL_MODEM_CORE_RESET, 0);
    sctrl_ctrl(CMD_SCTRL_MODEM_SUBCHIP_RESET, 0);
    sctrl_ctrl(CMD_SCTRL_MAC_SUBSYS_RESET, 0);
    sctrl_ctrl(CMD_SCTRL_USB_SUBSYS_RESET, 0);
}

int sctrl_rf_wakeup_enabled(void)
{
    // as none case need disable rf wakeup until now, always enable rf wakeup
    return 1;
}

void sctrl_subsys_power(UINT32 cmd)
{
    UINT32 reg = 0;
    UINT32 reg_val;
    UINT32 reg_word = 0;

    switch(cmd)
    {
    case CMD_SCTRL_DSP_POWERDOWN:
        reg = SCTRL_DSP_PWR;
        reg_word = DSP_PWD;
        break;

    case CMD_SCTRL_DSP_POWERUP:
        reg = SCTRL_DSP_PWR;
        reg_word = DSP_PWU;
        break;

#if (CFG_SOC_NAME != SOC_BK7231N)
    case CMD_SCTRL_USB_POWERDOWN:
        reg = SCTRL_USB_PWR;
        reg_val = REG_READ(SCTRL_USB_PWR);
        reg_val &= ~(USB_PWD_MASK << USB_PWD_POSI);
        reg_val |= USB_PWD << USB_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_USB_POWERUP:
        reg = SCTRL_USB_PWR;
        reg_val = REG_READ(SCTRL_USB_PWR);
        reg_val &= ~(USB_PWD_MASK << USB_PWD_POSI);
        reg_val |= USB_PWU << USB_PWD_POSI;
        reg_word = reg_val;
        break;
#endif

    case CMD_SCTRL_MAC_POWERDOWN:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MAC_PWD_MASK << MAC_PWD_POSI);
        reg_val |= MAC_PWD << MAC_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_MAC_POWERUP:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MAC_PWD_MASK << MAC_PWD_POSI);
        reg_val |= MAC_PWU << MAC_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_MODEM_POWERDOWN:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MODEM_PWD_MASK << MODEM_PWD_POSI);
        reg_val |= MODEM_PWD << MODEM_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_BLE_POWERDOWN:
        reg = SCTRL_USB_PWR;
        reg_val = REG_READ(SCTRL_USB_PWR);
        reg_val &= ~(BLE_PWD_MASK << BLE_PWD_POSI);
        reg_val |= BLE_PWD << BLE_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_MODEM_POWERUP:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MODEM_PWD_MASK << MODEM_PWD_POSI);
        reg_val |= MODEM_PWU << MODEM_PWD_POSI;
        reg_word = reg_val;
        break;

     case CMD_SCTRL_BLE_POWERUP:
        reg = SCTRL_USB_PWR;
        reg_val = REG_READ(SCTRL_USB_PWR);
        reg_val &= ~(BLE_PWD_MASK << BLE_PWD_POSI);
        reg_val |= BLE_PWU << BLE_PWD_POSI;
        reg_word = reg_val;
        break;

    default:
        break;
    }

    if(reg)
    {
        REG_WRITE(reg, reg_word);
    }
}

void sctrl_subsys_reset(UINT32 cmd)
{
    UINT32 reg = 0;
    UINT32 reset_word = 0;

    switch(cmd)
    {
    case CMD_SCTRL_MODEM_SUBCHIP_RESET:
        reg = SCTRL_MODEM_SUBCHIP_RESET_REQ;
        reset_word = MODEM_SUBCHIP_RESET_WORD;
        break;

    case CMD_SCTRL_MAC_SUBSYS_RESET:
        reg = SCTRL_MAC_SUBSYS_RESET_REQ;
        reset_word = MAC_SUBSYS_RESET_WORD;
        break;

    case CMD_SCTRL_USB_SUBSYS_RESET:
        reg = SCTRL_USB_SUBSYS_RESET_REQ;
        reset_word = USB_SUBSYS_RESET_WORD;
        break;

    case CMD_SCTRL_DSP_SUBSYS_RESET:
        reg = SCTRL_DSP_SUBSYS_RESET_REQ;
        reset_word = DSP_SUBSYS_RESET_WORD;
        break;

    default:
        break;
    }

    if(reg)
    {
        REG_WRITE(reg, reset_word);
        delay(10);
        REG_WRITE(reg, 0);
    }

    return;
}

#if (CFG_SOC_NAME == SOC_BK7231N)
void sctrl_fix_dpll_div(void)
{
    volatile INT32   i;
    uint32 reg;
    uint32 cpu_clock;

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    reg = REG_READ(SCTRL_CONTROL);
    cpu_clock = reg & 0xFF;
    reg = (reg & 0xFFFFFF00) | 0x52;
    REG_WRITE(SCTRL_CONTROL, reg);

    for(i = 0; i < 100; i ++);

    REG_WRITE(SCTRL_MODEM_SUBCHIP_RESET_REQ, MODEM_SUBCHIP_RESET_WORD);
    REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) | (1 << 14));

    for(i = 0; i < 100; i ++);

    REG_WRITE(SCTRL_MODEM_SUBCHIP_RESET_REQ, 0);
    REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) & ~(1 << 14));

    for(i = 0; i < 100; i ++);

    reg = REG_READ(SCTRL_CONTROL);
    reg = (reg & 0xFFFFFF00) | cpu_clock;
    REG_WRITE(SCTRL_CONTROL, reg);

    for(i = 0; i < 100; i ++);

    GLOBAL_INT_RESTORE();
}
#endif

#if (CFG_SOC_NAME != SOC_BK7231)
static int sctrl_read_efuse(void *param)
{
    UINT32 reg, ret = -1;
    EFUSE_OPER_PTR efuse;
    efuse = (EFUSE_OPER_PTR)param;

    if(efuse) {
        reg = REG_READ(SCTRL_EFUSE_CTRL);
        reg &= ~(EFUSE_OPER_ADDR_MASK << EFUSE_OPER_ADDR_POSI);
        reg &= ~(EFUSE_OPER_DIR);

        reg |= ((efuse->addr & EFUSE_OPER_ADDR_MASK) << EFUSE_OPER_ADDR_POSI);
        reg |= (EFUSE_OPER_EN);
        REG_WRITE(SCTRL_EFUSE_CTRL, reg);

        do {
            reg = REG_READ(SCTRL_EFUSE_CTRL);
        }while(reg & EFUSE_OPER_EN);

        reg = REG_READ(SCTRL_EFUSE_OPTR);
        if(reg & EFUSE_OPER_RD_DATA_VALID) {
            efuse->data = ((reg >> EFUSE_OPER_RD_DATA_POSI) & EFUSE_OPER_RD_DATA_MASK);
            ret = 0;
        } else {
            efuse->data = 0xFF;
        }
    }
    return ret;
}

static int check_efuse_can_write(UINT8 new_byte, UINT8 old_byte)
{
    if(new_byte == old_byte)
    {
        // no need to read
        return 1;
    }

    for(int i=0; i<8; i++)
    {
        UINT8 old_bit = ((old_byte >> i) & 0x01);
        UINT8 new_bit = ((new_byte >> i) & 0x01);

        if ((old_bit) && (!new_bit))
        {
            // can not change old from 1 to 0
            return 0;
        }
    }

    return 2;
}

static int sctrl_write_efuse(void *param)
{
    UINT32 reg, ret = -1;
    EFUSE_OPER_ST *efuse, efuse_bak;

#if (CFG_SOC_NAME == SOC_BK7221U)
    os_printf("BK7251 cannot write efuse via register\r\n");
    goto wr_exit;
#endif

    efuse = (EFUSE_OPER_PTR)param;
    if (efuse) {
        efuse_bak.addr = efuse->addr;
        efuse_bak.data = efuse->data;
        if (sctrl_read_efuse(&efuse_bak) == 0) {
            //read before write, ensure this byte and this bit no wrote
            ret = check_efuse_can_write(efuse->data, efuse_bak.data);
            if (ret == 0) {
                ret = -1;
                goto wr_exit;
            } else if (ret == 1) {
                ret = 0;
                goto wr_exit;
            }
        }

        // enable vdd2.5v first
        reg = REG_READ(SCTRL_CONTROL);
        reg |= EFUSE_VDD25_EN;
        REG_WRITE(SCTRL_CONTROL, reg);

        reg = REG_READ(SCTRL_EFUSE_CTRL);
        reg &= ~(EFUSE_OPER_ADDR_MASK << EFUSE_OPER_ADDR_POSI);
        reg &= ~(EFUSE_OPER_WR_DATA_MASK << EFUSE_OPER_WR_DATA_POSI);

        reg |= EFUSE_OPER_DIR;
        reg |= ((efuse->addr & EFUSE_OPER_ADDR_MASK) << EFUSE_OPER_ADDR_POSI);
        reg |= ((efuse->data & EFUSE_OPER_WR_DATA_MASK) << EFUSE_OPER_WR_DATA_POSI);
        reg |= EFUSE_OPER_EN;
        REG_WRITE(SCTRL_EFUSE_CTRL, reg);

        do {
            reg = REG_READ(SCTRL_EFUSE_CTRL);
        } while (reg & EFUSE_OPER_EN);

        // disable vdd2.5v at last
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~EFUSE_VDD25_EN;
        REG_WRITE(SCTRL_CONTROL, reg);

        // check, so read
        reg = efuse->data;
        efuse->data = 0;
        if (sctrl_read_efuse(param) == 0) {
            if (((UINT8)reg) == efuse->data)
                ret = 0;
        }
    }

wr_exit:
    return ret;
}


#endif // (CFG_SOC_NAME != SOC_BK7231)


UINT32 sctrl_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;
    UINT32 reg;
    GLOBAL_INT_DECLARATION();

    ret = SCTRL_SUCCESS;
    GLOBAL_INT_DISABLE();
    switch(cmd)
    {
    case CMD_GET_CHIP_ID:
        ret = REG_READ(SCTRL_CHIP_ID);
        break;

    case CMD_SCTRL_SET_FLASH_DPLL:
        reg = REG_READ(SCTRL_CONTROL);
        reg |= FLASH_26M_MUX_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_SET_FLASH_DCO:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~FLASH_26M_MUX_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_DSP_POWERDOWN:
    case CMD_SCTRL_USB_POWERDOWN:
    case CMD_SCTRL_MODEM_POWERDOWN:
    case CMD_SCTRL_MAC_POWERDOWN:
    case CMD_SCTRL_DSP_POWERUP:
    case CMD_SCTRL_USB_POWERUP:
    case CMD_SCTRL_MAC_POWERUP:
    case CMD_SCTRL_MODEM_POWERUP:
    case CMD_SCTRL_BLE_POWERDOWN:
    case CMD_SCTRL_BLE_POWERUP:
        sctrl_subsys_power(cmd);
        break;

    case CMD_GET_DEVICE_ID:
        ret = REG_READ(SCTRL_DEVICE_ID);
        break;

    case CMD_GET_SCTRL_CONTROL:
        *((UINT32 *)param) = REG_READ(SCTRL_CONTROL);
        break;

    case CMD_SET_SCTRL_CONTROL:
        REG_WRITE(SCTRL_CONTROL, *((UINT32 *)param));
        break;

    case CMD_SCTRL_MCLK_SELECT:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
        reg |= ((*(UINT32 *)param) & MCLK_MUX_MASK) << MCLK_MUX_POSI;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_MCLK_DIVISION:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
        reg |= ((*(UINT32 *)param) & MCLK_DIV_MASK) << MCLK_DIV_POSI;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_MCLK_MUX_GET:
        reg = ((REG_READ(SCTRL_CONTROL) >> MCLK_MUX_POSI) & MCLK_MUX_MASK);
        *(UINT32 *)param = reg;
        break;

    case CMD_SCTRL_MCLK_DIV_GET:
        reg = ((REG_READ(SCTRL_CONTROL) >> MCLK_DIV_POSI) & MCLK_DIV_MASK);
        *(UINT32 *)param = reg;
        break;

    case CMD_SCTRL_RESET_SET:
        reg = REG_READ(SCTRL_RESET);
        reg |= ((*(UINT32 *)param) & SCTRL_RESET_MASK);
        REG_WRITE(SCTRL_RESET, reg);
        break;

    case CMD_SCTRL_RESET_CLR:
        reg = REG_READ(SCTRL_RESET);
        reg &= ~((*(UINT32 *)param) & SCTRL_RESET_MASK);
        REG_WRITE(SCTRL_RESET, reg);
        break;

    case CMD_SCTRL_MODEM_SUBCHIP_RESET:
    case CMD_SCTRL_MAC_SUBSYS_RESET:
    case CMD_SCTRL_USB_SUBSYS_RESET:
    case CMD_SCTRL_DSP_SUBSYS_RESET:
        sctrl_subsys_reset(cmd);
        break;

    case CMD_SCTRL_MODEM_CORE_RESET:
        ret = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
        ret = ret & (~((MODEM_CORE_RESET_MASK) << MODEM_CORE_RESET_POSI));
        reg = ret | ((MODEM_CORE_RESET_WORD & MODEM_CORE_RESET_MASK)
                     << MODEM_CORE_RESET_POSI);
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);

        delay(1);
        reg = ret;
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);

        /*resetting, and waiting for done*/
        reg = REG_READ(SCTRL_RESET);
        while(reg & MODEM_CORE_RESET_BIT)
        {
            delay(10);
            reg = REG_READ(SCTRL_RESET);
        }
        ret = SCTRL_SUCCESS;
        break;

    case CMD_SCTRL_MPIF_CLK_INVERT:
        reg = REG_READ(SCTRL_CONTROL);
        reg |= MPIF_CLK_INVERT_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_BLK_ENABLE:
        reg = REG_READ(SCTRL_BLOCK_EN_CFG);
        reg &= (~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI));
        reg |= (BLOCK_EN_WORD_PWD & BLOCK_EN_WORD_MASK) << BLOCK_EN_WORD_POSI;
        reg |= ((*(UINT32 *)param) & BLOCK_EN_VALID_MASK);
        REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);
        break;

    case CMD_SCTRL_BLK_DISABLE:
        reg = REG_READ(SCTRL_BLOCK_EN_CFG);
        reg &= (~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI));
        reg |= (BLOCK_EN_WORD_PWD & BLOCK_EN_WORD_MASK) << BLOCK_EN_WORD_POSI;
        reg &= ~((*(UINT32 *)param) & BLOCK_EN_VALID_MASK);
        REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);
        break;

    case CMD_SCTRL_BIAS_REG_SET:
        reg = REG_READ(SCTRL_BIAS);
        reg |= (*(UINT32 *)param);
        REG_WRITE(SCTRL_BIAS, reg);
        break;

    case CMD_SCTRL_BIAS_REG_CLEAN:
        reg = REG_READ(SCTRL_BIAS);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(SCTRL_BIAS, reg);
        break;

    case CMD_SCTRL_BIAS_REG_READ:
        ret = REG_READ(SCTRL_BIAS);
        break;

    case CMD_SCTRL_BIAS_REG_WRITE:
        REG_WRITE(SCTRL_BIAS, *(UINT32 *)param);
        break;

    case CMD_SCTRL_ANALOG_CTRL4_SET:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        reg |= (*(UINT32 *)param);
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, reg);
        break;

    case CMD_SCTRL_ANALOG_CTRL4_CLEAN:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        reg &= ~(*(UINT32 *)param);
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, reg);
        break;

    case CMD_SCTRL_CALI_DPLL:
        sctrl_cali_dpll(0);
        break;

#if (CFG_SOC_NAME != SOC_BK7231)
    case CMD_SCTRL_SET_XTALH_CTUNE:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
        reg &= ~(XTALH_CTUNE_MASK<< XTALH_CTUNE_POSI);
        reg |= (((*(UINT32 *)param) &XTALH_CTUNE_MASK) << XTALH_CTUNE_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL2, reg);
        break;

    case CMD_SCTRL_GET_XTALH_CTUNE:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
        ret = ((reg >> XTALH_CTUNE_POSI) & XTALH_CTUNE_MASK);
        break;

    case CMD_EFUSE_WRITE_BYTE:
        ret = sctrl_write_efuse(param);
        break;

    case CMD_EFUSE_READ_BYTE:
        ret = sctrl_read_efuse(param);
        break;

#endif // (CFG_SOC_NAME != SOC_BK7231)

#if (CFG_SOC_NAME != SOC_BK7231)
    case CMD_SCTRL_SET_ANALOG6:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL6);
        reg |= (DPLL_CLK_FOR_AUDIO_EN | DPLL_DIVIDER_CLK_SEL | DPLL_RESET );
        sctrl_analog_set(SCTRL_ANALOG_CTRL6, reg);
        break;
#endif
    case CMD_SCTRL_SET_ANALOG0:
        sctrl_analog_set(SCTRL_ANALOG_CTRL0, (*(UINT32 *)param));
        break;
    case CMD_SCTRL_SET_ANALOG1:
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, (*(UINT32 *)param));
        break;
    case CMD_SCTRL_SET_ANALOG2:
        sctrl_analog_set(SCTRL_ANALOG_CTRL2, (*(UINT32 *)param));
        break;
    case CMD_SCTRL_SET_ANALOG3:
        sctrl_analog_set(SCTRL_ANALOG_CTRL3, (*(UINT32 *)param));
        break;
    case CMD_SCTRL_SET_ANALOG4:
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, (*(UINT32 *)param));
        break;
    case CMD_SCTRL_SET_ANALOG5:
        sctrl_analog_set(SCTRL_ANALOG_CTRL5, (*(UINT32 *)param));
        break;
    case CMD_SCTRL_GET_ANALOG0:
        ret = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
        break;
    case CMD_SCTRL_GET_ANALOG1:
        ret = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        break;
    case CMD_SCTRL_GET_ANALOG2:
        ret = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
        break;
    case CMD_SCTRL_GET_ANALOG3:
        ret = sctrl_analog_get(SCTRL_ANALOG_CTRL3);
        break;
    case CMD_SCTRL_GET_ANALOG4:
        ret = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        break;
    case CMD_SCTRL_GET_ANALOG5:
        ret = sctrl_analog_get(SCTRL_ANALOG_CTRL5);
        break;
    case CMD_SCTRL_SET_LOW_PWR_CLK:
        reg = REG_READ(SCTRL_LOW_PWR_CLK);
        reg &=~(LPO_CLK_MUX_MASK);
        reg |=((*(UINT32 *)param) << LPO_CLK_MUX_POSI);
        REG_WRITE(SCTRL_LOW_PWR_CLK, reg);
        break;
    case CMD_SCTRL_SET_GADC_SEL:
        #if (CFG_SOC_NAME == SOC_BK7231N)
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        reg &= ~(GADC_CAL_SEL_MASK << GADC_CAL_SEL_POSI);
        reg |= (((*(UINT32 *)param) & GADC_CAL_SEL_MASK) << GADC_CAL_SEL_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, reg);
        #endif
        break;
    case CMD_SCTRL_SET_VDD_VALUE:
        reg = REG_READ(SCTRL_DIGTAL_VDD);
        reg &= (~(DIG_VDD_ACTIVE_MASK << DIG_VDD_ACTIVE_POSI));
        reg |=((*(UINT32 *)param) << DIG_VDD_ACTIVE_POSI);
        REG_WRITE(SCTRL_DIGTAL_VDD, reg);
        break;
    case CMD_SCTRL_GET_VDD_VALUE:
        reg = REG_READ(SCTRL_DIGTAL_VDD);
        ret = (reg >> DIG_VDD_ACTIVE_POSI) & DIG_VDD_ACTIVE_MASK;
        break;

    case CMD_GET_SCTRL_RETETION:
        *((UINT32 *)param) = REG_READ(SCTRL_SW_RETENTION);
        break;

    case CMD_SET_SCTRL_RETETION:
        REG_WRITE(SCTRL_SW_RETENTION, *((UINT32 *)param));
        break;

    default:
        ret = SCTRL_FAILURE;
        break;
    }
    GLOBAL_INT_RESTORE();

    return ret;
}

#if (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7231U)
extern void flash_set_clk(UINT8 clk_conf);
static uint32_t sctrl_overclock_refcnt __maybe_unused = 0;
void sctrl_overclock(int enable)
{
    UINT32 param;

    GLOBAL_INT_DECLARATION();

    if (enable) {
        GLOBAL_INT_DISABLE();
         /* already enabled */
        if (sctrl_overclock_refcnt++)
            goto out;

        param = REG_READ(SCTRL_CONTROL);
        param &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
        param |= ((MCLK_DIV_2 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
        REG_WRITE(SCTRL_CONTROL, param);
        flash_set_clk(8);
    } else {
        GLOBAL_INT_DISABLE();
        if (sctrl_overclock_refcnt)
            --sctrl_overclock_refcnt;

        /* other API still hold overclock */
        if (sctrl_overclock_refcnt)
            goto out;

        /*config main clk*/
#if !USE_DCO_CLK_POWON
        param = REG_READ(SCTRL_CONTROL);
        param &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
#if CFG_SYS_REDUCE_NORMAL_POWER
        param |= ((MCLK_DIV_7 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#else
        param |= ((MCLK_DIV_5 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#endif // CFG_SYS_REDUCE_NORMAL_POWER
        REG_WRITE(SCTRL_CONTROL, param);
#endif /*(!USE_DCO_CLK_POWON)*/

#if CFG_USE_STA_PS
        flash_set_clk(9);
#else
        flash_set_clk(5);
#endif
    }
out:
    GLOBAL_INT_RESTORE();
}
#endif


// EOF
