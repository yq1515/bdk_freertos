
WPA_VERSION := wpa_supplicant-2.9

AT_SERVICE_CFG ?= 0
ifeq ($(AT_SERVICE_CFG),1)
ATSVR_CFG ?= 1
else
ATSVR_CFG ?= 0
endif

BEKEN_DIR ?= .

# -------------------------------------------------------------------
# Include folder list
# -------------------------------------------------------------------

INCLUDES += -I$(BEKEN_DIR)/include
INCLUDES += -I$(BEKEN_DIR)/demo
INCLUDES += -I$(BEKEN_DIR)/app
INCLUDES += -I$(BEKEN_DIR)/app/config
INCLUDES += -I$(BEKEN_DIR)/app/standalone-station
INCLUDES += -I$(BEKEN_DIR)/app/standalone-ap
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/common
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/ke/
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/mac/
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/hal
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/mm
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/ps
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/rd
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/rwnx
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/rx
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/scan
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/sta
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/tx
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/vif
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/rx/rxl
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/tx/txl
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/p2p
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/chan
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/td
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/tpc
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/lmac/src/tdls
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/mesh
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/mfp
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/rc
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/apm
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/bam
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/llc
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/me
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/rxu
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/scanu
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/sm
INCLUDES += -I$(BEKEN_DIR)/driver/wifi/umac/src/txu
INCLUDES += -I$(BEKEN_DIR)/app/video_work
INCLUDES += -I$(BEKEN_DIR)/app/net_work
INCLUDES += -I$(BEKEN_DIR)/driver/include
INCLUDES += -I$(BEKEN_DIR)/driver/common/reg
INCLUDES += -I$(BEKEN_DIR)/driver/entry
INCLUDES += -I$(BEKEN_DIR)/driver/dma
INCLUDES += -I$(BEKEN_DIR)/driver/intc
INCLUDES += -I$(BEKEN_DIR)/driver/phy
INCLUDES += -I$(BEKEN_DIR)/driver/pwm
INCLUDES += -I$(BEKEN_DIR)/driver/rc_beken
INCLUDES += -I$(BEKEN_DIR)/driver/flash
INCLUDES += -I$(BEKEN_DIR)/driver/rw_pub
INCLUDES += -I$(BEKEN_DIR)/driver/common/reg
INCLUDES += -I$(BEKEN_DIR)/driver/common
INCLUDES += -I$(BEKEN_DIR)/driver/uart
INCLUDES += -I$(BEKEN_DIR)/driver/sys_ctrl
INCLUDES += -I$(BEKEN_DIR)/driver/gpio
INCLUDES += -I$(BEKEN_DIR)/driver/general_dma
INCLUDES += -I$(BEKEN_DIR)/driver/spidma
INCLUDES += -I$(BEKEN_DIR)/driver/icu
INCLUDES += -I$(BEKEN_DIR)/driver/i2c
INCLUDES += -I$(BEKEN_DIR)/driver/jpeg
INCLUDES += -I$(BEKEN_DIR)/driver/usb
INCLUDES += -I$(BEKEN_DIR)/driver/security
INCLUDES += -I$(BEKEN_DIR)/driver/irda
INCLUDES += -I$(BEKEN_DIR)/components/include
INCLUDES += -I$(BEKEN_DIR)/components/ble_wifi_exchange
INCLUDES += -I$(BEKEN_DIR)/components/rf_test
INCLUDES += -I$(BEKEN_DIR)/components/user_driver
INCLUDES += -I$(BEKEN_DIR)/components/power_save
INCLUDES += -I$(BEKEN_DIR)/components/uart_debug
INCLUDES += -I$(BEKEN_DIR)/components/ethernet_intf
INCLUDES += -I$(BEKEN_DIR)/components/camera_intf
INCLUDES += -I$(BEKEN_DIR)/components/video_transfer
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/hostapd
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/bk_patch
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/src/utils
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/src/ap
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/src/common
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/src/drivers
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/src
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/wpa_supplicant
INCLUDES += -I$(BEKEN_DIR)/components/$(WPA_VERSION)/bk_patch
INCLUDES += -I$(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src
INCLUDES += -I$(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/port
INCLUDES += -I$(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/include
INCLUDES += -I$(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/include/netif
INCLUDES += -I$(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/include/lwip
INCLUDES += -I$(BEKEN_DIR)/components/temp_detect
INCLUDES += -I$(BEKEN_DIR)/components/spidma_intf
INCLUDES += -I$(BEKEN_DIR)/components/saradc_intf
INCLUDES += -I$(BEKEN_DIR)/components/rwnx_intf
INCLUDES += -I$(BEKEN_DIR)/components/joint_up
INCLUDES += -I$(BEKEN_DIR)/components/base64
INCLUDES += -I$(BEKEN_DIR)/components/easy_flash
INCLUDES += -I$(BEKEN_DIR)/components/easy_flash/inc
INCLUDES += -I$(BEKEN_DIR)/components/easy_flash/port
INCLUDES += -I$(BEKEN_DIR)/components/rf_use
INCLUDES += -I$(BEKEN_DIR)/components/usb
INCLUDES += -I$(BEKEN_DIR)/components/sensor
INCLUDES += -I$(BEKEN_DIR)/os/include
INCLUDES += -I$(BEKEN_DIR)/os/FreeRTOSv9.0.0
INCLUDES += -I$(BEKEN_DIR)/components/utf8
INCLUDES += -I$(BEKEN_DIR)/app/http


ifeq ($(CFG_BK_AWARE),1)
INCLUDES += -I$(BEKEN_DIR)/components/bk_aware
endif

ifeq ($(CFG_USE_SDCARD_HOST),1)
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/msc
INCLUDES += -I$(BEKEN_DIR)/components/fatfs
endif

# For BK7251
ifeq ($(CFG_SOC_NAME), 3)
INCLUDES += -I$(BEKEN_DIR)/driver/audio
INCLUDES += -I$(BEKEN_DIR)/driver/sdcard
endif

# For WPA3
ifeq ($(CFG_WPA3),1)
INCLUDES += -I$(BEKEN_DIR)/components/wolfssl
endif

#paho-mqtt
# INCLUDES += -I$(BEKEN_DIR)/components/paho-mqtt/client
# INCLUDES += -I$(BEKEN_DIR)/components/paho-mqtt/client/src
# INCLUDES += -I$(BEKEN_DIR)/components/paho-mqtt/packet/src
# INCLUDES += -I$(BEKEN_DIR)/components/paho-mqtt/mqtt_ui
# INCLUDES += -I$(BEKEN_DIR)/components/paho-mqtt/mqtt_ui/ssl_mqtt
# INCLUDES += -I$(BEKEN_DIR)/components/paho-mqtt/mqtt_ui/tcp_mqtt

ifeq ($(CFG_SUPPORT_BLE),1)
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_4_2))
INCLUDES += -I$(BEKEN_DIR)/driver/ble
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/controller/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/hci/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/host/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/sys/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/config
INCLUDES += -I$(BEKEN_DIR)/driver/ble/modules/app/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble/modules/gernel_api
INCLUDES += -I$(BEKEN_DIR)/driver/ble/modules/mesh_model/ali
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/arch
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/driver/ble_icu
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/driver/ir
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/driver/reg
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/driver/sys_ctrl
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/driver/uart
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/plactform/modules/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/profiles/comm/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble/profiles/prf/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/profiles/sdp/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/include
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/dbg
INCLUDES += -I$(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/include
endif
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_5_x))
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/inc
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gap/gapc
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gap/gapm
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/attc
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/attm
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/atts
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/gattc
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/gatt/gattm
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/l2c/l2cc
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/hl/src/l2c/l2cm
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/ll/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/ll/import/reg
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/llc
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/lld
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/ble/ll/src/llm
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/em/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/hci/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/hci/src
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/sch/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/ip/sch/import
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/aes/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/aes/src
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/common/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/dbg/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/dbg/src
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/ecc_p256/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/h4tl/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/ke/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib/modules/ke/src
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/prf
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/rwip/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/rwip/import/reg
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/nvds/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/config
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/driver/reg
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/driver/rf
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/driver/uart
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/entry
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/arch/armv5
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/arch/armv5/ll
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/arch/armv5/compiler
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/profiles/comm/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/profiles/sdp/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/app/api
INCLUDES += -I$(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/ui
endif
endif

#usb module
#ifeq ($(CFG_MENTOR_USB),1)
INCLUDES += -I$(BEKEN_DIR)/driver/usb/include
INCLUDES += -I$(BEKEN_DIR)/driver/usb/include/class
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/cd
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/comm
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/hid
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/msd
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/compl
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/hub
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/drivers/trans
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/example/msd
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/hid
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/lib
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/msc
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/systems/none/afs
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/systems/none
INCLUDES += -I$(BEKEN_DIR)/driver/usb/src/uvc
#endif

ifeq ("${CFG_MBEDTLS}", "1")
#CFG_DEFINE_INCLUDE += MBEDTLS_CONFIG_FILE=\"tls_config.h\"
INCLUDES += -I$(BEKEN_DIR)/components/mbedtls/mbedtls/include
INCLUDES += -I$(BEKEN_DIR)/components/mbedtls/mbedtls/include/mbedtls
INCLUDES += -I$(BEKEN_DIR)/components/mbedtls/mbedtls-port/inc
INCLUDES += -I$(BEKEN_DIR)/components/mbedtls/mbedtls_ui/
endif


ifeq ($(ATSVR_CFG),1)
INCLUDES += -I$(BEKEN_DIR)/components/at_server/
INCLUDES += -I$(BEKEN_DIR)/components/at_server/_at_server
INCLUDES += -I$(BEKEN_DIR)/components/at_server/_at_server_port
INCLUDES += -I$(BEKEN_DIR)/components/at_server/at_server_func
INCLUDES += -I$(BEKEN_DIR)/components/at_server/atsvr_cmd
endif

# CherryUSB
ifeq ($(CFG_CHERRY_USB),1)
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/common
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/core
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/osal
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/cdc
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/hid
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/msc
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/audio
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/video
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/hub
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/demo
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/demo/beken/usb_device
ifeq ($(CFG_UF2),1)
INCLUDES += -I$(BEKEN_DIR)/components/uf2
INCLUDES += -I$(BEKEN_DIR)/components/uf2/ports
endif
endif

# TinyUSB
ifeq ($(CFG_TINYUSB),1)

INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/hw
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/audio
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/cdc
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/cdc
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/dfu
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/hid
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/hid
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/midi
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/msc
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/msc
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/net
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/usbtmc
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/vendor
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/vendor
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/class/video
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/common
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/device
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/host
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/typec
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/src/typec


# examples
INCLUDES += -I$(BEKEN_DIR)/components/tinyusb/examples/device/hid_generic_inout/src
endif

# -------------------------------------------------------------------
# Source file list
# -------------------------------------------------------------------

#application layer
SRC_C += $(BEKEN_DIR)/app/app.c
SRC_C += $(BEKEN_DIR)/app/ate_app.c
SRC_C += $(BEKEN_DIR)/app/config/param_config.c
SRC_C += $(BEKEN_DIR)/app/standalone-ap/sa_ap.c
SRC_C += $(BEKEN_DIR)/app/standalone-station/sa_station.c
SRC_C += $(BEKEN_DIR)/app/video_work/video_transfer_tcp.c
SRC_C += $(BEKEN_DIR)/app/video_work/video_transfer_udp.c
SRC_C += $(BEKEN_DIR)/app/video_work/video_buffer.c
SRC_C += $(BEKEN_DIR)/app/net_work/video_demo_main.c
SRC_C += $(BEKEN_DIR)/app/net_work/video_demo_station.c
SRC_C += $(BEKEN_DIR)/app/net_work/video_demo_softap.c

#demo module
SRC_C += $(BEKEN_DIR)/demos/ieee802_11_demo.c

#driver layer
SRC_C += $(BEKEN_DIR)/driver/common/dd.c
SRC_C += $(BEKEN_DIR)/driver/common/drv_model.c
SRC_C += $(BEKEN_DIR)/driver/dma/dma.c
SRC_C += $(BEKEN_DIR)/driver/driver.c
SRC_C += $(BEKEN_DIR)/driver/entry/arch_main.c
SRC_C += $(BEKEN_DIR)/driver/fft/fft.c
SRC_C += $(BEKEN_DIR)/driver/flash/flash.c
SRC_C += $(BEKEN_DIR)/driver/general_dma/general_dma.c
SRC_C += $(BEKEN_DIR)/driver/gpio/gpio.c
SRC_C += $(BEKEN_DIR)/driver/i2s/i2s.c
SRC_C += $(BEKEN_DIR)/driver/icu/icu.c
SRC_C += $(BEKEN_DIR)/driver/intc/intc.c
SRC_C += $(BEKEN_DIR)/driver/irda/irda.c
SRC_C += $(BEKEN_DIR)/driver/macphy_bypass/mac_phy_bypass.c
SRC_C += $(BEKEN_DIR)/driver/phy/phy_trident.c
SRC_C += $(BEKEN_DIR)/driver/pwm/pwm.c
SRC_C += $(BEKEN_DIR)/driver/pwm/pwm_bk7231n.c
SRC_C += $(BEKEN_DIR)/driver/pwm/mcu_ps_timer.c
SRC_C += $(BEKEN_DIR)/driver/pwm/bk_timer.c
SRC_C += $(BEKEN_DIR)/driver/pwm/pwm_mutex.c
SRC_C += $(BEKEN_DIR)/driver/qspi/qspi.c
SRC_C += $(BEKEN_DIR)/driver/rw_pub/rw_platf_pub.c
SRC_C += $(BEKEN_DIR)/driver/saradc/saradc.c
SRC_C += $(BEKEN_DIR)/driver/spidma/spidma.c
SRC_C += $(BEKEN_DIR)/driver/sys_ctrl/sys_ctrl.c
SRC_C += $(BEKEN_DIR)/driver/uart/Retarget.c
SRC_C += $(BEKEN_DIR)/driver/uart/uart.c
SRC_C += $(BEKEN_DIR)/driver/uart/printf.c
SRC_C += $(BEKEN_DIR)/driver/wdt/wdt.c
SRC_C += $(BEKEN_DIR)/driver/calendar/calendar.c
SRC_C += $(BEKEN_DIR)/driver/security/security.c
SRC_C += $(BEKEN_DIR)/driver/security/hal_aes.c
SRC_C += $(BEKEN_DIR)/driver/security/hal_sha.c
SRC_C += $(BEKEN_DIR)/driver/jpeg/jpeg_encoder.c
SRC_C += $(BEKEN_DIR)/driver/i2c/i2c1.c
SRC_C += $(BEKEN_DIR)/driver/i2c/i2c2.c

ifeq ($(CFG_SDIO),1)
SRC_C += $(BEKEN_DIR)/driver/sdio/sdio.c
SRC_C += $(BEKEN_DIR)/driver/sdio/sdma.c
SRC_C += $(BEKEN_DIR)/driver/sdio/sutil.c
endif

#componentstion layer
SRC_C += $(BEKEN_DIR)/components/func.c
SRC_C += $(BEKEN_DIR)/components/usb_plug/usb_plug.c
SRC_C += $(BEKEN_DIR)/components/security/security_func.c
SRC_C += $(BEKEN_DIR)/components/joint_up/role_launch.c
SRC_C += $(BEKEN_DIR)/app/http/utils_httpc.c
SRC_C += $(BEKEN_DIR)/app/http/utils_net.c
SRC_C += $(BEKEN_DIR)/app/http/utils_timer.c
SRC_C += $(BEKEN_DIR)/app/http/lite-log.c

ifeq ($(CFG_BK_AWARE),1)
SRC_C += $(BEKEN_DIR)/components/bk_aware/bk_aware.c
endif

SRC_WPA_C += $(BEKEN_DIR)/components/hostapd_intf/hostapd_intf.c
ifeq ($(CFG_USE_SDCARD_HOST),1)
SRC_C += $(BEKEN_DIR)/components/fatfs/cc936.c
SRC_C += $(BEKEN_DIR)/components/fatfs/ccsbcs.c
SRC_C += $(BEKEN_DIR)/components/fatfs/disk_io.c
SRC_C += $(BEKEN_DIR)/components/fatfs/driver_udisk.c
SRC_C += $(BEKEN_DIR)/components/fatfs/ff.c
SRC_C += $(BEKEN_DIR)/components/fatfs/playmode.c
endif

ifeq ($(CFG_SDIO),1)
SRC_C += $(BEKEN_DIR)/components/sdio_intf/sdio_intf.c
endif

SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/port/ethernetif.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/port/net.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/port/sys_arch.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/api_lib.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/api_msg.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/err.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/netbuf.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/netdb.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/netifapi.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/sockets.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/api/tcpip.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/apps/ping/ping.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/def.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/dns.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/inet_chksum.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/init.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ip.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/dhcp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/etharp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/icmp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/igmp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4_addr.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv4/ip4_frag.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/dhcp6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/ethip6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/icmp6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/inet6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6_addr.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/ip6_frag.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/mld6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/ipv6/nd6.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/mem.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/memp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/netif.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/pbuf.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/raw.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/stats.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/sys.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/tcp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/tcp_in.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/tcp_out.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/timeouts.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/core/udp.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/lwip-2.0.2/src/netif/ethernet.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/dhcpd/dhcp-server.c
SRC_C += $(BEKEN_DIR)/components/lwip_intf/dhcpd/dhcp-server-main.c
SRC_C += $(BEKEN_DIR)/components/misc/fake_clock.c
SRC_C += $(BEKEN_DIR)/components/misc/pseudo_random.c
SRC_C += $(BEKEN_DIR)/components/misc/target_util.c
SRC_C += $(BEKEN_DIR)/components/misc/start_type.c
SRC_C += $(BEKEN_DIR)/components/power_save/power_save.c
SRC_C += $(BEKEN_DIR)/components/power_save/manual_ps.c
SRC_C += $(BEKEN_DIR)/components/power_save/mcu_ps.c
SRC_C += $(BEKEN_DIR)/components/power_save/ap_idle.c
SRC_C += $(BEKEN_DIR)/components/saradc_intf/saradc_intf.c
SRC_C += $(BEKEN_DIR)/components/rwnx_intf/rw_ieee80211.c
SRC_C += $(BEKEN_DIR)/components/rwnx_intf/rw_msdu.c
SRC_C += $(BEKEN_DIR)/components/rwnx_intf/rw_msg_rx.c
SRC_C += $(BEKEN_DIR)/components/rwnx_intf/rw_msg_tx.c
SRC_C += $(BEKEN_DIR)/components/sim_uart/gpio_uart.c
SRC_C += $(BEKEN_DIR)/components/sim_uart/pwm_uart.c
SRC_C += $(BEKEN_DIR)/components/spidma_intf/spidma_intf.c
SRC_C += $(BEKEN_DIR)/components/temp_detect/temp_detect.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverFlash.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverGpio.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverPwm.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverUart.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverWdg.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverRng.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverTimer.c
SRC_C += $(BEKEN_DIR)/components/wlan_ui/wlan_cli.c
SRC_C += $(BEKEN_DIR)/components/wlan_ui/bk_peripheral_test.c
# utf8
SRC_C += $(BEKEN_DIR)/components/utf8/conv_utf8.c

# video / jpeg
SRC_C += $(BEKEN_DIR)/components/camera_intf/camera_intf.c
SRC_C += $(BEKEN_DIR)/components/video_transfer/video_transfer.c

# For WPA3: wolfssl
ifeq ($(CFG_WPA3),1)
SRC_C += $(BEKEN_DIR)/components/wolfssl/wolfcrypt/src/wolfmath.c
SRC_C += $(BEKEN_DIR)/components/wolfssl/wolfcrypt/src/memory.c
SRC_C += $(BEKEN_DIR)/components/wolfssl/wolfcrypt/src/tfm.c
SRC_C += $(BEKEN_DIR)/components/wolfssl/wolfcrypt/src/ecc.c

# wpa_supplicant 2.9 needs random generator
SRC_C += $(BEKEN_DIR)/components/wolfssl/wolfcrypt/src/random.c
SRC_C += $(BEKEN_DIR)/components/wolfssl/wolfcrypt/src/sha256.c
endif

# For BK7251
ifeq ($(CFG_SOC_NAME), 3)
SRC_C += $(BEKEN_DIR)/driver/audio/audio.c
SRC_C += $(BEKEN_DIR)/driver/audio/audio_adc.c
SRC_C += $(BEKEN_DIR)/driver/audio/audio_dac.c
SRC_C += $(BEKEN_DIR)/driver/audio/ring_buffer.c
SRC_C += $(BEKEN_DIR)/driver/audio/ring_buffer_dma_read.c
SRC_C += $(BEKEN_DIR)/driver/audio/ring_buffer_dma_write.c

SRC_C += $(BEKEN_DIR)/components/audio/audio_intf.c

SRC_C += $(BEKEN_DIR)/driver/sdcard/sdcard.c
SRC_C += $(BEKEN_DIR)/driver/sdcard/sdio_driver.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi_master.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi_slave.c
SRC_C += $(BEKEN_DIR)/components/sd_music/sdcard_test.c
endif

# For BK7231u
ifeq ($(CFG_SOC_NAME), 2)
SRC_C += $(BEKEN_DIR)/driver/spi/spi.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi_master.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi_slave.c
endif

# For BK7231N
ifeq ($(CFG_SOC_NAME), 5)
SRC_C += $(BEKEN_DIR)/driver/spi/spi_bk7231n.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi_master_bk7231n.c
SRC_C += $(BEKEN_DIR)/driver/spi/spi_slave_bk7231n.c
endif

SRC_C += $(BEKEN_DIR)/components/wlan_ui/wlan_ui.c
SRC_C += $(BEKEN_DIR)/components/net_param_intf/net_param.c
SRC_C += $(BEKEN_DIR)/components/base64/base_64.c
SRC_C += $(BEKEN_DIR)/components/airkiss/bk_airkiss.c
SRC_C += $(BEKEN_DIR)/components/airkiss/airkiss_main.c
SRC_C += $(BEKEN_DIR)/components/airkiss/airkiss_pingpong.c

#easy flash
SRC_C += $(BEKEN_DIR)/components/easy_flash/bk_ef.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/src/easyflash.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/src/ef_env.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/src/ef_env_wl.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/src/ef_iap.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/src/ef_log.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/src/ef_utils.c
SRC_C += $(BEKEN_DIR)/components/easy_flash/port/ef_port.c

#paho-mqtt
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/client/src/MQTTClient.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/client/src/MQTTFreeRTOS.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/client/paho_mqtt_udp.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTConnectClient.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTConnectServer.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTDeserializePublish.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTFormat.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTPacket.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTSerializePublish.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTSubscribeClient.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTSubscribeServer.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTUnsubscribeClient.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/packet/src/MQTTUnsubscribeServer.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/mqtt_ui/ssl_mqtt/ssl_mqtt_client_port.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/mqtt_ui/tcp_mqtt/tcp_mqtt_client_port.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/mqtt_ui/mqtt_client_core.c
# SRC_C += $(BEKEN_DIR)/components/paho-mqtt/mqtt_ui/mqtt_client_com_port.c


ifeq ("${CFG_MBEDTLS}", "1")
#MBEDTLS_SRC_DIRS += $(shell find components/mbedtls/mbedtls/library -type d)
#MBEDTLS_SRC_DIRS += $(shell find components/mbedtls/mbedtls-port/src -type d)
##MBEDTLS_SRC_DIRS += ./components/mbedtls/mbedtls/library/
##MBEDTLS_SRC_DIRS += ./components/mbedtls/mbedtls-port/src/
#SRC_C += $(foreach dir, $(MBEDTLS_SRC_DIRS), $(wildcard $(dir)/*.c))

SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls-port/src/timing_alt.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls-port/src/tls_certificate.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls-port/src/tls_client.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls-port/src/tls_hardware.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls-port/src/tls_mem.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls-port/src/tls_net.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/aes.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/aesni.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/arc4.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/asn1parse.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/asn1write.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/base64.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/bignum.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/blowfish.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/camellia.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ccm.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/certs.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/cipher.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/cipher_wrap.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/cmac.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ctr_drbg.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/debug.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/des.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/dhm.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ecdh.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ecdsa.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ecjpake.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ecp.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ecp_curves.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/entropy.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/entropy_poll.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/error.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/gcm.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/havege.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/hmac_drbg.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/md.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/md_wrap.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/md2.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/md4.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/md5.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/memory_buffer_alloc.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/net_sockets.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/oid.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/padlock.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pem.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pk.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pk_wrap.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pkcs5.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pkcs11.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pkcs12.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pkparse.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/pkwrite.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/platform.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ripemd160.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/rsa.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/sha1.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/sha256.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/sha512.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_cache.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_ciphersuites.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_cli.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_cookie.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_srv.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_ticket.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/ssl_tls.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/threading.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/timing.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/version.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/version_features.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509_create.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509_crl.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509_crt.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509_csr.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509write_crt.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/x509write_csr.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls/library/xtea.c
SRC_C += $(BEKEN_DIR)/components/mbedtls/mbedtls_ui/sl_tls.c
endif

ifeq ($(CFG_SUPPORT_BLE),1)
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_4_2))
#ble pub
SRC_C += $(BEKEN_DIR)/driver/ble/ble.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_ble.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_comm.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_sdp.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_sec.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_task.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/driver/ble_icu/ble_icu.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/driver/uart/ble_uart.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/arch/ble_arch_main.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/common/RomCallFlash.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/dbg/dbg.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/dbg/dbg_mwsgen.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/dbg/dbg_swdiag.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/dbg/dbg_task.c
SRC_C += $(BEKEN_DIR)/driver/ble/plactform/modules/rf/src/ble_rf_xvr.c
SRC_C += $(BEKEN_DIR)/driver/ble/profiles/comm/src/comm.c
SRC_C += $(BEKEN_DIR)/driver/ble/profiles/comm/src/comm_task.c
SRC_C += $(BEKEN_DIR)/driver/ble/profiles/prf/src/prf.c
SRC_C += $(BEKEN_DIR)/driver/ble/profiles/prf/src/prf_utils.c
SRC_C += $(BEKEN_DIR)/driver/ble/profiles/sdp/src/sdp_service.c
SRC_C += $(BEKEN_DIR)/driver/ble/profiles/sdp/src/sdp_service_task.c
#ble mesh pub
ifeq ($(CFG_SUPPORT_BLE_MESH),1)
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_api.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_api_msg.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_param_int.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mm_api.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mm_api_msg.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/m_api.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/mesh_api/m_api_msg.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_route.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_bind.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_replay.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_state.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_bat.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_dtt.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_loc.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_lvl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_oo.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_plvl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_poo.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_prop.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ctl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_hsl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ln.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_xyl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_ctl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_hsl.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_ln.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/Scenes/m_fnd_Scenes.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/transition_time/m_fnd_generic_transition_time.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/vendor/mm_vendors.c
SRC_C += $(BEKEN_DIR)/driver/ble/beken_ble_sdk/mesh/src/models/vendor/mm_vendor_midea.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_mesh.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/app/src/app_mm_msg.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/gernel_api/mesh_general_api.c
SRC_C += $(BEKEN_DIR)/driver/ble/modules/mesh_model/ali/app_light_ali_server.c
endif
endif #BLE_VERSION_4_2
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_5_x))
# ble pub
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/prf/prf.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/prf/prf_utils.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/profiles/comm/src/comm.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/profiles/comm/src/comm_task.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/app/src/app_comm.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/app/src/app_ble.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/app/src/app_task.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/ui/ble_ui.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/rwip/src/rwip.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/rwip/src/rwble.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/entry/ble_main.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/driver/rf/rf_xvr.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/driver/rf/ble_rf_port.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/platform/7231n/driver/uart/uart_ble.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/app/src/app_ble_init.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/app/src/app_sdp.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/profiles/sdp/src/sdp_common.c
SRC_C += $(BEKEN_DIR)/driver/ble_5_x_rw/ble_pub/profiles/sdp/src/sdp_comm_task.c
endif #BLE_VERSION_5_x
endif #CFG_SUPPORT_BLE

