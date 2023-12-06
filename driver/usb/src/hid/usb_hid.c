/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/
#include "include.h"

#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_impl.h"
#include "mu_stdio.h"
#include "mu_strng.h"
#include "mu_hfi.h"
#include "mu_spi.h"
#include "mu_none.h"
#include "mu_hidif.h"
#include "mu_kii.h"
#include "mu_nci.h"

#if CFG_SUPPORT_HID
static void MGC_HidNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State);
static void MGC_HidOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus);

/**************************** GLOBALS *****************************/
static MUSB_Port *MGC_pCdiPort = NULL;
static MUSB_BusHandle MGC_hCdiBus = NULL;
static uint8_t MGC_bDesireHostRole = TRUE;
static uint8_t MGC_aHidPeripheralList[256];
static MUSB_DeviceDriver MGC_aDeviceDriver[2];
static uint8_t MGC_aControlData[64];//[256];
static uint8_t MGC_bHidSelfPowered = TRUE;

static MUSB_HostClient MGC_HidHostClient =
{
    MGC_aHidPeripheralList,		/* peripheral list */
    0,			    /* filled in main */
    MGC_aDeviceDriver,
    0					/* filled in main */
};

static MUSB_OtgClient MGC_HidOtgClient =
{
    NULL,	/* no instance data; we are singleton */
    &MGC_bDesireHostRole,
    MGC_HidNewOtgState,
    MGC_HidOtgError
};

/*!< hidraw in endpoint */
#define HIDRAW_IN_EP       0x81
#define HIDRAW_IN_SIZE     64
#define HIDRAW_IN_INTERVAL 10

/*!< hidraw out endpoint */
#define HIDRAW_OUT_EP          0x02
#define HIDRAW_OUT_EP_SIZE     64
#define HIDRAW_OUT_EP_INTERVAL 10

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     500
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_HID_CONFIG_DESC_SIZ (9 + 9 + 9 + 7 + 7)

/* USB Descriptor Types */
#define USB_DESCRIPTOR_TYPE_DEVICE                0x01U
#define USB_DESCRIPTOR_TYPE_CONFIGURATION         0x02U
#define USB_DESCRIPTOR_TYPE_STRING                0x03U
#define USB_DESCRIPTOR_TYPE_INTERFACE             0x04U
#define USB_DESCRIPTOR_TYPE_ENDPOINT              0x05U
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER      0x06U
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED           0x07U
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER       0x08U
#define USB_DESCRIPTOR_TYPE_OTG                   0x09U
#define USB_DESCRIPTOR_TYPE_DEBUG                 0x0AU
#define USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION 0x0BU
#define USB_DESCRIPTOR_TYPE_BINARY_OBJECT_STORE   0x0FU
#define USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY     0x10U
#define USB_DESCRIPTOR_TYPE_WIRELESS_ENDPOINTCOMP 0x11U

/* usb string index define */
#define USB_STRING_LANGID_INDEX    0x00
#define USB_STRING_MFC_INDEX       0x01
#define USB_STRING_PRODUCT_INDEX   0x02
#define USB_STRING_SERIAL_INDEX    0x03
#define USB_STRING_CONFIG_INDEX    0x04
#define USB_STRING_INTERFACE_INDEX 0x05
#define USB_STRING_OS_INDEX        0x06
#define USB_STRING_MAX             USB_STRING_OS_INDEX


/*!< custom hid report descriptor size */
#define HID_CUSTOM_REPORT_DESC_SIZE 34

#define WBVAL(x) (x & 0xFF), ((x >> 8) & 0xFF)
#define DBVAL(x) (x & 0xFF), ((x >> 8) & 0xFF), ((x >> 16) & 0xFF), ((x >> 24) & 0xFF)

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA) ((mA) / 2)


/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_REMOTE_WAKEUP 0x20
#define USB_CONFIG_POWERED_MASK  0x40
#define USB_CONFIG_BUS_POWERED   0x80
#define USB_CONFIG_SELF_POWERED  0xC0

/* Useful define */
#define USB_1_1 0x0110
#define USB_2_0 0x0200
/* Set USB version to 2.1 so that the host will request the BOS descriptor */
#define USB_2_1 0x0210

		/* HID Class Descriptor Types (HID 7.1) */
