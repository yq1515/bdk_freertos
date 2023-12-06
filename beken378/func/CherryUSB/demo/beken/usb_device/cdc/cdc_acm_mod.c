#include "usbd_core.h"
#include "usbd_cdc.h"


/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

//#define USBD_VID           0xFFFF
//#define USBD_PID           0xFFFF
//#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

/*!< global descriptor */
const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
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
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2048] = { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };

volatile bool ep_tx_busy_flag = false;


/**
  *
  * Licensed to the Apache Software Foundation (ASF) under one or more
  * contributor license agreements.  See the NOTICE file distributed with
  * this work for additional information regarding copyright ownership.  The
  * ASF licenses this file to you under the Apache License, Version 2.0 (the
  * "License"); you may not use this file except in compliance with the
  * License.  You may obtain a copy of the License at
  *
  *   http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
  * License for the specific language governing permissions and limitations
  * under the License.
  *
  */


#ifndef WBVAL
#define WBVAL(x) (unsigned char)((x) & 0xFF), (unsigned char)(((x) >> 8) & 0xFF)
#endif




/*!< USBD CONFIG */
#define USBD_VERSION 0x0110
#define USBD_PRODUCT_VERSION 0x0001
#define USBD_VID 0xffff
#define USBD_PID 0xffff
#define USBD_MAX_POWER 0xfa
#define USBD_LANGID_STRING 1033
#define USBD_CONFIG_DESCRIPTOR_SIZE 107




/*!< USBD ENDPOINT CONFIG */

#define USBD_IF0_AL0_EP0_ADDR 0x01
#define USBD_IF0_AL0_EP0_SIZE 0x40
#define USBD_IF0_AL0_EP0_INTERVAL 0x01

#define USBD_IF1_AL0_EP0_ADDR 0x02
#define USBD_IF1_AL0_EP0_SIZE 0x40
#define USBD_IF1_AL0_EP0_INTERVAL 0x00

#define USBD_IF1_AL0_EP1_ADDR 0x83
#define USBD_IF1_AL0_EP1_SIZE 0x40
#define USBD_IF1_AL0_EP1_INTERVAL 0x00

#define USBD_IF2_AL0_EP0_ADDR 0x01
#define USBD_IF2_AL0_EP0_SIZE 0x40
#define USBD_IF2_AL0_EP0_INTERVAL 0x01

#define USBD_IF2_AL0_EP1_ADDR 0x82
#define USBD_IF2_AL0_EP1_SIZE 0x40
#define USBD_IF2_AL0_EP1_INTERVAL 0x01




/*!< USBD HID CONFIG */
#define USBD_HID_VERSION 0x0111
#define USBD_HID_COUNTRY_CODE 0
#define USBD_IF2_AL0_HID_REPORT_DESC_SIZE 2




