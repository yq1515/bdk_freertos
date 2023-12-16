// https://github.com/harbaum/I2C-Tiny-USB/
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "uart_pub.h"


// map struct usb_setup_packet to i2c_cmd
struct i2c_cmd {
    unsigned char    type;
    unsigned char    cmd;
    unsigned short    flags;
    unsigned short    addr;
    unsigned short    len;
} __PACKED;


/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

//#define USBD_VID           0x2A57
//#define USBD_PID           0x7231
#define USBD_VID           0x0403
#define USBD_PID           0xc631

#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + 9 /* + 7 */)

#define USB_I2C_IN_EP     0x81
#define USB_I2C_EP_SIZE   64

#define STATUS_IDLE          0
#define STATUS_ADDRESS_ACK   1
#define STATUS_ADDRESS_NAK   2


/* commands from USB, must e.g. match command ids in kernel driver */
#define CMD_ECHO       0
#define CMD_GET_FUNC   1
#define CMD_SET_DELAY  2
#define CMD_GET_STATUS 3

#define CMD_I2C_IO     4
#define CMD_I2C_BEGIN  1    // flag fo I2C_IO
#define CMD_I2C_END    2    // flag fo I2C_IO

/* linux kernel flags */
#define I2C_M_TEN       0x10    /* we have a ten bit chip address */
#define I2C_M_RD        0x01
#define I2C_M_NOSTART       0x4000
#define I2C_M_REV_DIR_ADDR  0x2000
#define I2C_M_IGNORE_NAK    0x1000
#define I2C_M_NO_RD_ACK     0x0800

/* To determine what functionality is present */
#define I2C_FUNC_I2C            0x00000001
#define I2C_FUNC_10BIT_ADDR     0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING  0x00000004          /* I2C_M_{REV_DIR_ADDR,NOSTART,..} */
#define I2C_FUNC_SMBUS_HWPEC_CALC   0x00000008          /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_READ_WORD_DATA_PEC  0x00000800   /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA_PEC 0x00001000   /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_PROC_CALL_PEC    0x00002000      /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL_PEC 0x00004000   /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL  0x00008000      /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_QUICK        0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE    0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE   0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA   0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA  0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA   0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA  0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL    0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA  0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA 0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK   0x04000000      /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK  0x08000000      /* w/ 1-byte reg. addr. */
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK_2  0x10000000     /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK_2 0x20000000     /* w/ 2-byte reg. addr. */
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA_PEC  0x40000000  /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA_PEC 0x80000000  /* SMBus 2.0 */

#define I2C_FUNC_SMBUS_BYTE I2C_FUNC_SMBUS_READ_BYTE | \
    I2C_FUNC_SMBUS_WRITE_BYTE
#define I2C_FUNC_SMBUS_BYTE_DATA I2C_FUNC_SMBUS_READ_BYTE_DATA | \
    I2C_FUNC_SMBUS_WRITE_BYTE_DATA
#define I2C_FUNC_SMBUS_WORD_DATA I2C_FUNC_SMBUS_READ_WORD_DATA | \
    I2C_FUNC_SMBUS_WRITE_WORD_DATA
#define I2C_FUNC_SMBUS_BLOCK_DATA I2C_FUNC_SMBUS_READ_BLOCK_DATA | \
    I2C_FUNC_SMBUS_WRITE_BLOCK_DATA
#define I2C_FUNC_SMBUS_I2C_BLOCK I2C_FUNC_SMBUS_READ_I2C_BLOCK | \
    I2C_FUNC_SMBUS_WRITE_I2C_BLOCK

#define I2C_FUNC_SMBUS_EMUL I2C_FUNC_SMBUS_QUICK | \
    I2C_FUNC_SMBUS_BYTE | \
    I2C_FUNC_SMBUS_BYTE_DATA | \
    I2C_FUNC_SMBUS_WORD_DATA | \
    I2C_FUNC_SMBUS_PROC_CALL | \
    I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | \
    I2C_FUNC_SMBUS_WRITE_BLOCK_DATA_PEC | \
    I2C_FUNC_SMBUS_I2C_BLOCK

