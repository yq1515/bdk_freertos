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

#if CFG_CHERRY_USB
void cdc_acm_init(void);
#endif

#if CFG_USB_SPI_DL

extern void usbd_hid_init();
int bk_protocol(uint8_t* data);
int bk_protocol_operator_flash(uint8_t* data);
void bk_ret_protocol(uint8_t data[]);
void bk_flash_ret_protocol(uint8_t data[]);

void spi_flash_send_command(uint8_t cmd);
void spi_chip_enable(void);
void cdc_acm_init(void);
void cdc_write_info(uint8_t * _write_data,uint32_t len);
void spi_delay_ms(uint32_t ms);
void spi_connect_deinit(void);
void spi_tx_rx(uint8_t* tx_data,uint32_t tx_length,uint8_t** rx_data,uint32_t rx_length);

uint32_t spi_get_flash_id(void);
uint32_t spi_set_baudrate(uint32_t br);
// uint32_t spi_connect_init(void);
uint32_t spi_connect_reset(void);
uint32_t spi_switch_chip_wr_mode(uint8_t enter_chip_reg_cmd);
uint32_t spi_flash_crc_check(uint32_t flash_addr,uint32_t length);

uint32_t spi_read_chip_register(uint8_t spi_mode,uint32_t reg_addr);
uint32_t spi_write_chip_register(uint8_t spi_mode,uint32_t reg_addr,uint32_t data);
int spi_sr_write(uint8_t* tx_data,uint32_t tx_length,uint8_t** rx_data,uint32_t rx_length);

// extern beken_semaphore_t spi_usb_sem;
beken_semaphore_t bk_get_spi_sema();
beken_semaphore_t bk_get_usb_sema();
void bk_set_spi_sema(beken_semaphore_t sema);
void bk_set_usb_sema(beken_semaphore_t sema);

void bk_get_spi_usb_info(uint8_t **tx_data, uint32_t *tx_size, uint8_t **rx_data, uint32_t *rx_size);
void bk_set_spi_usb_info(const void *tx_data, uint32_t tx_size, void *rx_data, uint32_t rx_size);

// extern uint32_t spi_flash_read_id(void);
// extern int spi_flash_read(uint32_t addr, uint32_t size, uint8_t *dst);
// extern int spi_flash_write(uint32_t addr, uint32_t size, uint8_t *src);
// extern int spi_flash_erase_dl(uint32_t addr, uint32_t size,uint32_t erase_mode);


