#ifndef _BK_SPI_FLASH_H_
#define _BK_SPI_FLASH_H_

typedef uint8_t spi_unit_t; /**< spi uint id */

typedef enum {
	SPI_ID_0 = 0, /**< SPI id 1 */
#if (SOC_SPI_UNIT_NUM > 1)
	SPI_ID_1,     /**< SPI id 2 */
#endif
#if (SOC_SPI_UNIT_NUM > 2)
	SPI_ID_2,     /**< SPI id 3 */
#endif
	SPI_ID_MAX    /**< SPI id max */
} spi_id_t;

typedef enum {
	SPI_ROLE_SLAVE = 0, /**< SPI as slave */
	SPI_ROLE_MASTER,    /**< SPI as master */
} spi_role_t;

typedef enum {
	SPI_BIT_WIDTH_8BITS = 0, /**< SPI bit width 8bits */
	SPI_BIT_WIDTH_16BITS,    /**< SPI bit width 16bits */
} spi_bit_width_t;

typedef enum {
	SPI_POLARITY_LOW = 0, /**< SPI clock polarity low */
	SPI_POLARITY_HIGH,    /**< SPI clock polarity high */
} spi_polarity_t;

typedef enum {
	SPI_PHASE_1ST_EDGE = 0, /**< SPI clock phase the first edge */
	SPI_PHASE_2ND_EDGE,     /**< SPI clock phase the second edge */
} spi_phase_t;

typedef enum {
	SPI_POL_MODE_0 = 0, /**< SPI mode 0 */
	SPI_POL_MODE_1,     /**< SPI mode 1 */
	SPI_POL_MODE_2,     /**< SPI mode 2 */
	SPI_POL_MODE_3,     /**< SPI mode 3 */
} spi_mode_t;

typedef enum {
	SPI_4WIRE_MODE = 0, /**< SPI four wire mode */
	SPI_3WIRE_MODE,     /**< SPI three wire mode */
} spi_wire_mode_t;

typedef enum {
	SPI_MSB_FIRST = 0, /**< SPI MSB first */
	SPI_LSB_FIRST,     /**< SPI LSB first */
} spi_bit_order_t;

typedef enum {
	SPI_FIFO_INT_LEVEL_1 = 0, /**< SPI fifo int level 1 */
	SPI_FIFO_INT_LEVEL_16,    /**< SPI fifo int level 16 */
	SPI_FIFO_INT_LEVEL_32,    /**< SPI fifo int level 32 */
	SPI_FIFO_INT_LEVEL_48,    /**< SPI fifo int level 48 */
} spi_fifo_int_level;

typedef enum {
	SPI_CLK_XTAL = 0,
	SPI_CLK_APLL,
	SPI_CLK_UNKNOW = 0xff
} spi_src_clk_t;

int spi_flash_init(void);
uint32_t spi_connect_init(void);
void spi_flash_deinit(void);
uint32_t spi_flash_read_id(void);
void spi_flash_send_command(uint8_t cmd);
int spi_flash_read(uint32_t addr, uint32_t size, uint8_t *dst);
int spi_flash_write(uint32_t addr, uint32_t size, uint8_t *src);
int spi_flash_erase_dl(uint32_t addr, uint32_t size,uint32_t erase_mode);
void spi_flash_protect(void);
void spi_flash_unprotect(void);

#endif //_BK_SPI_FLASH_H_