/* the currently support capability is quite limited */
const unsigned long i2c_func = I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;



/*!< global descriptor */
static const uint8_t usb_i2c_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_1_1, 0xFF, 0x00, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /************** Descriptor of Custom interface *****************/
    0x09,                                       /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE,              /* bDescriptorType: Interface descriptor type */
    0x00,                                       /* bInterfaceNumber: Number of Interface */
    0x00,                                       /* bAlternateSetting: Alternate setting */
    0x00,                                       /* bNumEndpoints */
    0xff,                                       /* bInterfaceClass: Vendor */
    0x00,                                       /* bInterfaceSubClass : */
    0x00,                                       /* nInterfaceProtocol : */
    0,                                          /* iInterface: Index of string descriptor */

#if 0
    /******************** Descriptor of Custom in endpoint ********************/
    0x07,                                       /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT,               /* bDescriptorType: */
    USB_I2C_IN_EP,                              /* bEndpointAddress: Endpoint Address (IN) */
    0x02,                                       /* bmAttributes: Bulk endpoint */
    WBVAL(USB_I2C_EP_SIZE),                     /* wMaxPacketSize: 4 Byte max */
    0,                                          /* bInterval: Polling Interval */
#endif

    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x18,                                               /* bLength */
    USB_DESCRIPTOR_TYPE_STRING,                         /* bDescriptorType */
    'B',                                        0x00,   /* wcChar0 */
    'e',                                        0x00,   /* wcChar1 */
    'k',                                        0x00,   /* wcChar2 */
    'e',                                        0x00,   /* wcChar3 */
    'n',                                        0x00,   /* wcChar4 */
    ',',                                        0x00,   /* wcChar5 */
    ' ',                                        0x00,   /* wcChar6 */
    'I',                                        0x00,   /* wcChar7 */
    'n',                                        0x00,   /* wcChar8 */
    'c',                                        0x00,   /* wcChar9 */
    '.',                                        0x00,   /* wcChar10 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x10,                                               /* bLength */
    USB_DESCRIPTOR_TYPE_STRING,                         /* bDescriptorType */
    'U',                                        0x00,   /* wcChar0 */
    'S',                                        0x00,   /* wcChar1 */
    'B',                                        0x00,   /* wcChar2 */
    '2',                                        0x00,   /* wcChar3 */
    'I',                                        0x00,   /* wcChar4 */
    'I',                                        0x00,   /* wcChar5 */
    'C',                                        0x00,   /* wcChar6 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                                               /* bLength */
    USB_DESCRIPTOR_TYPE_STRING,                         /* bDescriptorType */
    '2',                                        0x00,   /* wcChar0 */
    '0',                                        0x00,   /* wcChar1 */
    '2',                                        0x00,   /* wcChar2 */
    '3',                                        0x00,   /* wcChar3 */
    '1',                                        0x00,   /* wcChar4 */
    '2',                                        0x00,   /* wcChar5 */
    '1',                                        0x00,   /* wcChar6 */
    '3',                                        0x00,   /* wcChar7 */
    '2',                                        0x00,   /* wcChar8 */
    '0',                                        0x00,   /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048];