/*!< USBD Descriptor */
const unsigned char usbd_descriptor[] = {
/********************************************** Device Descriptor */
    0x12,                                       /*!< bLength */
    0x01,                                       /*!< bDescriptorType */
    WBVAL(USBD_VERSION),                        /*!< bcdUSB */
    0x00,                                       /*!< bDeviceClass */
    0x00,                                       /*!< bDeviceSubClass */
    0x00,                                       /*!< bDeviceProtocol */
    0x40,                                       /*!< bMaxPacketSize */
    WBVAL(USBD_VID),                            /*!< idVendor */
    WBVAL(USBD_PID),                            /*!< idProduct */
    WBVAL(USBD_PRODUCT_VERSION),                /*!< bcdDevice */
    0x01,                                       /*!< iManufacturer */
    0x02,                                       /*!< iProduct */
    0x03,                                       /*!< iSerial */
    0x01,                                       /*!< bNumConfigurations */
/********************************************** Config Descriptor */
    0x09,                                       /*!< bLength */
    0x02,                                       /*!< bDescriptorType */
    WBVAL(USBD_CONFIG_DESCRIPTOR_SIZE),         /*!< wTotalLength */
    0x03,                                       /*!< bNumInterfaces */
    0x01,                                       /*!< bConfigurationValue */
    0x00,                                       /*!< iConfiguration */
    0xa0,                                       /*!< bmAttributes */
    USBD_MAX_POWER,                             /*!< bMaxPower */
/********************************************** Interface Associate Descriptor */
    0x08,                                       /*!< bLength */
    0x0b,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bFirstInterface */
    0x02,                                       /*!< bInterfaceCount */
    0x02,                                       /*!< bFunctionClass */
    0x02,                                       /*!< bFunctionSubClass */
    0x01,                                       /*!< bFunctionProtocol */
    0x00,                                       /*!< iFunction */
/********************************************** Interface 0 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x01,                                       /*!< bNumEndpoints */
    0x02,                                       /*!< bInterfaceClass */
    0x02,                                       /*!< bInterfaceSubClass */
    0x01,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of CDC ACM Control */
    0x05,                                       /*!< bLength */
    0x24,                                       /*!< bDescriptorType */
    0x00,                                       /*!< bDescriptorSubtype */
    WBVAL(0x0110),                              /*!< bcdCDC */
    0x05,                                       /*!< bLength */
    0x24,                                       /*!< bDescriptorType */
    0x01,                                       /*!< bDescriptorSubtype */
    0x00,                                       /*!< bmCapabilities */
    0x01,                                       /*!< bDataInterface */
    0x04,                                       /*!< bLength */
    0x24,                                       /*!< bDescriptorType */
    0x02,                                       /*!< bDescriptorSubtype */
    0x02,                                       /*!< bmCapabilities */
    0x05,                                       /*!< bLength */
    0x24,                                       /*!< bDescriptorType */
    0x06,                                       /*!< bDescriptorSubtype */
    0x00,                                       /*!< bMasterInterface */
    0x01,                                       /*!< bSlaveInterface0 */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF0_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF0_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF0_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 1 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x01,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x02,                                       /*!< bNumEndpoints */
    0x0a,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of CDC ACM Data */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF1_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF1_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF1_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Endpoint 1 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF1_AL0_EP1_ADDR,                      /*!< bEndpointAddress */
    0x02,                                       /*!< bmAttributes */
    WBVAL(USBD_IF1_AL0_EP1_SIZE),               /*!< wMaxPacketSize */
    USBD_IF1_AL0_EP1_INTERVAL,                  /*!< bInterval */
/********************************************** Interface 2 Alternate 0 Descriptor */
    0x09,                                       /*!< bLength */
    0x04,                                       /*!< bDescriptorType */
    0x02,                                       /*!< bInterfaceNumber */
    0x00,                                       /*!< bAlternateSetting */
    0x02,                                       /*!< bNumEndpoints */
    0x03,                                       /*!< bInterfaceClass */
    0x00,                                       /*!< bInterfaceSubClass */
    0x00,                                       /*!< bInterfaceProtocol */
    0x00,                                       /*!< iInterface */
/********************************************** Class Specific Descriptor of HID */
    0x09,                                       /*!< bLength */
    0x21,                                       /*!< bDescriptorType */
    WBVAL(USBD_HID_VERSION),                    /*!< bcdHID */
    USBD_HID_COUNTRY_CODE,                      /*!< bCountryCode */
    0x01,                                       /*!< bNumDescriptors */
    0x22,                                       /*!< bDescriptorType */
    WBVAL(USBD_IF2_AL0_HID_REPORT_DESC_SIZE),   /*!< wItemLength */
/********************************************** Endpoint 0 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF2_AL0_EP0_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF2_AL0_EP0_SIZE),               /*!< wMaxPacketSize */
    USBD_IF2_AL0_EP0_INTERVAL,                  /*!< bInterval */
/********************************************** Endpoint 1 Descriptor */
    0x07,                                       /*!< bLength */
    0x05,                                       /*!< bDescriptorType */
    USBD_IF2_AL0_EP1_ADDR,                      /*!< bEndpointAddress */
    0x03,                                       /*!< bmAttributes */
    WBVAL(USBD_IF2_AL0_EP1_SIZE),               /*!< wMaxPacketSize */
    USBD_IF2_AL0_EP1_INTERVAL,                  /*!< bInterval */
/********************************************** Language ID String Descriptor */
    0x04,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    WBVAL(USBD_LANGID_STRING),                  /*!< wLangID0 */
/********************************************** String 1 Descriptor */
/* Bekencorp */
    0x14,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x42, 0x00,                                 /*!< 'B' wcChar0 */
    0x65, 0x00,                                 /*!< 'e' wcChar1 */
    0x6b, 0x00,                                 /*!< 'k' wcChar2 */
    0x65, 0x00,                                 /*!< 'e' wcChar3 */
    0x6e, 0x00,                                 /*!< 'n' wcChar4 */
    0x63, 0x00,                                 /*!< 'c' wcChar5 */
    0x6f, 0x00,                                 /*!< 'o' wcChar6 */
    0x72, 0x00,                                 /*!< 'r' wcChar7 */
    0x70, 0x00,                                 /*!< 'p' wcChar8 */
/********************************************** String 2 Descriptor */
/* USB Tools */
    0x14,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x55, 0x00,                                 /*!< 'U' wcChar0 */
    0x53, 0x00,                                 /*!< 'S' wcChar1 */
    0x42, 0x00,                                 /*!< 'B' wcChar2 */
    0x20, 0x00,                                 /*!< ' ' wcChar3 */
    0x54, 0x00,                                 /*!< 'T' wcChar4 */
    0x6f, 0x00,                                 /*!< 'o' wcChar5 */
    0x6f, 0x00,                                 /*!< 'o' wcChar6 */
    0x6c, 0x00,                                 /*!< 'l' wcChar7 */
    0x73, 0x00,                                 /*!< 's' wcChar8 */
/********************************************** String 3 Descriptor */
/* 9527 */
    0x0a,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x39, 0x00,                                 /*!< '9' wcChar0 */
    0x35, 0x00,                                 /*!< '5' wcChar1 */
    0x32, 0x00,                                 /*!< '2' wcChar2 */
    0x37, 0x00,                                 /*!< '7' wcChar3 */
    0x00
};