#define HID_DESCRIPTOR_TYPE_HID          0x21
#define HID_DESCRIPTOR_TYPE_HID_REPORT   0x22
#define HID_DESCRIPTOR_TYPE_HID_PHYSICAL 0x23

#define USB_LANGID_INIT(id)                           \
			0x04,							/* bLength */	  \
			USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */ \
			WBVAL(id)					/* wLangID0 */

#define USB_CONFIG_DESCRIPTOR_INIT(wTotalLength, bNumInterfaces, bConfigurationValue, bmAttributes, bMaxPower) \
		0x09,							   /* bLength */													   \
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* bDescriptorType */											   \
		WBVAL(wTotalLength),			   /* wTotalLength */												   \
		bNumInterfaces, 				   /* bNumInterfaces */ 											   \
		bConfigurationValue,			   /* bConfigurationValue */										   \
		0x00,							   /* iConfiguration */ 											   \
		bmAttributes,					   /* bmAttributes */												   \
		USB_CONFIG_POWER_MA(bMaxPower)	   /* bMaxPower */

#define USB_DEVICE_DESCRIPTOR_INIT(bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol, idVendor, idProduct, bcdDevice, bNumConfigurations) \
    0x12,                       /* bLength */                                                                                              \
    USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */                                                                                      \
    WBVAL(bcdUSB),              /* bcdUSB */                                                                                               \
    bDeviceClass,               /* bDeviceClass */                                                                                         \
    bDeviceSubClass,            /* bDeviceSubClass */                                                                                      \
    bDeviceProtocol,            /* bDeviceProtocol */                                                                                      \
    0x40,                       /* bMaxPacketSize */                                                                                       \
    WBVAL(idVendor),            /* idVendor */                                                                                             \
    WBVAL(idProduct),           /* idProduct */                                                                                            \
    WBVAL(bcdDevice),           /* bcdDevice */                                                                                            \
    USB_STRING_MFC_INDEX,       /* iManufacturer */                                                                                        \
    USB_STRING_PRODUCT_INDEX,   /* iProduct */                                                                                             \
    USB_STRING_SERIAL_INDEX,    /* iSerial */                                                                                              \
    bNumConfigurations          /* bNumConfigurations */

static const uint8_t MGC_aDescriptorData[] = {
	USB_DEVICE_DESCRIPTOR_INIT(USB_1_1, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0002, 0x01),
	USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
	/************** Descriptor of Custom interface *****************/
	0x09,                          /* bLength: Interface Descriptor size */
	USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
	0x00,                          /* bInterfaceNumber: Number of Interface */
	0x00,                          /* bAlternateSetting: Alternate setting */
	0x02,                          /* bNumEndpoints */
	0x03,                          /* bInterfaceClass: HID */
	0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
	0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
	0,                             /* iInterface: Index of string descriptor */
	/******************** Descriptor of Custom HID ********************/
	0x09,                    /* bLength: HID Descriptor size */
	HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
	0x11,                    /* bcdHID: HID Class Spec release number */
	0x01,
	0x00,                        /* bCountryCode: Hardware target country */
	0x01,                        /* bNumDescriptors: Number of HID class descriptors to follow */
	0x22,                        /* bDescriptorType */
	HID_CUSTOM_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
	0x00,
	/******************** Descriptor of Custom in endpoint ********************/
	0x07,                         /* bLength: Endpoint Descriptor size */
	USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
	HIDRAW_IN_EP,                 /* bEndpointAddress: Endpoint Address (IN) */
	0x03,                         /* bmAttributes: Interrupt endpoint */
	WBVAL(HIDRAW_IN_SIZE),        /* wMaxPacketSize: 4 Byte max */
	HIDRAW_IN_INTERVAL,           /* bInterval: Polling Interval */
	/******************** Descriptor of Custom out endpoint ********************/
	0x07,                         /* bLength: Endpoint Descriptor size */
	USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
	HIDRAW_OUT_EP,                /* bEndpointAddress: Endpoint Address (IN) */
	0x03,                         /* bmAttributes: Interrupt endpoint */
	WBVAL(HIDRAW_OUT_EP_SIZE),    /* wMaxPacketSize: 4 Byte max */
	HIDRAW_OUT_EP_INTERVAL,       /* bInterval: Polling Interval */
	/* 73 */
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
	'H', 0x00,                  /* wcChar10 */
	'I', 0x00,                  /* wcChar11 */
	'D', 0x00,                  /* wcChar12 */
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


	0x00
};