void usbd_event_handler(uint8_t event)
{
    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        /* setup first out ep read transfer */
        // usbd_ep_start_read(USB_I2C_OUT_EP, read_buffer, 2048);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

static struct usbd_interface intf0;

static void i2c_do(struct i2c_cmd *cmd, uint8_t **data, uint32_t *len)
{
#if 0
    uchar addr;

    LED_PORT |= LED_BV;

    DEBUGF("i2c %s at 0x%02x, len = %d\n", (cmd->flags & I2C_M_RD)?"rd":"wr", cmd->addr, cmd->len);

    /* normal 7bit address */
    addr = (cmd->addr << 1);
    if (cmd->flags & I2C_M_RD)
        addr |= 1;

    if (cmd->cmd & CMD_I2C_BEGIN)
        i2c_start();
    else
        i2c_repstart();

    // send DEVICE address
    if (!i2c_put_u08(addr)) {
        DEBUGF("I2C read: address error @ %x\n", addr);

        status = STATUS_ADDRESS_NAK;
        expected = 0;
        i2c_stop();
    } else {
        status = STATUS_ADDRESS_ACK;
        expected = cmd->len;
        saved_cmd = cmd->cmd;

        /* check if transfer is already done (or failed) */
        if ((cmd->cmd & CMD_I2C_END) && !expected)
            i2c_stop();
    }

    /* more data to be expected? */

    LED_PORT &= ~LED_BV;

    return cmd->len ? 0xff:0x00;
#endif
}

// @setup: SETUP packet
//         bRequest: CMD_XXX
//         wValue: I2C_M_XX
//         wIndex: I2C Slave address, either 7 or 10 bits.
// @data: reply data buf pointer
static int usbd_i2c_vendor_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    print_hex_dump("", setup, sizeof(*setup));

    switch (setup->bRequest) {
    case CMD_ECHO: // echo (for transfer reliability testing)
        (*data)[0] = setup->wValue;
        (*data)[1] = setup->wIndex;
        *len = 2;
        break;

    case CMD_GET_FUNC:
        os_memcpy(*data, &i2c_func, sizeof(i2c_func));
        *len = sizeof(i2c_func);
        break;

    case CMD_SET_DELAY:
#if 0
        /* The original device used a 12MHz clock: */
        /* --- */
        /* The delay function used delays 4 system ticks per cycle. */
        /* This gives 1/3us at 12Mhz per cycle. The delay function is */
        /* called twice per clock edge and thus four times per full cycle. */
        /* Thus it is called one time per edge with the full delay */
        /* value and one time with the half one. Resulting in */
        /* 2 * n * 1/3 + 2 * 1/2 n * 1/3 = n us. */
        /* --- */
        /* On littleWire-like devices, we run at 16.5MHz, so delay must be scaled up. */
    {
        uint32_t delay = *(unsigned short *)(data + 2);
        delay = (delay * 1650UL + 600UL) / 1200UL;
        clock_delay = delay;
        if (clock_delay <= DELAY_OVERHEAD)
            clock_delay = 1;
        else
            clock_delay = clock_delay - DELAY_OVERHEAD;
        clock_delay2 = clock_delay / 2;
        if (!clock_delay2) clock_delay2 = 1;
    }

        DEBUGF("request for delay %dus\n", clock_delay);
#endif
        *len = 0;
        break;

    case CMD_I2C_IO:
    case CMD_I2C_IO + CMD_I2C_BEGIN:
    case CMD_I2C_IO + CMD_I2C_END:
    case CMD_I2C_IO + CMD_I2C_BEGIN + CMD_I2C_END:
        // these are only allowed as class transfers

        i2c_do((struct i2c_cmd *)setup, data, len);
        if ((setup->bmRequestType & USB_REQUEST_DIR_MASK) == USB_REQUEST_DIR_IN)
            *len = setup->wLength;
        break;

    case CMD_GET_STATUS:
        (*data)[0] = STATUS_ADDRESS_ACK;
        *len = 1;
        break;

    default:
        // must not happen ...
        break;
    }

    return 0;
}

static struct usbd_interface *usbd_i2c_init_intf(struct usbd_interface *intf)
{
    intf->class_interface_handler = NULL;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = usbd_i2c_vendor_handler;
    intf->notify_handler = NULL;

    return intf;
}

void usb_i2c_init(void)
{
    usbd_desc_register(usb_i2c_descriptor);
    usbd_add_interface(usbd_i2c_init_intf(&intf0));
    usbd_initialize();
}

