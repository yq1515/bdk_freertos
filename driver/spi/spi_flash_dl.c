#include "include.h"
#include "arm_arch.h"

#include "typedef.h"
#include "sys_config.h"

#include "spi_pub.h"
#include "gpio_pub.h"
#include "sys_ctrl_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"
#include "rtos_pub.h"
#include "spi_flash_dl.h"

//#include <rtdevice.h>

#if CFG_USB_SPI_DL

#if !CFG_USE_SPI_MASTER
#error "SPI FLASH NEED CFG_USE_SPI_MASTER ENABLE!!!"
#endif

#define FLASH_PHY_PAGE_SIZE       256
#define FLASH_PHY_SECTOR_SIZE     4096
#define FLASH_PHY_BLK_32K         (32*1024)
#define FLASH_PHY_BLK_64K         (64*1024)

#define CMD_READ_ID               0x9F
#define READ_ID_RESPONE_LEN       3

#define CMD_READ_STATUS_S7_0      0x05
#define CMD_READ_STATUS_S15_8     0x35
#define READ_STATUS_LEN           1
#define FLASH_STATUS_WIP_BIT      (1 << 0)
#define FLASH_STATUS_WEL_BIT      (1 << 1)
#define FLASH_STATUS_PRETECT_MASK (0x3F << 2)

#define CMD_WRITE_STATUS          0x01
#define ERASE_MODE_ALL            0x01
#define ERASE_MODE_BLOCK_64K      0x02
#define ERASE_MODE_BLOCK_32K      0x03
#define ERASE_MODE_SECTOR         0x04
#define CMD_ERASE_ALL             0xc7  // 0x60
#define CMD_ERASE_BLK_64K         0xD8
#define CMD_ERASE_BLK_32K         0x52
#define CMD_ERASE_SECTOR          0x20
#define DELAY_WHEN_BUSY_MS        0
#define CMD_READ_DATA             0x03
#define CMD_PAGE_PROG             0x02
#define CMD_WRITE_ENABLE          0x06
#define CMD_WRITE_DISABLE         0x04

#define SPI_ID 0
#define CONFIG_SPI_DMA 1
#define FLASH_PAGE_SIZE 256
#define FLASH_SECTOR_SIZE 0x1000


int spi_connect_status = 0;

#ifdef CONFIG_SPI_DMA

typedef struct {
    uint8_t*send_buf;
    uint32_t send_len;

    uint8_t*recv_buf;
    uint32_t recv_len;

    beken_semaphore_t spi_sema;
	beken_semaphore_t usb_sema;
}spi_usb_trans_t ;


spi_usb_trans_t p_spi_usb_trans = {0};

uint8_t chip_status = 0;//0:idle ; 1 : D2 ; 2 : B9 ==> if use it , need to switch controller

static unsigned int crc32_table[256];


static void spi_flash_init_extral_gpio(void)
{
    bk_gpio_config_output(SPI_FLASH_WP_GPIO_NUM);
    bk_gpio_output(SPI_FLASH_WP_GPIO_NUM, GPIO_INT_LEVEL_HIGH);

    bk_gpio_config_output(SPI_FLASH_HOLD_GPIO_NUM);
    bk_gpio_output(SPI_FLASH_HOLD_GPIO_NUM, GPIO_INT_LEVEL_HIGH);
}

static void spi_flash_enable_voltage(void)
{
    UINT32 param;

    param = BLK_BIT_MIC_QSPI_RAM_OR_FLASH;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);

    param = QSPI_IO_3_3V;
    sddev_control(SCTRL_DEV_NAME, CMD_QSPI_IO_VOLTAGE, &param);

    param = PSRAM_VDD_3_3V_DEF;
    sddev_control(SCTRL_DEV_NAME, CMD_QSPI_VDDRAM_VOLTAGE, &param);
}

static void spi_flash_disable_voltage(void)
{
    UINT32 param;

    param = BLK_BIT_MIC_QSPI_RAM_OR_FLASH;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_DISABLE, &param);
}


int make_crc32_table(void)
{
    u32 c;
    int i = 0;
    int bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (u32)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c & 1)
            {
                c = (c >> 1) ^ (0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }
        crc32_table[i] = c;
    }
    return 0;
}