/*!< USBD HID REPORT 0 Descriptor */
const unsigned char usbd_hid_0_report_descriptor[USBD_IF2_AL0_HID_REPORT_DESC_SIZE] = {
    0x00,
    0xC0    /* END_COLLECTION */
};



/*******************************************************END OF FILE*************/





#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

void usbd_configure_done_callback(void)
{
    /* setup first out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);
}

// void print_hex_dump(const char *prefix, const void *buf, int len);

void usbd_cdc_acm_bulk_out(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual out len:%d\r\n", nbytes);
    // for (int i = 0; i < 100; i++) {
    //     printf("%02x ", read_buffer[i]);
    // }
    // printf("\r\n");
    /* setup next out ep read transfer */
    usbd_ep_start_read(CDC_OUT_EP, read_buffer, 2048);

    print_hex_dump("OUT: ", read_buffer, nbytes);
    os_printf("\n");
}

void usbd_cdc_acm_bulk_in(uint8_t ep, uint32_t nbytes)
{
    USB_LOG_RAW("actual in len:%d\r\n", nbytes);

#if 0
    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(CDC_IN_EP, NULL, 0);
    } else {
        ep_tx_busy_flag = false;
    }
#endif
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_interface intf0;
struct usbd_interface intf1;

void cdc_acm_init(void)
{
    // usbd_desc_register(cdc_descriptor);
    usbd_desc_register(usbd_descriptor);
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));
    usbd_add_endpoint(&cdc_out_ep);
    usbd_add_endpoint(&cdc_in_ep);
    usbd_initialize();
}

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    if (dtr) {
        dtr_enable = 1;
    } else {
        dtr_enable = 0;
    }
}

// void print_hex_dump(const char *prefix, const void *buf, int len);
void cdc_acm_data_send_with_dtr_test(void)
{
    if (dtr_enable) {
    	os_printf("%s %d\n", __func__, __LINE__);
    	print_hex_dump("IN: ", write_buffer, 10);
        memset(&write_buffer[10], 'a', 2038);
        ep_tx_busy_flag = true;
        usbd_ep_start_write(CDC_IN_EP, write_buffer, 2048);
        //while (ep_tx_busy_flag) {
        //}
    } else {
    	os_printf("%s %d\n", __func__, __LINE__);
	    usbd_ep_start_write(CDC_IN_EP, write_buffer, 2048);
    }
}