#operation system module
SRC_OS += $(BEKEN_DIR)/os/FreeRTOSv9.0.0/rtos_pub.c
SRC_C  += $(BEKEN_DIR)/os/mem_arch.c
SRC_C  += $(BEKEN_DIR)/os/platform_stub.c
SRC_C  += $(BEKEN_DIR)/os/str_arch.c

ifeq ($(ATSVR_CFG),1)
SRC_C += $(BEKEN_DIR)/components/at_server/_at_server_port/atsvr_core.c
SRC_C += $(BEKEN_DIR)/components/at_server/_at_server_port/atsvr_port.c
SRC_C += $(BEKEN_DIR)/components/at_server/atsvr_cmd/atsvr_cmd.c
SRC_C += $(BEKEN_DIR)/components/at_server/atsvr_cmd/atsvr_wlan.c
SRC_C += $(BEKEN_DIR)/components/at_server/atsvr_cmd/atsvr_misc.c
SRC_C += $(BEKEN_DIR)/components/at_server/_at_server/_at_server.c
SRC_C += $(BEKEN_DIR)/components/at_server/_at_server/_at_server_port.c
SRC_C += $(BEKEN_DIR)/components/at_server/at_server_func/_atsvr_func.c
SRC_C += $(BEKEN_DIR)/components/at_server/at_server.c
endif