uint32 crc32_make(uint32 crc, const uint8* buf, int len)
{
	while (len--)
	{
		crc = (crc >> 8)^(crc32_table[(crc^*buf++) & 0xff]);
	}

	return crc;
}

beken_semaphore_t bk_get_spi_sema()
{
    return p_spi_usb_trans.spi_sema;
}

void bk_set_spi_sema(beken_semaphore_t sema)
{
    p_spi_usb_trans.spi_sema = sema;
}

beken_semaphore_t bk_get_usb_sema()
{
    return p_spi_usb_trans.usb_sema;
}

void bk_set_usb_sema(beken_semaphore_t sema)
{
    p_spi_usb_trans.usb_sema = sema;
}

int bk_protocol(uint8_t* data)
{
    return  data[0] == 0x01 &&
            data[1] == 0xe0 &&
            data[2] == 0xfc ? 1 : 0;
}

int bk_protocol_operator_flash(uint8_t* data)
{
    return  data[0] == 0x01 &&
            data[1] == 0xe0 &&
            data[2] == 0xfc &&
            data[3] == 0xff &&
            data[4] == 0xf4 ? 1 : 0;
}

void bk_ret_protocol(uint8_t data[])
{
    data[0] = 0x04;
    data[1] = 0x0e;
    data[3] = 0x01;
    data[4] = 0xe0;
    data[5] = 0xfc;
}

void bk_flash_ret_protocol(uint8_t data[])
{
    data[0] = 0x04;
    data[1] = 0x0e;
    data[2] = 0xff;
    data[3] = 0x01;
    data[4] = 0xe0;
    data[5] = 0xfc;
    data[6] = 0xf4;
}

void bk_set_spi_usb_info(const void *tx_data, uint32_t tx_size, void *rx_data, uint32_t rx_size)
{
    p_spi_usb_trans.send_buf = (uint8_t*)tx_data;
    p_spi_usb_trans.send_len = tx_size;
    p_spi_usb_trans.recv_buf = (uint8_t*)rx_data;
    p_spi_usb_trans.recv_len = rx_size;
}

void bk_get_spi_usb_info(uint8_t **tx_data, uint32_t *tx_size, uint8_t **rx_data, uint32_t *rx_size)
{
    *tx_data = p_spi_usb_trans.send_buf ;
    *tx_size = p_spi_usb_trans.send_len ;
    *rx_data = p_spi_usb_trans.recv_buf ;
    *rx_size = p_spi_usb_trans.recv_len ;
}

void spi_delay_ms(uint32_t ms)
{
    rtos_delay_milliseconds(ms);
}

void spi_connect_deinit(void)
{
    //reset gpio44~47
    REG_WRITE(0x802800 + 14 * 4,0);
    REG_WRITE(0x802800 + 15 * 4,0);
    REG_WRITE(0x802800 + 16 * 4,0);
    REG_WRITE(0x802800 + 17 * 4,0);

    REG_WRITE(0x802800 + 26 * 4,0);
    rtos_delay_milliseconds(2);

    spi_flash_disable_voltage();
    bk_spi_master_deinit();

    rtos_delay_milliseconds(2);

}

uint32_t spi_get_flash_id(void)
{
    uint32_t uid = 0;
    uint8_t uid_buf[READ_ID_RESPONE_LEN] = {0};
    uint8_t uid_cmd[] = {CMD_READ_ID};
    struct spi_message msg = {0};

    os_memset(&msg, 0, sizeof(struct spi_message));
    msg.send_buf = uid_cmd;
    msg.send_len = sizeof(uid_cmd);
    msg.recv_buf = uid_buf;
    msg.recv_len = READ_ID_RESPONE_LEN;

    bk_spi_master_xfer(&msg);

    uid = (uid_buf[0] << 16) | (uid_buf[1] << 8) | (uid_buf[2]);

    os_printf("============flash_mid:0x%06x==========\r\n", uid);
    return uid;
}