void spi_dl_init(void)
{
    cdc_acm_init();

    spi_connect_init();

	beken_semaphore_t sema_spi = NULL;
	beken_semaphore_t sema_usb = NULL;

	int err = 0;
	uint8_t*tx_data;
	uint32_t tx_size;
	uint8_t*rx_data;
	uint32_t rx_size;
	uint8_t* rev_protocol = (uint8_t *)os_zalloc(5120);
	if (rev_protocol == NULL)
	{
		return ;
	}

    err = rtos_init_semaphore(&sema_spi, 1);
    err = rtos_init_semaphore(&sema_usb, 1);
	bk_set_spi_sema(sema_spi);
	bk_set_usb_sema(sema_usb);

	if (bk_get_spi_sema() != NULL && bk_get_usb_sema() != NULL)
    {
		while (1)
		{
			err = rtos_get_semaphore(bk_get_spi_sema(), BEKEN_NEVER_TIMEOUT);
			if (err != 0)
			{
				return ;
			}
			else
			{
				bk_get_spi_usb_info(&tx_data, &tx_size, &rx_data, &rx_size);
				
				if (bk_protocol(tx_data) == 1)
				{
					if (bk_protocol_operator_flash(tx_data) == 1)//deal with flash : include flash download read erase;
					{
						os_memset(rev_protocol, 0, sizeof(rev_protocol));
						bk_flash_ret_protocol(rev_protocol);	

						rev_protocol[8] = 0x00;
						rev_protocol[9] = tx_data[7];
						rev_protocol[10] = 0x00;

						/* code */
						if (tx_data[7] == 0x0e)
						{
							/*spi_tx_rx*/
							if (tx_data[8] == 0x9f) // get flash_id
							{
								uint8_t *buf = (uint8_t *)os_zalloc(0x20);
								spi_tx_rx(&tx_data[8],1,&buf,3);
								rev_protocol[7] = tx_data[5] + 1;			//length

								rev_protocol[11] = 0x00;
								os_memcpy(&rev_protocol[12],buf,3);

                                if (buf) 
								{
									os_free(buf);
								}
								buf = NULL;

								rx_size = 15;
							}
							else
							{
								//spi_tx_rx
								if (tx_data[8] == 0x06)
								{
									spi_chip_enable();	
									// spi_tx_rx(&tx_data[8],1,&rx_data,0);
									rev_protocol[7] = tx_data[5] + 1;			//length

									rev_protocol[11] = 0x00;
									rev_protocol[12] = tx_data[8];
									rx_size = 13;
								}
								else if (tx_data[8] == 0x4B)//read flash uid
								{
                                    uint8_t *buf = (uint8_t *)os_zalloc(0x20);

									spi_tx_rx(&tx_data[8],5,&buf,16);
									rev_protocol[7] = tx_data[5] + 1;			//length

									rev_protocol[11] = 0x00;
									rev_protocol[12] = 0x00;
									os_memcpy(&rev_protocol[16],buf,16);
									
                                    if (buf) 
                                    {
                                        os_free(buf);
                                    }
                                    buf = NULL;

									rx_size = 16 + 16;
								}
							}

						}
						else if (tx_data[7] == 0x0c) /*flash_read_SR_reg*/
						{
                            uint8_t *buf = (uint8_t *)os_zalloc(0x20);
							spi_tx_rx(&tx_data[8],1,&buf,1);
							rev_protocol[7] = tx_data[5] + 2;			//length

							rev_protocol[11] = tx_data[8];
							os_memcpy(&rev_protocol[12],buf,1);
                            if (buf) 
                            {
                                os_free(buf);
                            }
                            buf = NULL;
							rx_size = 13;
						}
						else if (tx_data[7] == 0x0d) /*flash_write_SR_reg*/
						{
							// spi_flash_send_command(0x33);
							// spi_flash_send_command(0xCC);
							// spi_flash_send_command(0xAA);
							// spi_chip_enable();	
							if (!spi_sr_write(&tx_data[8],tx_data[5] - 1,&rx_data,0))
							{
								rev_protocol[7] = tx_data[5] + 1;			//length
								rev_protocol[11] = tx_data[8];
								os_memcpy(&rev_protocol[12],&tx_data[9],2);
								rx_size = 7 + 1 + 1 + rev_protocol[7];
							}
							else
							{
								rx_size = 1;
							}
						}
						else if (tx_data[7] == 0x0f)/*flash_erase*/
						{
							uint32_t size = 0;
							if (tx_data[8] == 0xd8)/*64K erase*/
							{
								size = 64 * 1024;
							}
							else if (tx_data[8] == 0x52)/*32K erase*/
							{
								size = 32 * 1024;
							}
							else if (tx_data[8] == 0x20)/*4K erase*/
							{
								size = 4 * 1024;
							}
							
							uint32_t addr = 0;
							addr = (tx_data[12] << 24) + (tx_data[11] << 16) + (tx_data[10] << 8) + tx_data[9];
							
							spi_flash_erase_dl(addr, size,0);
							rev_protocol[7] = tx_data[5] + 1;			//length

							rev_protocol[11] = tx_data[8];
							os_memcpy(&rev_protocol[12],&tx_data[9],4);
							rx_size = 16;
						}
						else if (tx_data[7] == 0x0a)/*flash_erase_all*/
						{
							spi_flash_erase_dl(0, 0,1);

							rev_protocol[7] = tx_data[5] + 1;			//length
							rev_protocol[11] = tx_data[8];
							rx_size = 12;
						}
						else if (tx_data[7] == 0x07)/*flash_download_page*/
						{
							uint32_t addr = 0;
							addr = (tx_data[11] << 24) + (tx_data[10] << 16) + (tx_data[9] << 8) + tx_data[8];

							spi_flash_write(addr, 4096, &tx_data[12]);
							rev_protocol[7] = tx_data[5] + 1;			//length
							os_memcpy(&rev_protocol[11],&addr,4);
							rx_size = 15;
						}
						else if (tx_data[7] == 0x09)/*flash_read_page*/
						{
							uint32_t addr = 0;
							addr = (tx_data[11] << 24) + (tx_data[10] << 16) + (tx_data[9] << 8) + tx_data[8];
							rev_protocol[7] = tx_data[5] + 1;			//((1 + 1 + (4 + 4 * 1024)) & 0xff
							rev_protocol[8] = 0x10;						//((1 + 1 + (4 + 4 * 1024)) >> 8 & 0xff
							os_memcpy(&rev_protocol[11],&addr,4);
							for (size_t i = 0; i < 0x1000; i+=0x800)
							{
								uint8_t *buf = (uint8_t *)os_zalloc(0x800);
								spi_flash_read(addr + i,0x800,buf);
								os_memcpy(&rev_protocol[15 + i],buf,0x800);
								if (buf) 
								{
									os_free(buf);
								}
								buf = NULL;
							}
							rx_size = 15 + 4096;
						}
						
					}
					else
					{
						os_memset(rev_protocol, 0, sizeof(rev_protocol));
						bk_ret_protocol(rev_protocol);

						if (tx_data[4] == 0x00)
						{
							// uint32_t uid = spi_connect_reset();
							// if (uid > 0 && uid != 0xffffff)
							// {
							// 	spi_switch_chip_wr_mode(tx_data[5]);
							// 	rev_protocol[2] = 0x05;
							// 	rev_protocol[6] = 0x01;
							// 	rev_protocol[7] = 0x00;
							// 	rx_size = 8;
							// }
							
						}
						else if (tx_data[4] == 0x01)/*reg write*/
						{
							uint32_t reg_addr = 0;
							uint32_t reg_data = 0;
							reg_addr = (tx_data[9] << 24) + (tx_data[8] << 16) + (tx_data[7] << 8) + tx_data[6];
							reg_data = (tx_data[13] << 24) + (tx_data[12] << 16) + (tx_data[11] << 8) + tx_data[10];

							spi_write_chip_register(tx_data[5],reg_addr,reg_data);
							rev_protocol[2] = 0x0d;			//length
							rev_protocol[6] = 0x01;			//read mode word
							rev_protocol[7] = tx_data[5];	//keyword

							rev_protocol[8] = (reg_addr ) & 0xff;
							rev_protocol[9] = (reg_addr >> 8) & 0xff;
							rev_protocol[10] = (reg_addr >> 16) & 0xff;
							rev_protocol[11] = (reg_addr >> 24) & 0xff;

							rev_protocol[12] = (reg_data) & 0xff;
							rev_protocol[13] = (reg_data >> 8) & 0xff;
							rev_protocol[14] = (reg_data >> 16) & 0xff;
							rev_protocol[15] = (reg_data >> 24) & 0xff;
							rx_size = 16;
						}
						else if (tx_data[4] == 0x03)/*reg read*/
						{
							uint32_t reg_addr = 0;
							uint32_t reg_val = 0;
							reg_addr = (tx_data[9] << 24) + (tx_data[8] << 16) + (tx_data[7] << 8) + tx_data[6];
							reg_val = spi_read_chip_register(tx_data[5],reg_addr);

							rev_protocol[2] = 0x0d;			//length
							rev_protocol[6] = 0x03;			//read mode word
							rev_protocol[7] = tx_data[5];	//keyword

							rev_protocol[8] = (reg_addr ) & 0xff;
							rev_protocol[9] = (reg_addr >> 8) & 0xff;
							rev_protocol[10] = (reg_addr >> 16) & 0xff;
							rev_protocol[11] = (reg_addr >> 24) & 0xff;

							rev_protocol[12] = (reg_val) & 0xff;
							rev_protocol[13] = (reg_val >> 8) & 0xff;
							rev_protocol[14] = (reg_val >> 16) & 0xff;
							rev_protocol[15] = (reg_val >> 24) & 0xff;
							rx_size = 16;
						}
                        else if (tx_data[4] == 0x10)/*crc check*/
						{
							uint32_t flash_addr = 0;
							uint32_t flash_length = 0;
							uint32_t crc_val = 0;
                            
							flash_addr = (tx_data[8] << 24) + (tx_data[7] << 16) + (tx_data[6] << 8) + tx_data[5];
							flash_length = (tx_data[12] << 24) + (tx_data[11] << 16) + (tx_data[10] << 8) + tx_data[9];
                            
							crc_val = spi_flash_crc_check(flash_addr,flash_length - flash_addr + 1);

							rev_protocol[2] = 0x08;			//length
							rev_protocol[6] = tx_data[4];	//crc check mode		
							rev_protocol[7] = (crc_val ) & 0xff;
							rev_protocol[8] = (crc_val >> 8) & 0xff;
							rev_protocol[9] = (crc_val >> 16) & 0xff;
							rev_protocol[10] = (crc_val >> 24) & 0xff;
							rx_size = 11;
						}
						else if (tx_data[4] == 0x0f && tx_size == 10)//set_baudrate
						{
							uint32_t baudrate = 0;
							baudrate = (tx_data[8] << 24) + (tx_data[7] << 16) + (tx_data[6] << 8) + tx_data[5];

							uint32_t uid = spi_set_baudrate(baudrate);
							if (uid > 0 && uid != 0xffffff)
							{
								rev_protocol[2] = 0x09;
								rev_protocol[7] = (baudrate ) & 0xff;
								rev_protocol[8] = (baudrate >> 8) & 0xff;
								rev_protocol[9] = (baudrate >> 16) & 0xff;
								rev_protocol[10] = (baudrate >> 24) & 0xff;
								rev_protocol[11] = 0x0f;
								rx_size = 12;
							}
						}
						else if (tx_data[4] == 0xfe)//reboot
						{
							spi_connect_deinit();
							uint32_t uid = spi_connect_init();
							if (uid > 0 && uid != 0xffffff)
							{
								spi_switch_chip_wr_mode(tx_data[5]);
								rev_protocol[2] = 0x05;
								rev_protocol[6] = 0x01;
								rev_protocol[7] = 0x00;
								rx_size = 8;
							}
						}
						else if (tx_data[4] == 0xee)//spi_connect_deinit
						{
							spi_connect_deinit();
						}
						else if (tx_data[4] == 0xff)//tests
						{
							spi_connect_init();
						}
					}
					cdc_write_info(rev_protocol,rx_size);
					
					
					
					
					// for (size_t i = 0; i < tx_size; i++)
					// {
					// 	os_printf("==>%02X \r\n",tx_data[i]);
					// }
					// spi_connect_reset();
				}
			}
		}
    }

}

#endif  // CFG_USB_SPI_DL