static const uint8_t MGC_aHighSpeedDescriptorData[] =
{
    /* device qualifier */
    0x0A,                      /* bLength              */
    MUSB_DT_DEVICE_QUALIFIER,  /* DEVICE Qualifier     */
    0x01,0x01,                 /* USB 1.1              */
    0,                         /* CLASS                */
    0,                         /* Subclass             */
    0x00,                      /* Protocol             */
    0x40,                      /* bMaxPacketSize0      */
    0x01,                      /* One configuration    */
    0x00,                      /* bReserved            */

	//USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
	0x09,							   /* bLength */
	USB_DESCRIPTOR_TYPE_OTHER_SPEED, /* bDescriptorType */
	WBVAL(USB_HID_CONFIG_DESC_SIZ),			   /* wTotalLength */
	0x01, 				   /* bNumInterfaces */
	0x01,			   /* bConfigurationValue */
	0x00,							   /* iConfiguration */
	USB_CONFIG_BUS_POWERED,					   /* bmAttributes */
	USB_CONFIG_POWER_MA(USBD_MAX_POWER),	   /* bMaxPower */

	/************** Descriptor of Custom interface *****************/
	0x09,                          /* bLength: Interface Descriptor size */
	USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
	0x00,                          /* bInterfaceNumber: Number of Interface */
	0x00,                          /* bAlternateSetting: Alternate setting */
	0x02,                          /* bNumEndpoints */
	0x03,                          /* bInterfaceClass: HID */
	0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
	0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
	0,                             /* iInterface: Index of string descriptor */
	/******************** Descriptor of Custom HID ********************/
	0x09,                    /* bLength: HID Descriptor size */
	HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
	0x11,                    /* bcdHID: HID Class Spec release number */
	0x01,
	0x00,                        /* bCountryCode: Hardware target country */
	0x01,                        /* bNumDescriptors: Number of HID class descriptors to follow */
	0x22,                        /* bDescriptorType */
	HID_CUSTOM_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
	0x00,
	/******************** Descriptor of Custom in endpoint ********************/
	0x07,                         /* bLength: Endpoint Descriptor size */
	USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
	HIDRAW_IN_EP,                 /* bEndpointAddress: Endpoint Address (IN) */
	0x03,                         /* bmAttributes: Interrupt endpoint */
	WBVAL(HIDRAW_IN_SIZE),        /* wMaxPacketSize: 4 Byte max */
	HIDRAW_IN_INTERVAL,           /* bInterval: Polling Interval */
	/******************** Descriptor of Custom out endpoint ********************/
	0x07,                         /* bLength: Endpoint Descriptor size */
	USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
	HIDRAW_OUT_EP,                /* bEndpointAddress: Endpoint Address (IN) */
	0x03,                         /* bmAttributes: Interrupt endpoint */
	WBVAL(HIDRAW_OUT_EP_SIZE),    /* wMaxPacketSize: 4 Byte max */
	HIDRAW_OUT_EP_INTERVAL,       /* bInterval: Polling Interval */

};

uint8_t gHidMouseReportDescriptor[] = {
    /* USER CODE BEGIN 0 */
    0x06, 0x00, 0xff, // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,       // USAGE (Vendor Usage 1)
    0xa1, 0x01,       // COLLECTION (Application)
    0x09, 0x01,       //   USAGE (Vendor Usage 1)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
    0x95, 0x40,       //   REPORT_COUNT (64)
    0x75, 0x08,       //   REPORT_SIZE (8)
    0x81, 0x02,       //   INPUT (Data,Var,Abs)
    /* <___________________________________________________> */
    0x09, 0x01,       //   USAGE (Vendor Usage 1)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
    0x95, 0x40,       //   REPORT_COUNT (64)
    0x75, 0x08,       //   REPORT_SIZE (8)
    0x91, 0x02,       //   OUTPUT (Data,Var,Abs)
    /* USER CODE END 0 */
    0xC0 /*     END_COLLECTION	             */
};

int ulgHidMouseReportDescriptorLen = sizeof(gHidMouseReportDescriptor);

static uint8_t MGC_HidDeviceRequest(void *hClient, MUSB_BusHandle hBus,
                                    uint32_t dwSequence, const uint8_t *pSetup,
                                    uint16_t wLength);