uint32_t spi_connect_init(void)
{
    uint8_t uid_buf[READ_ID_RESPONE_LEN] = {0};
    uint8_t uid_cmd_link[] = {0xD2};
    struct spi_message msg = {0};

    chip_status = 0;
    if (chip_status != 1)
    {
        //reset gpio14~17
        REG_WRITE(0x802800 + 14 * 4,0);
        REG_WRITE(0x802800 + 15 * 4,0);
        REG_WRITE(0x802800 + 16 * 4,0);
        REG_WRITE(0x802800 + 17 * 4,0);

        REG_WRITE(0x802800 + 26 * 4,0);
        rtos_delay_milliseconds(10);

        spi_flash_enable_voltage();
        spi_flash_init_extral_gpio();
        
        bk_spi_master_init(SPI_DEF_CLK_HZ, SPI_DEF_MODE);

        
        REG_WRITE(0x802800 + 26 * 4,2);

        for (size_t i = 0; i < 150; i++)
        {
            os_memset(&msg, 0, sizeof(struct spi_message));
            msg.send_buf = uid_cmd_link;
            msg.send_len = 1;
            msg.recv_buf = uid_buf;
            msg.recv_len = 1;
            
            bk_spi_master_xfer(&msg);
            rtos_delay_milliseconds(1);
        }
        chip_status = 1;
    }
    rtos_delay_milliseconds(10);
    
    return spi_get_flash_id();
}

uint32_t spi_set_baudrate(uint32_t br)
{
    os_printf("============br : %d==========\r\n", br);

    spi_connect_deinit();

    uint8_t uid_buf[READ_ID_RESPONE_LEN] = {0};
    uint8_t uid_cmd_link[] = {0xD2};
    struct spi_message msg = {0};

    //reset gpio14~17
    REG_WRITE(0x802800 + 14 * 4,0);
    REG_WRITE(0x802800 + 15 * 4,0);
    REG_WRITE(0x802800 + 16 * 4,0);
    REG_WRITE(0x802800 + 17 * 4,0);

    REG_WRITE(0x802800 + 26 * 4,0);
    rtos_delay_milliseconds(10);

    spi_flash_enable_voltage();
    spi_flash_init_extral_gpio();
    
    bk_spi_master_init(br, SPI_DEF_MODE);

    REG_WRITE(0x802800 + 26 * 4,2);

    for (size_t i = 0; i < 150; i++)
    {
        os_memset(&msg, 0, sizeof(struct spi_message));
        msg.send_buf = uid_cmd_link;
        msg.send_len = 1;
        msg.recv_buf = uid_buf;
        msg.recv_len = 1;
        
        bk_spi_master_xfer(&msg);
        rtos_delay_milliseconds(1);
    }
    rtos_delay_milliseconds(10);
    
    return spi_get_flash_id();
}

uint32_t spi_connect_reset(void)
{
    uint32_t uid = 0;
    if (spi_connect_status == 1)
    {
        return uid;
    }
    spi_connect_status = 1;
    uint8_t uid_buf[READ_ID_RESPONE_LEN] = {0};
    uint8_t uid_cmd[] = {CMD_READ_ID};
    uint8_t uid_cmd_link[] = {0xD2};

    struct spi_message msg = {0};
    os_memset(&msg, 0, sizeof(struct spi_message));
    msg.send_buf = uid_cmd;
    msg.send_len = sizeof(uid_cmd);
    msg.recv_buf = uid_buf;
    msg.recv_len = READ_ID_RESPONE_LEN;

    bk_spi_master_xfer(&msg);
    spi_connect_status = 0;

    uid = (uid_buf[0] << 16) | (uid_buf[1] << 8) | (uid_buf[2]);
    if (uid > 0 && uid != 0xffffff)
    {
        return uid;
    }

    //reset gpio14~17
    REG_WRITE(0x802800 + 14 * 4,0);
    REG_WRITE(0x802800 + 15 * 4,0);
    REG_WRITE(0x802800 + 16 * 4,0);
    REG_WRITE(0x802800 + 17 * 4,0);

    
    REG_WRITE(0x802800 + 26 * 4,0);
    rtos_delay_milliseconds(10);

    spi_flash_enable_voltage();
    spi_flash_init_extral_gpio();
    
    bk_spi_master_init(SPI_DEF_CLK_HZ, SPI_DEF_MODE);


    REG_WRITE(0x802800 + 26 * 4,2);

    for (size_t i = 0; i < 150; i++)
    {
        os_memset(&msg, 0, sizeof(struct spi_message));
        msg.send_buf = uid_cmd_link;
        msg.send_len = 1;
        msg.recv_buf = uid_buf;
        msg.recv_len = 1;
        
        bk_spi_master_xfer(&msg);
        rtos_delay_milliseconds(1);
    }
    chip_status = 1;
    
    os_memset(&msg, 0, sizeof(struct spi_message));
    for (size_t i = 0; i < 150; i++)
    {
        msg.send_buf = uid_cmd;
        msg.send_len = sizeof(uid_cmd);
        msg.recv_buf = uid_buf;
        msg.recv_len = READ_ID_RESPONE_LEN;

        bk_spi_master_xfer(&msg);

        uid = (uid_buf[0] << 16) | (uid_buf[1] << 8) | (uid_buf[2]);
        if (uid > 0)
        {
            break;
        }
    }

    // os_printf("============flash_mid:0x%06x==========\r\n", uid);
    spi_connect_status = 0;
    
    return uid;
}