ifeq ($(CFG_MENTOR_USB),1)
SRC_C += $(BEKEN_DIR)/driver/usb/usb.c
SRC_C += $(BEKEN_DIR)/components/usb/fusb.c
endif

SRC_C += $(BEKEN_DIR)/components/ble_wifi_exchange/ble_wifi_port.c

# CherryUSB
#
ifeq ($(CFG_CHERRY_USB),1)
SRC_C += $(BEKEN_DIR)/components/CherryUSB/core/usbd_core.c
#SRC_C += $(BEKEN_DIR)/components/CherryUSB/demo/beken/usb_device/cdc/cdc_acm.c
#SRC_C += $(BEKEN_DIR)/components/CherryUSB/class/cdc/usbd_cdc.c
#SRC_C += $(BEKEN_DIR)/components/CherryUSB/demo/beken/usb_device/hid/hid_common.c
#SRC_C += $(BEKEN_DIR)/components/CherryUSB/class/hid/usbd_hid.c
SRC_C += $(BEKEN_DIR)/components/CherryUSB/class/msc/usbd_msc.c
SRC_C += $(BEKEN_DIR)/components/CherryUSB/port/beken_musb/usb_dc_beken_musb.c

# UF2
ifeq ($(CFG_UF2),1)
SRC_C += $(BEKEN_DIR)/components/uf2/ghostfat.c
SRC_C += $(BEKEN_DIR)/components/uf2/msc_desc.c
SRC_C += $(BEKEN_DIR)/components/uf2/ports/board_flash.c
endif # CFG_UF2
endif