static uint8_t MGC_HidDeviceConfigSelected(void *hClient, MUSB_BusHandle hBus,
        uint8_t bConfigurationValue,
        MUSB_PipePtr *ahPipe);
static void MGC_HidNewUsbState(void* hClient, MUSB_BusHandle hBus,
			       MUSB_State State);

MUSB_FunctionClient MGC_HidFunctionClient =
{
    NULL,   /* no instance data; we are singleton */
    MGC_aDescriptorData,
    sizeof(MGC_aDescriptorData),
    3,      /* # strings per language */
    MGC_aHighSpeedDescriptorData,
    sizeof(MGC_aHighSpeedDescriptorData),
    sizeof(MGC_aControlData),
    MGC_aControlData,
    &MGC_bHidSelfPowered,
    MGC_HidDeviceRequest,
    MGC_HidDeviceConfigSelected,
    NULL,
    MGC_HidNewUsbState
};


static MUSB_HfiDevice *MGC_pHfiDevice = NULL;
static uint8_t MediaIsOk = FALSE;
// static MUSB_LinkedList MGC_TestNciPortList;
static uint32_t MGC_HidTxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp);
static uint32_t MGC_HidRxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp);

uint8_t rxBuff[256];

/** IRP for data transmission */
static MUSB_Irp MGC_HidTxDataIrp =
{
    NULL,
    NULL,
    0,
    0,
    0,
    MGC_HidTxDataComplete,
    NULL,
    FALSE,	/* bAllowShortTransfer */
    TRUE,	/* bIsrCallback */
    FALSE	/* bAllowDma */
};

/** IRP for data reception */
static MUSB_Irp MGC_HidRxDataIrp =
{
    NULL,
    rxBuff,
    sizeof(rxBuff),
    0,
    0,
    MGC_HidRxDataComplete,
    NULL,
    FALSE,	/* bAllowShortTransfer */
    TRUE,	/* bIsrCallback */
    FALSE	/* bAllowDma */
};



/* HID Class Specific Requests (HID 7.2) */
#define HID_REQUEST_GET_REPORT   0x01
#define HID_REQUEST_GET_IDLE     0x02
#define HID_REQUEST_GET_PROTOCOL 0x03
#define HID_REQUEST_SET_REPORT   0x09
#define HID_REQUEST_SET_IDLE     0x0A
#define HID_REQUEST_SET_PROTOCOL 0x0B

/** data transmit complete callback */
static uint32_t MGC_HidTxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp)
{
    os_printf("MGC_MsdTxDataComplete\r\n");
#if 0
    /* data sent; send status */
    /* TODO: check for data send error */

	if ((bcmdread == 1) && (MGC_Csw.dCswDataResidue > 0))
	{
		uint8_t *ptr = (uint8_t *)pCompleteParam;
		if(ptr && (1 == *ptr))//continue read card
		{
			MGC_MsdTxDataIrp.pCompleteParam = NULL;
			usb_event_post(MSG_CMD_USB_READ,NULL,USB_NODE_DATA_SIZE);
		}
	}
	else
	{
	    /* send CSW */
	    MGC_MsdTxCswIrp.dwActualLength = 0L;
	    MUSB_DIAG_STRING(3, "MSD: Starting CSW Tx");
	    MUSB_StartTransfer(&MGC_MsdTxCswIrp);
	}
#else
#endif

	return 0;
}

uint8_t txBuff[128];

/** data reception callback */
static uint32_t MGC_HidRxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp)
{
    MUSB_DPRINTF("MGC_HidRxDataComplete, len %d\r\n", pIrp->dwActualLength);

#if 0
    /* data recvd; so send status */
    if (bcmdwrite == 1)
    {
		usb_event_post(MSG_CMD_USB_WRITE_DATA_TO_SDCARD,MGC_aJunk,USB_NODE_DATA_SIZE);
    }

    if (MGC_Csw.dCswDataResidue != 0)
    {
        MGC_Start_Data_Rx((uint8_t *)MGC_aJunk);
    }    /* TODO: check for data recv error */
#else
	//MUSB_StartTransfer(&MGC_HidRxDataIrp);
#endif

	//print_hex_dump("IN: ", pIrp->pBuffer, pIrp->dwActualLength);

#if 1
	os_memcpy(txBuff, pIrp->pBuffer, pIrp->dwActualLength);

	MGC_HidTxDataIrp.pBuffer = txBuff;
	MGC_HidTxDataIrp.dwLength = pIrp->dwActualLength;
	MUSB_StartTransfer(&MGC_HidTxDataIrp);
#endif

    return 0;
}