uint32_t spi_switch_chip_wr_mode(uint8_t enter_chip_reg_cmd)
{
    uint32_t uRet = 0;
    uint8_t uid_buf_reg[1024] = {0};

    struct spi_message msg = {0};

    if ((enter_chip_reg_cmd == 0xB9 && chip_status != 2) ||
        (enter_chip_reg_cmd == 0xD2 && chip_status != 1))
    {
        //reset gpio44~47
        REG_WRITE(0x802800 + 44 * 4,0);
        REG_WRITE(0x802800 + 45 * 4,0);
        REG_WRITE(0x802800 + 46 * 4,0);
        REG_WRITE(0x802800 + 47 * 4,0);

        
        REG_WRITE(0x802800 + 26 * 4,0);
        rtos_delay_milliseconds(10);

        spi_flash_enable_voltage();
        spi_flash_init_extral_gpio();
        
        bk_spi_master_init(SPI_DEF_CLK_HZ, SPI_DEF_MODE);


        REG_WRITE(0x802800 + 26 * 4,2);

        uint8_t spi_cmd[] = {enter_chip_reg_cmd};

        for (size_t i = 0; i < 150; i++)
        {
            os_memset(&msg, 0, sizeof(struct spi_message));
            msg.send_buf = spi_cmd;
            msg.send_len = 1;
            msg.recv_buf = uid_buf_reg;
            msg.recv_len = 0;
            
            bk_spi_master_xfer(&msg);
            
            // rtos_delay_milliseconds(1);
        }
        if (enter_chip_reg_cmd == 0xB9)
        {
            chip_status = 2;
        }
        else if (enter_chip_reg_cmd == 0xD2)
        {
            chip_status = 1;
        }
        
        uRet = 1;
    }
    else
    {
        uRet = 1;
    }
    return uRet;
}

uint32_t spi_flash_crc_check(uint32_t flash_addr,uint32_t length)
{
    uint32_t crc_val = 0xFFFFFFFF;
    uint32_t data_len = 0;
    uint32_t i = 0;
    uint8_t *rx_data = (uint8_t *)os_zalloc(FLASH_PAGE_SIZE);
    
	make_crc32_table();
    for (i = 0; i < length; i += FLASH_PAGE_SIZE)
    {
        if ((length - i) >= FLASH_PAGE_SIZE)
        {
            data_len = FLASH_PAGE_SIZE;
        }
        else
        {
            data_len = length - i;
        }
        os_memset(rx_data, 0xFF, FLASH_PAGE_SIZE);
        spi_flash_read(flash_addr + i, data_len, rx_data);

        crc_val = crc32_make(crc_val, rx_data, FLASH_PAGE_SIZE);
    }

    os_free(rx_data);
    return crc_val;

}

uint32_t spi_read_chip_register(uint8_t spi_mode,uint32_t reg_addr)
{
    uint8_t cmd[20] = {0};
    struct spi_message msg = {0};
    uint8_t uid_buf_reg[1024] = {0};
    uint32_t reg_val = 0;

    os_memset(&msg, 0, sizeof(struct spi_message));
    os_memset(cmd, 0, sizeof(cmd));

    cmd[0] = spi_mode;
    cmd[1] = (reg_addr) & 0xff;
    cmd[2] = (reg_addr >> 8) & 0xff;
    cmd[3] = (reg_addr >> 16) & 0xff;
    cmd[4] = (reg_addr >> 24) & 0xff;
    msg.send_buf = cmd;
    msg.send_len = 5;
    msg.recv_buf = uid_buf_reg;
    msg.recv_len = 8;

    bk_spi_master_xfer(&msg);
    // for (int i = 0; i < msg.recv_len; i++) {
    //     os_printf("recv_buffer[%d]=0x%x\r\n", i, uid_buf_reg[i]);
    // }
    reg_val = ((uid_buf_reg[0]<<24) | (uid_buf_reg[1] << 16) | (uid_buf_reg[2] << 8) | (uid_buf_reg[3] )) ;
    os_printf("============reg_addr:0x%08x==========\r\n============reg_val:0x%08x==========\r\n",reg_addr, reg_val);

    return reg_val;
}