ifeq ($(CFG_TINYUSB),1)
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/tusb.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/common/tusb_fifo.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/device/usbd.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/device/usbd_control.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/typec/usbc.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/audio/audio_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/cdc/cdc_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/dfu/dfu_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/dfu/dfu_rt_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/hid/hid_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/midi/midi_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/msc/msc_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/net/ecm_rndis_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/net/ncm_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/usbtmc/usbtmc_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/video/video_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/vendor/vendor_device.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/host/usbh.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/host/hub.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/cdc/cdc_host.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/hid/hid_host.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/msc/msc_host.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/class/vendor/vendor_host.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/src/typec/usbc.c

# tinyusb examples
SRC_C += $(BEKEN_DIR)/components/tinyusb/examples/device/hid_generic_inout/src/main.c
SRC_C += $(BEKEN_DIR)/components/tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.c
endif

#assembling files
SRC_S +=  $(BEKEN_DIR)/driver/entry/boot_handlers.S
SRC_S +=  $(BEKEN_DIR)/driver/entry/boot_vectors.S



# -------------------------------------------------------------------
# Lib source file list
# -------------------------------------------------------------------

# Lib files will be deleted when making SDK.
SRC_IP_C =
-include $(BEKEN_DIR)/ip/ip_lib_src.mk