static uint8_t MGC_HidDeviceRequest(void *hClient, MUSB_BusHandle hBus,
                                    uint32_t dwSequence, const uint8_t *pSetup,
                                    uint16_t wLength)
{
    // uint32_t dwStatus;
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pSetup;
    uint8_t bOk = FALSE;
    uint8_t data[1];

    MUSB_DPRINTF1("MGC_HidDeviceRequest: bmRequestType  0x%x, bRequest 0x%x\r\n", pRequest->bmRequestType, pRequest->bRequest);

#if 0
    if (MUSB_TYPE_STANDARD == (pRequest->bmRequestType & MUSB_TYPE_MASK))
    {
        switch (pRequest->bRequest)
        {
        case MUSB_REQ_GET_INTERFACE:
            MUSB_DeviceResponse(hBus, dwSequence, &MGC_bMsdInterface, 1, FALSE);
            bOk = TRUE;
            break;
        case MUSB_REQ_SET_INTERFACE:
            MUSB_DeviceResponse(hBus, dwSequence, NULL, 0, FALSE);
            bOk = TRUE;
            break;
         case MUSB_REQ_GET_DESCRIPTOR:
            MUSB_DeviceResponse(hBus, dwSequence, hid_custom_report_desc, sizeof(hid_custom_report_desc), FALSE);
            bOk = TRUE;
            break;
        }
    } else
#endif

    if ((pRequest->bmRequestType & MUSB_TYPE_CLASS)
            && (pRequest->bmRequestType & MUSB_RECIP_INTERFACE))
    {
        switch (pRequest->bRequest)
        {
        case HID_REQUEST_GET_REPORT:
            /* reset */
            data[0] = 0;
            MUSB_DeviceResponse(hBus, dwSequence, data, 1, FALSE);
            bOk = TRUE;
            break;

        case HID_REQUEST_GET_IDLE:
            /* get max lun */
            data[0] = 0;
            MUSB_DeviceResponse(hBus, dwSequence, data, 1, FALSE);
            bOk = TRUE;
            break;

        case HID_REQUEST_GET_PROTOCOL:
            data[0] = 0;
            MUSB_DeviceResponse(hBus, dwSequence, data, 1, FALSE);
			bOk = TRUE;
			break;

        case HID_REQUEST_SET_REPORT:
			bOk = TRUE;
			break;

        case HID_REQUEST_SET_IDLE:
			bOk = TRUE;
			break;

        case HID_REQUEST_SET_PROTOCOL:
			bOk = TRUE;
			break;
        }
    }
    // dwStatus++;
    return bOk;
}

static uint8_t MGC_HidDeviceConfigSelected(void *hClient, MUSB_BusHandle hBus,
        uint8_t bConfigurationValue,
        MUSB_PipePtr *ahPipe)
{
    uint32_t dwStatus;

    MUSB_DPRINTF1("MGC_HidDeviceConfigSelected\r\n");

    MGC_HidTxDataIrp.hPipe = ahPipe[0];  // in ep
    MGC_HidRxDataIrp.hPipe = ahPipe[1];  // out ep
    dwStatus = MUSB_StartTransfer(&MGC_HidRxDataIrp);
    MUSB_DPRINTF("MGC_HidDeviceConfigSelected: dwStatus = 0x%lx\r\n", dwStatus);
    if (MUSB_STATUS_OK == dwStatus)
    {
        return TRUE;
    }
    /* TODO: log error? */
    return FALSE;
}

static void MGC_HidNewUsbState(void* hClient, MUSB_BusHandle hBus,
			       MUSB_State State)
{
	MUSB_DPRINTF1("MGC_HidNewUsbState: state = %x\r\n",State); // MUSB_DPRINTF
}


/*************************** FUNCTIONS ****************************/
MUSB_GhiStatus MUSB_GhiAddDevice(MUSB_GhiDeviceHandle *phDevice,
                                 const MUSB_GhiDeviceInfo *pInfo, MUSB_GhiDevice *pDevice)
{
    return MUSB_GHI_ERROR_UNSUPPORTED_DEVICE;
}

void MUSB_GhiDeviceRemoved(MUSB_GhiDeviceHandle hDevice)
{
}