uint32_t spi_write_chip_register(uint8_t spi_mode,uint32_t reg_addr,uint32_t data)
{
    uint8_t cmd[20] = {0};
    struct spi_message msg = {0};
    uint8_t uid_buf_reg[1024] = {0};
    uint32_t reg_val = 0;

    os_memset(&msg, 0, sizeof(struct spi_message));
    os_memset(cmd, 0, sizeof(cmd));

    cmd[0] = spi_mode;
    cmd[1] = (reg_addr) & 0xff;
    cmd[2] = (reg_addr >> 8) & 0xff;
    cmd[3] = (reg_addr >> 16) & 0xff;
    cmd[4] = (reg_addr >> 24) & 0xff;
    cmd[5] = (data) & 0xff;
    cmd[6] = (data >> 8) & 0xff;
    cmd[7] = (data >> 16) & 0xff;
    cmd[8] = (data >> 24) & 0xff;
    msg.send_buf = cmd;
    msg.send_len = 9;
    msg.recv_buf = uid_buf_reg;
    msg.recv_len = 0;

    bk_spi_master_xfer(&msg);
    
    os_printf("============reg_addr:0x%08x==========\r\n============data:0x%08x==========\r\n",reg_addr, data);
    return reg_val;
}

static uint32_t spi_flash_is_busy(void)
{
    uint8_t ustatus_buf[READ_STATUS_LEN] = {0};
    uint8_t ustatus_cmd[] = {CMD_READ_STATUS_S7_0};
    struct spi_message msg;

    os_memset(&msg, 0, sizeof(struct spi_message));
    msg.send_buf = ustatus_cmd;
    msg.send_len = sizeof(ustatus_cmd);
    msg.recv_buf = ustatus_buf;
    msg.recv_len = READ_STATUS_LEN;
    bk_spi_master_xfer(&msg);
// os_printf("============ustatus_buf[0]:0x%02x==========\r\n", ustatus_buf[0]);
    return (ustatus_buf[0] & FLASH_STATUS_WIP_BIT);
}

void spi_tx_rx(uint8_t* tx_data,uint32_t tx_length,uint8_t** rx_data,uint32_t rx_length)
{
    // os_printf("====== tx ======\r\n");

    // for (int i = 0; i < tx_length; i++) {
    //     os_printf("%02x ", tx_data[i]);
    // }
    // os_printf("====== tx ======\r\n");

    struct spi_message msg = {0};
    os_memset(&msg, 0, sizeof(struct spi_message));
    msg.send_buf = tx_data;
    msg.send_len = tx_length;
    msg.recv_buf = *rx_data;
    msg.recv_len = rx_length;

    bk_spi_master_xfer(&msg);

    // for (int i = 0; i < rx_length; i++) {
    //     os_printf("%02x ", (*rx_data)[i]);
    // }
    // os_printf("====== rx ======\r\n");
}

uint8_t wait_busy_down(uint8_t* busy_byte)
{
    uint8_t send_buf[64] = {0};
    send_buf[0] = 0x05;

    spi_tx_rx(send_buf,1,&busy_byte,1);
    // os_printf("============busy_byte[0]:0x%02x==========\r\n", busy_byte[0]);

    return 1;
}

void spi_chip_enable(void)
{
    for (size_t i = 0; i < 10; i++)
    {
        spi_flash_send_command(CMD_WRITE_ENABLE); 

        uint8_t send_buf[64] = {0};
        for (size_t j = 0; j < 10; j++)
        {
            if (!wait_busy_down(&send_buf[0]))
            {
                break;
            }
            // os_printf("============spi_chip_enable :0x%02x==========\r\n", send_buf[0]);
            if (!(FLASH_STATUS_WIP_BIT & send_buf[0]))
            {
                break;
            }
            
        }
        if (FLASH_STATUS_WEL_BIT & send_buf[0])
        {
            break;
        }
    }
    
}