SRC_BLE_C =
ifeq ($(CFG_SUPPORT_BLE),1)
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_4_2))
-include $(BEKEN_DIR)/driver/ble/ble_lib_src.mk
endif
ifeq ($(CFG_BLE_VERSION),$(BLE_VERSION_5_x))
-include $(BEKEN_DIR)/driver/ble_5_x_rw/ble_lib_src.mk
endif
endif

SRC_USB_C =
ifeq ($(CFG_MENTOR_USB),1)
-include $(BEKEN_DIR)/driver/usb/usb_lib_src.mk
endif

SRC_SENSOR_C =
ifeq ($(CFG_WIFI_SENSOR),1)
-include $(BEKEN_DIR)/components/sensor/sensor_lib_src.mk
endif

SRC_CAL_C =
-include $(BEKEN_DIR)/components/bk7011_cal/cal_lib_src.mk

SRC_SUPPLICANT_C =
-include $(BEKEN_DIR)/components/wpa_supplicant_2_9/supplicant_lib_src.mk

SRC_UART_DEBUG_C =
-include $(BEKEN_DIR)/components/uart_debug/uart_debug_lib_src.mk

SRC_RF_TEST_C =
-include $(BEKEN_DIR)/components/rf_test/rf_test_lib_src.mk

SRC_RF_USE_C =
-include $(BEKEN_DIR)/components/rf_use/rf_use_lib_src.mk