MUSB_KiiStatus MUSB_KiiAddDevice(MUSB_KiiDeviceHandle *phDevice,
                                 const MUSB_KiiDeviceInfo *pInfo,
                                 MUSB_KiiDevice *pDevice)
{
    MUSB_PRT("Keyboard added\r\n");
    return MUSB_KII_SUCCESS;
}

void MUSB_KiiModifiersChanged(MUSB_KiiDeviceHandle hDevice,
                              uint32_t dwModifierMask)
{
    MUSB_PRT("Modifiers changed\r\n");
}

void MUSB_KiiCharacterKeyPressed(MUSB_KiiDeviceHandle hDevice,
                                 uint16_t wCode)
{
    MUSB_PRT("Char pressed\r\n");
}

void MUSB_KiiCharacterKeyReleased(MUSB_KiiDeviceHandle hDevice,
                                  uint16_t wCode)
{
    MUSB_PRT("Char released\r\n");
}

void MUSB_KiiSpecialKeyPressed(MUSB_KiiDeviceHandle hDevice,
                               MUSB_KiiSpecialKey Key)
{
    MUSB_PRT("Special pressed\r\n");
}

void MUSB_KiiSpecialKeyReleased(MUSB_KiiDeviceHandle hDevice,
                                MUSB_KiiSpecialKey Key)
{
    MUSB_PRT("Special released\r\n");
}

void MUSB_KiiDeviceRemoved(MUSB_KiiDeviceHandle hDevice)
{
    MUSB_PRT("Keyboard removed\r\n");
}

MUSB_SpiStatus MUSB_SpiAddDevice(MUSB_SpiDeviceHandle *phDevice,
                                 const MUSB_SpiDeviceInfo *pInfo, MUSB_SpiDevice *pDevice)
{
    MUSB_PRT("Mouse added\r\n");
    return MUSB_SPI_SUCCESS;
}

void MUSB_SpiButtonPressed(MUSB_SpiDeviceHandle hDevice,
                           uint8_t bButtonIndex)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, "Mouse button ");
    MUSB_Stringize(aNumber, 12, bButtonIndex + 1, 10, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    MUSB_StringConcat(aLine, 80, " pressed");
    MUSB_PrintLine(aLine);
}

void MUSB_SpiButtonReleased(MUSB_SpiDeviceHandle hDevice,
                            uint8_t bButtonIndex)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, "Mouse button ");
    MUSB_Stringize(aNumber, 12, bButtonIndex + 1, 10, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    MUSB_StringConcat(aLine, 80, " released");
    MUSB_PrintLine(aLine);
}