void spi_flash_send_command(uint8_t cmd)
{
    UINT8 ucmd = cmd;
    struct spi_message msg;

    os_memset(&msg, 0, sizeof(struct spi_message));

    msg.send_buf = &ucmd;
    msg.send_len = sizeof(ucmd);
    msg.recv_buf = NULL;
    msg.recv_len = 0;
    bk_spi_master_xfer(&msg);
}

int spi_sr_write(uint8_t* tx_data,uint32_t tx_length,uint8_t** rx_data,uint32_t rx_length)
{
    uint8_t send_buf[64] = {0};
    struct spi_message msg;

    os_memset(&msg, 0, sizeof(struct spi_message));
    msg.send_buf = tx_data;
    msg.send_len = tx_length;
    msg.recv_buf = NULL;
    msg.recv_len = 0;
    bk_spi_master_xfer(&msg);

    do
    {
        if (!wait_busy_down(&send_buf[0]))
        {
            break;
        }
        //os_printf("spi_sr_write:0x%02x==========\r\n", send_buf[0]);
    } while (0x01 & send_buf[0]);
    
    return 0;
}

uint32_t spi_flash_read_id(void)
{
    uint32_t uid = 0;
    uint8_t uid_buf[READ_ID_RESPONE_LEN] = {0};
    uint8_t uid_cmd[] = {CMD_READ_ID};
    uint8_t uid_cmd_link[] = {0xD2};
    uint8_t uid_buf_reg[1024] = {0};


    struct spi_message msg = {0};
    //reset gpio14~17
    REG_WRITE(0x802800 + 14 * 4,0);
    REG_WRITE(0x802800 + 15 * 4,0);
    REG_WRITE(0x802800 + 16 * 4,0);
    REG_WRITE(0x802800 + 17 * 4,0);

    
    REG_WRITE(0x802800 + 26 * 4,0);
    rtos_delay_milliseconds(10);

    // spi_config_t config = {0};
    spi_flash_enable_voltage();
    spi_flash_init_extral_gpio();
    
    bk_spi_master_init(SPI_DEF_CLK_HZ, SPI_DEF_MODE);


    REG_WRITE(0x802800 + 26 * 4,2);

#if 1
    for (size_t i = 0; i < 150; i++)
    {
        os_memset(&msg, 0, sizeof(struct spi_message));
        msg.send_buf = uid_cmd_link;
        msg.send_len = 1;
        msg.recv_buf = uid_buf;
        msg.recv_len = 1;
        
        bk_spi_master_xfer(&msg);
        // rtos_delay_milliseconds(1);
    }
    
    os_memset(&msg, 0, sizeof(struct spi_message));
    msg.send_buf = uid_cmd;
    msg.send_len = sizeof(uid_cmd);
    msg.recv_buf = uid_buf;
    msg.recv_len = READ_ID_RESPONE_LEN;

    bk_spi_master_xfer(&msg);

    uid = (uid_buf[0] << 16) | (uid_buf[1] << 8) | (uid_buf[2]);

    os_printf("============uid:%06x==========\r\n", uid);
#if 1
    //reset gpio14~17
    REG_WRITE(0x802800 + 14 * 4,0);
    REG_WRITE(0x802800 + 15 * 4,0);
    REG_WRITE(0x802800 + 16 * 4,0);
    REG_WRITE(0x802800 + 17 * 4,0);

    
    REG_WRITE(0x802800 + 26 * 4,0);
    rtos_delay_milliseconds(10);

    spi_flash_enable_voltage();
    spi_flash_init_extral_gpio();
    
    bk_spi_master_init(SPI_DEF_CLK_HZ, SPI_DEF_MODE);


    REG_WRITE(0x802800 + 26 * 4,2);
#endif

#endif

    // rtos_delay_milliseconds(1000);
    //get chip_id

    uint8_t tmp[20] = {0};
    tmp[0] = 0xB9;

    uint8_t uid_cmd_reg_B9[] = {0xB9};

    for (size_t i = 0; i < 250; i++)
    {
        // spi_flash_send_command(0xB9); 
        os_memset(&msg, 0, sizeof(struct spi_message));
        msg.send_buf = uid_cmd_reg_B9;
        msg.send_len = 1;
        msg.recv_buf = uid_buf;
        msg.recv_len = 1;
        
        bk_spi_master_xfer(&msg);
        // rtos_delay_milliseconds(1);
    }
    os_memset(&msg, 0, sizeof(struct spi_message));
    os_memset(tmp, 0, sizeof(tmp));


    tmp[0] = 0x6C;
    tmp[1] = 0x04;
    tmp[2] = 0x00;
    tmp[3] = 0x01;
    tmp[4] = 0x44;
    msg.send_buf = tmp;
    msg.send_len = 5;
    msg.recv_buf = uid_buf_reg;
    msg.recv_len = 8;

    bk_spi_master_xfer(&msg);
    // for (int i = 0; i < msg.recv_len; i++) {
    //     os_printf("recv_buffer[%d]=0x%x\r\n", i, uid_buf_reg[i]);
    // }
    uid = ((uid_buf_reg[0]<<24) | (uid_buf_reg[1] << 16) | (uid_buf_reg[2] << 8) | (uid_buf_reg[3] )) ;
    os_printf("============chip_id:0x%08x==========\r\n", uid);

    return uid;
}

