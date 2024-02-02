/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Based on the file "include/zephyr/drivers/gnss/ublox_neo_m8.h" from pull request #46447
 * (https://github.com/zephyrproject-rtos/zephyr/pull/46447).
 */

#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include "gnss_u_blox_protocol_defines.h"

#ifndef ZEPHYR_U_BLOX_PROTOCOL_
#define ZEPHYR_U_BLOX_PROTOCOL_

#define U_BLOX_BAUDRATE_COUNT	8

#define U_BLOX_MESSAGE_HEADER_SIZE		6
#define U_BLOX_MESSAGE_FOOTER_SIZE		2
#define U_BLOX_MESSAGE_SIZE_WITHOUT_PAYLOAD	U_BLOX_MESSAGE_HEADER_SIZE + U_BLOX_MESSAGE_FOOTER_SIZE

#define U_BLOX_MESSAGE_LEN_MAX	264
#define U_BLOX_PAYLOAD_LEN_MAX	256

#define U_BLOX_CFG_PRT_WAIT_MS	4000
#define U_BLOX_CFG_RST_WAIT_MS	8000

extern const uint32_t u_blox_baudrate[U_BLOX_BAUDRATE_COUNT];

#define TO_LITTLE_ENDIAN(data, b)			\
	for (int j = 0; j < sizeof(data); j++) {	\
		b[j] = (data >> (j * 8)) & 0xFF;	\
	}

#define UBX_CFG_PRT_IN_PROTO_UBX			BIT(0)
#define UBX_CFG_PRT_IN_PROTO_NMEA			BIT(1)
#define UBX_CFG_PRT_IN_PROTO_RTCM			BIT(2)
#define UBX_CFG_PRT_IN_PROTO_RTCM3			BIT(5)
#define UBX_CFG_PRT_OUT_PROTO_UBX			BIT(0)
#define UBX_CFG_PRT_OUT_PROTO_NMEA			BIT(1)
#define UBX_CFG_PRT_OUT_PROTO_RTCM3			BIT(5)

#define UBX_CFG_PRT_PORT_MODE_CHAR_LEN_5		0U
#define UBX_CFG_PRT_PORT_MODE_CHAR_LEN_6		BIT(6)
#define UBX_CFG_PRT_PORT_MODE_CHAR_LEN_7		BIT(7)
#define UBX_CFG_PRT_PORT_MODE_CHAR_LEN_8		BIT(6) | BIT(7)

#define UBX_CFG_PRT_PORT_MODE_PARITY_EVEN		0U
#define UBX_CFG_PRT_PORT_MODE_PARITY_ODD		BIT(9)
#define UBX_CFG_PRT_PORT_MODE_PARITY_NONE		BIT(11)

#define UBX_CFG_PRT_PORT_MODE_STOP_BITS_1		0U
#define UBX_CFG_PRT_PORT_MODE_STOP_BITS_1_HALF		BIT(12)
#define UBX_CFG_PRT_PORT_MODE_STOP_BITS_2		BIT(13)
#define UBX_CFG_PRT_PORT_MODE_STOP_BITS_HALF		BIT(12) | BIT(13)

#define UBX_CFG_PRT_GET_PAYLOAD_SIZE	1
#define UBX_CFG_PRT_SET_PAYLOAD_SIZE	20
#define UBX_CFG_RST_SET_PAYLOAD_SIZE	4
#define UBX_CFG_NAV5_GET_PAYLOAD_SIZE	0
#define UBX_CFG_NAV5_SET_PAYLOAD_SIZE	36
#define UBX_CFG_GNSS_GET_PAYLOAD_SIZE	0
#define UBX_CFG_MSG_SET_PAYLOAD_SIZE	3

struct u_blox_cfg_prt_get_data {
	uint8_t port_id;
};
void u_blox_cfg_prt_get_data_default(struct u_blox_cfg_prt_get_data *data);
int u_blox_cfg_prt_get(uint8_t *ubx_frame, uint16_t ubx_frame_size, struct u_blox_cfg_prt_get_data *data);

struct u_blox_cfg_prt_set_data {
	uint8_t port_id;
	uint16_t tx_ready_pin_conf;
	uint32_t port_mode;
	uint32_t baudrate;
	uint16_t in_proto_mask;
	uint16_t out_proto_mask;
	uint16_t flags;
};
void u_blox_cfg_prt_set_data_default(struct u_blox_cfg_prt_set_data *data);
int u_blox_cfg_prt_set(uint8_t *ubx_frame, uint16_t ubx_frame_size, struct u_blox_cfg_prt_set_data *data);

#define UBX_CFG_RST_NAV_BBR_MASK_HOT_START		0x0000
#define UBX_CFG_RST_NAV_BBR_MASK_WARM_START		0x0001
#define UBX_CFG_RST_NAV_BBR_MASK_COLD_START		0xFFFF
#define UBX_CFG_RST_RESET_MODE_HARD_RESET				0x00
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_SOFT_RESET			0x01
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_SOFT_RESET_GNSS_ONLY		0x02
#define UBX_CFG_RST_RESET_MODE_HARD_RESET_AFTER_SHUTDOWN		0x04
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_GNSS_STOP			0x08
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_GNSS_START			0x09

struct u_blox_cfg_rst_set_data {
	uint16_t nav_bbr_mask;
	uint8_t reset_mode;
};
void u_blox_cfg_rst_set_data_default(struct u_blox_cfg_rst_set_data *data);
int u_blox_cfg_rst_set(uint8_t *ubx_frame, uint16_t ubx_frame_size, struct u_blox_cfg_rst_set_data *data);

int u_blox_cfg_nav5_get(uint8_t *ubx_frame, uint16_t ubx_frame_size);

struct u_blox_cfg_nav5_set_data {
	uint16_t mask;
	uint8_t dyn_model;

	uint8_t fix_mode;

	int32_t fixed_alt;
	uint32_t fixed_alt_var;

	int8_t min_elev;
	uint8_t dr_limit;

	uint16_t p_dop;
	uint16_t t_dop;
	uint16_t p_acc;
	uint16_t t_acc;

	uint8_t static_hold_threshold;
	uint8_t dgnss_timeout;
	uint8_t cno_threshold_num_svs;
	uint8_t cno_threshold;

	uint16_t static_hold_dist_threshold;
	uint8_t utc_standard;
};
void u_blox_cfg_nav5_set_data_default(struct u_blox_cfg_nav5_set_data *data);
int u_blox_cfg_nav5_set(uint8_t *ubx_frame, uint16_t ubx_frame_size, struct u_blox_cfg_nav5_set_data *data);

int u_blox_cfg_gnss_get(uint8_t *ubx_frame, uint16_t ubx_frame_size);
int u_blox_cfg_gnss_set(uint8_t *ubx_frame, uint16_t ubx_frame_size, uint8_t msg_ver,
			     uint8_t num_trk_ch_use, uint8_t *config, uint16_t config_size);

struct u_blox_cfg_msg_set_data {
	uint8_t message_class;
	uint8_t message_id;
	uint8_t rate;
};
void u_blox_cfg_msg_set_data_default(struct u_blox_cfg_msg_set_data *data);
int u_blox_cfg_msg_set(uint8_t *ubx_frame, uint16_t ubx_frame_size, struct u_blox_cfg_msg_set_data *data);

#endif /* ZEPHYR_U_BLOX_PROTOCOL_ */