void MUSB_SpiWheelsMoved(MUSB_SpiDeviceHandle hDevice,
                         const int8_t *pMotions)
{
    char aLine[80];
    char aNumber[12];

    if(pMotions[0] > 127)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse X wheel moved -");
        MUSB_Stringize(aNumber, 12, 0x100 - pMotions[0], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
    else if(pMotions[0] != 0)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse X wheel moved +");
        MUSB_Stringize(aNumber, 12, pMotions[0], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
    if(pMotions[1] > 127)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse Y wheel moved -");
        MUSB_Stringize(aNumber, 12, 0x100 - pMotions[1], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
    else if(pMotions[1] != 0)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse Y wheel moved +");
        MUSB_Stringize(aNumber, 12, pMotions[1], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
}

void MUSB_SpiDeviceRemoved(MUSB_SpiDeviceHandle hDevice)
{
    MUSB_PRT("Mouse removed\r\n");
}

/* NCI implementations */
MUSB_NciStatus MUSB_NciAddPort(MUSB_NciClientHandle *phClient,
                               const MUSB_NciPortInfo *pPortInfo,
                               const MUSB_NciConnectionInfo *pConnectionInfo,
                               MUSB_NciPortServices *pPortServices)
{
    return MUSB_NCI_NO_MEMORY;
}

void MUSB_NciPortConnected(MUSB_NciClientHandle hClient,
                           const MUSB_NciConnectionInfo *pConnectionInfo)
{
}

void MUSB_NciPortPacketReceived(MUSB_NciClientHandle hClient,
                                uint8_t *pBuffer, uint16_t wLength, uint8_t bMustCopy)
{
}

void MUSB_NciReturnBuffer(MUSB_NciClientHandle hClient,
                          MUSB_NciRxBuffer *pBuffer)
{
}

void MUSB_NciPortDisconnected(MUSB_NciClientHandle hClient)
{
}

void MUSB_NciPortRemoved(MUSB_NciClientHandle hClient)
{
}
/* OTG client */
static void MGC_HidNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State)
{
    char aAnswer[4];

    switch(State)
    {
    case MUSB_AB_IDLE:
        MUSB_PrintLine("S - Start Session");
        MUSB_GetLine(aAnswer, 4);
        if(('s' == aAnswer[0]) || ('S' == aAnswer[0]))
        {
            MUSB_RequestBus(MGC_hCdiBus);
        }
        break;

    case MUSB_A_SUSPEND:
        MUSB_PrintLine("R - Resume bus");
        MUSB_GetLine(aAnswer, 4);
        if(('r' == aAnswer[0]) || ('R' == aAnswer[0]))
        {
            MUSB_ResumeBus(MGC_hCdiBus);
        }
        break;

    default:
        break;
    }
}

static void MGC_HidOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus)
{
    switch(dwStatus)
    {
    case MUSB_STATUS_UNSUPPORTED_DEVICE:
        MUSB_PRT("Device not supported\r\n");
        break;

    case MUSB_STATUS_UNSUPPORTED_HUB:
        MUSB_PRT("Hubs not supported\r\n");
        break;

    case MUSB_STATUS_OTG_VBUS_INVALID:
        MUSB_PRT("Vbus error\r\n");
        break;

    case MUSB_STATUS_OTG_NO_RESPONSE:
        MUSB_PRT("Device not responding\r\n");
        break;

    case MUSB_STATUS_OTG_SRP_FAILED:
        MUSB_PRT("Device not responding (SRP failed)\r\n");
        break;

    default:
        break;
    }
}

MUSB_HfiStatus MUSB_HfiAddDevice(MUSB_HfiVolumeHandle *phVolume,
                                 const MUSB_HfiDeviceInfo *pInfo,
                                 MUSB_HfiDevice *pDevice)
{
    MGC_pHfiDevice = pDevice;
    MediaIsOk = TRUE;
    return MUSB_HFI_SUCCESS;

}

void MUSB_HfiMediumInserted(MUSB_HfiVolumeHandle 	 hVolume,
                            const MUSB_HfiMediumInfo *pMediumInfo)
{
}

void MUSB_HfiMediumRemoved(MUSB_HfiVolumeHandle hVolume)
{
}

void MUSB_HfiDeviceRemoved(void)
{
    MGC_pHfiDevice = NULL;
    MediaIsOk = FALSE;
}

uint8_t MGC_HidGetMediumstatus(void)
{
    return MediaIsOk;
}

int usb_sw_init(void)
{
    uint8_t *pList;
    uint8_t bDriver = 0;
    uint16_t wCount = 0;
    uint16_t wSize = 0;
    uint16_t wRemain;
    MUSB_DeviceDriver *pDriver;

    wRemain = (uint16_t)sizeof(MGC_aHidPeripheralList);
    pList = MGC_aHidPeripheralList;

    wSize = MUSB_FillHidPeripheralList(bDriver, pList, wRemain);
    if(wSize < wRemain)
    {
        pDriver = MUSB_GetHidClassDriver();
        if(pDriver)
        {
            MUSB_MemCopy(&(MGC_HidHostClient.aDeviceDriverList[bDriver]),
                         pDriver,
                         sizeof(MUSB_DeviceDriver));

            pList += wSize;
            wCount += wSize;
            wRemain -= wSize;

            bDriver++;
        }
    }

    MGC_HidHostClient.wPeripheralListLength = wCount;
    MGC_HidHostClient.bDeviceDriverListLength = bDriver;

    if(!MUSB_InitSystem(5))
    {
        MUSB_PRT("[MGC] InitSystem failed\r\n");
        return -1;
    }

    /* find first CDI port */
    MGC_pCdiPort = MUSB_GetPort(0);

    /* start session */
    MGC_hCdiBus = MUSB_RegisterOtgClient(MGC_pCdiPort,
                                         &MGC_HidFunctionClient,
                                         &MGC_HidHostClient,
                                         &MGC_HidOtgClient);

    MUSB_NoneRunBackground();

    return 0;
}

int usb_sw_uninit(void)
{
    return 0;
}
#endif // CFG_SUPPORT_HID

// eof