static int spi_flash_read_page(uint32_t addr, uint32_t size, uint8_t *dst)
{
    struct spi_message msg;
    uint8_t ucmd[] = {CMD_READ_DATA, 0x00, 0x00, 0x00};

    if(dst == NULL)
        return 1;

    if(size > FLASH_PHY_SECTOR_SIZE)
        return 1;

    if(size == 0)
        return 0;

    os_memset(&msg, 0, sizeof(struct spi_message));
    ucmd[1] = ((addr >> 16) & 0xff);
    ucmd[2] = ((addr >> 8) & 0xff);
    ucmd[3] = (addr & 0xff);

    msg.send_buf = ucmd;
    msg.send_len = sizeof(ucmd);
    msg.recv_buf = dst;
    msg.recv_len = size;

    while(spi_flash_is_busy())
    {
        rtos_delay_milliseconds(DELAY_WHEN_BUSY_MS);
    }

    bk_spi_master_xfer(&msg);

    return 0;
}

int spi_flash_read(uint32_t addr, uint32_t size, uint8_t *dst)
{
    if(dst == NULL)
        return 1;

    if(size == 0)
    {
        return 0;
    }

    for(int i=0; i<size; )
    {
        int ret;
        uint32_t dsize;

        if((size - i) >= FLASH_PHY_SECTOR_SIZE)
            dsize = FLASH_PHY_SECTOR_SIZE;
        else
            dsize = size - i;

        ret = spi_flash_read_page(addr, dsize, dst);
        if(ret)
        {
            os_printf("spiff read page err:%d\r\n", ret);
            return 1;
        }

        addr = addr + dsize;
        dst = dst + dsize;
        i = i + dsize;
    }

    return 0;
}

void spi_flash_earse(uint32_t addr, uint32_t mode)
{
    uint8_t send_buf[64] = {0};

    struct spi_message msg;
    uint8_t ucmd[] = {0x00, 0x00, 0x00, 0x00};
    uint32_t send_len;

    os_memset(&msg, 0, sizeof(struct spi_message));

    if(mode == ERASE_MODE_ALL)
    {
        ucmd[0] = CMD_ERASE_ALL;
        send_len = 1;
    }
    else
    {
        if(mode == ERASE_MODE_BLOCK_64K)
        {
            ucmd[0] = CMD_ERASE_BLK_64K;
        }
        else if(mode == ERASE_MODE_BLOCK_32K)
        {
            ucmd[0] = CMD_ERASE_BLK_32K;
        }
        else if(mode == ERASE_MODE_SECTOR)
        {
            ucmd[0] = CMD_ERASE_SECTOR;
        }
        else
        {
            os_printf("earse wrong mode:%d\r\n", mode);
            return;
        }

        ucmd[1] = ((addr >> 16) & 0xff);
        ucmd[2] = ((addr >> 8) & 0xff);
        ucmd[3] = (addr & 0xff);
        send_len = 4;
    }
    
    msg.send_buf = ucmd;
    msg.send_len = send_len;

    msg.recv_buf = NULL;
    msg.recv_len = 0;

    while(spi_flash_is_busy())
    {
        rtos_delay_milliseconds(DELAY_WHEN_BUSY_MS);
    }

    spi_flash_send_command(CMD_WRITE_ENABLE);

    while(spi_flash_is_busy())
    {
        rtos_delay_milliseconds(DELAY_WHEN_BUSY_MS);
    }

    bk_spi_master_xfer(&msg);
    // bk_spi_write_bytes(SPI_ID, ucmd, send_len);
    // bk_spi_dma_write_bytes(SPI_ID, ucmd, send_len);
    do
    {
        if (!wait_busy_down(&send_buf[0]))
        {
            break;
        }
        //os_printf("============send_buf[0]:0x%02x==========\r\n", send_buf[0]);

    } while (0x03 == send_buf[0]);
}

int spi_flash_erase_dl(uint32_t addr, uint32_t size,uint32_t erase_mode)
{
    int left_size = (int)size;

    if (erase_mode == 1)
    {
        spi_flash_earse(addr, erase_mode);
    }
    else
    {
        while (left_size > 0)
        {
            uint32_t erase_size = 0;

            if(left_size <= 4 * 1024)
            {
                erase_size = 4 * 1024;
                erase_mode = ERASE_MODE_SECTOR;
            }
            else if(size <= 32 * 1024)
            {
                erase_size = 32 * 1024;
                erase_mode = ERASE_MODE_BLOCK_32K;
            }
            else
            {
                erase_size = 64 * 1024;
                erase_mode = ERASE_MODE_BLOCK_64K;
            }

            spi_flash_earse(addr, erase_mode);

            if(addr & (erase_size - 1))
            {
                size = erase_size - (addr & (erase_size - 1));
            }
            else
            {
                size = erase_size;
            }

        os_printf("addr:%d,erase_mode:%d,left_size:%d,size:%d\r\n",addr,erase_mode,left_size,size);

            left_size -= size;
            addr += size;
        }

    }
    
    
    return 0;
}

static int spi_flash_program_page(uint32_t addr, uint32_t size, uint8_t *src)
{
    uint8_t send_buf[64] = {0};

    struct spi_message msg;
    uint8_t *ucmd;

    if(src == NULL)
        return 1;

    if(size > FLASH_PHY_PAGE_SIZE)
        return 1;

    if(size == 0)
        return 0;

    ucmd = os_malloc(size + 4);
    if(!ucmd)
        return 1;

    os_memset(&msg, 0, sizeof(struct spi_message));
    os_memset(ucmd, 0, size + 4);

    ucmd[0] = CMD_PAGE_PROG;
    ucmd[1] = ((addr >> 16) & 0xff);
    ucmd[2] = ((addr >> 8) & 0xff);
    ucmd[3] = (addr & 0xff);
    os_memcpy(&ucmd[4], src, size);

    msg.send_buf = ucmd;
    msg.send_len = size + 4;
    msg.recv_buf = NULL;
    msg.recv_len = 0;

    while(spi_flash_is_busy())
    {
        rtos_delay_milliseconds(DELAY_WHEN_BUSY_MS);
    }

    spi_flash_send_command(CMD_WRITE_ENABLE);

    while(spi_flash_is_busy())
    {
        rtos_delay_milliseconds(DELAY_WHEN_BUSY_MS);
    }

    bk_spi_master_xfer(&msg);
    // bk_spi_write_bytes(SPI_ID, ucmd, msg.send_len);

    // bk_spi_dma_write_bytes(SPI_ID, ucmd, msg.send_len);
    do
    {
        if (!wait_busy_down(&send_buf[0]))
        {
            break;
        }
        // os_printf("============send_buf[0]:0x%02x==========\r\n", send_buf[0]);
    } while (0x03 == send_buf[0]);
    os_free(ucmd);

    return 0;
}

int spi_flash_write(uint32_t addr, uint32_t size, uint8_t *src)
{
    if(src == NULL)
        return 1;

    if(size == 0)
    {
        return 0;
    }

    for(int i=0; i<size; )
    {
        int ret;
        uint32_t dsize;

        if((size - i) >= FLASH_PHY_PAGE_SIZE)
            dsize = FLASH_PHY_PAGE_SIZE;
        else
            dsize = size - i;

        ret = spi_flash_program_page(addr, dsize, src);
        if(ret)
        {
            os_printf("spi write page err:%d\r\n", ret);
            return 1;
        }

        addr = addr + dsize;
        src = src + dsize;
        i = i + dsize;
    }

    return 0;
}
#endif
#endif


