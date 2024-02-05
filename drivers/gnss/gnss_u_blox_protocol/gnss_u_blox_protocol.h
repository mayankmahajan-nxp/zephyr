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

#define U_BLOX_BAUDRATE_COUNT			8

#define U_BLOX_FRM_HEADER_SZ			6
#define U_BLOX_FRM_FOOTER_SZ			2
#define U_BLOX_FRM_SZ_WITHOUT_PAYLOAD		U_BLOX_FRM_HEADER_SZ + U_BLOX_FRM_FOOTER_SZ
#define U_BLOX_FRM_SZ(payload_size)		payload_size + U_BLOX_FRM_SZ_WITHOUT_PAYLOAD

#define U_BLOX_PREAMBLE_SYNC_CHAR_1		0xB5
#define U_BLOX_PREAMBLE_SYNC_CHAR_2		0x62

#define U_BLOX_PREAMBLE_SYNC_CHAR_1_IDX		0
#define U_BLOX_PREAMBLE_SYNC_CHAR_2_IDX		1
#define U_BLOX_FRM_MSG_CLASS_IDX		2
#define U_BLOX_FRM_MSG_ID_IDX			3
#define U_BLOX_FRM_PAYLOAD_SZ_L_IDX		4
#define U_BLOX_FRM_PAYLOAD_SZ_H_IDX		5
#define U_BLOX_FRM_PAYLOAD_IDX			6

#define U_BLOX_CHECKSUM_START_IDX		2
#define U_BLOX_CHECKSUM_STOP_IDX_FROM_END	2
#define U_BLOX_CHECKSUM_A_IDX_FROM_END		2
#define U_BLOX_CHECKSUM_B_IDX_FROM_END		1

#define U_BLOX_FRM_SZ_MAX			264
#define U_BLOX_PAYLOAD_SZ_MAX			256
#define U_BLOX_FRM_HEADER_SZ			6
#define U_BLOX_FRM_FOOTER_SZ			2
#define U_BLOX_FRM_SZ_WO_PAYLOAD		U_BLOX_FRM_HEADER_SZ + U_BLOX_FRM_FOOTER_SZ

#define U_BLOX_CFG_RST_WAIT_MS			8000

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

#define UBX_CFG_PRT_POLL_PAYLOAD_SZ	1
#define UBX_CFG_PRT_POLL_FRM_SZ		U_BLOX_FRM_SZ_WO_PAYLOAD + UBX_CFG_PRT_POLL_PAYLOAD_SZ
#define UBX_CFG_PRT_SET_PAYLOAD_SZ	20
#define UBX_CFG_PRT_SET_FRM_SZ		U_BLOX_FRM_SZ_WO_PAYLOAD + UBX_CFG_PRT_SET_PAYLOAD_SZ
#define UBX_CFG_RST_PAYLOAD_SZ		4
#define UBX_CFG_RST_FRM_SZ		U_BLOX_FRM_SZ_WO_PAYLOAD + UBX_CFG_RST_PAYLOAD_SZ
#define UBX_CFG_NAV5_PAYLOAD_SZ		36
#define UBX_CFG_NAV5_FRM_SZ		U_BLOX_FRM_SZ_WO_PAYLOAD + UBX_CFG_NAV5_PAYLOAD_SZ
#define UBX_CFG_MSG_PAYLOAD_SZ		3
#define UBX_CFG_MSG_FRM_SZ		U_BLOX_FRM_SZ_WO_PAYLOAD + UBX_CFG_MSG_PAYLOAD_SZ
#define UBX_CFG_GNSS_PAYLOAD_INIT_SZ	4
#define UBX_CFG_GNSS_PAYLOAD_CFG_BLK_SZ	8
#define UBX_CFG_GNSS_PAYLOAD_SZ(n)	\
	UBX_CFG_GNSS_PAYLOAD_INIT_SZ + UBX_CFG_GNSS_PAYLOAD_CFG_BLK_SZ * n
#define UBX_CFG_GNSS_FRM_SZ(n)		U_BLOX_FRM_SZ_WO_PAYLOAD + UBX_CFG_GNSS_PAYLOAD_SZ(n)

int u_blox_create_frame(uint8_t *ubx_frame, uint16_t ubx_frame_size,
			uint8_t message_class, uint8_t message_id,
			const void *const data, uint16_t payload_size);

struct u_blox_cfg_prt_poll_data {
	uint8_t port_id;
};
void u_blox_cfg_prt_poll_data_default(struct u_blox_cfg_prt_poll_data *data);

struct u_blox_cfg_prt_set_data {
	uint8_t port_id;
	uint8_t reserved0;
	uint16_t tx_ready_pin_conf;
	uint32_t port_mode;
	uint32_t baudrate;
	uint16_t in_proto_mask;
	uint16_t out_proto_mask;
	uint16_t flags;
	uint8_t reserved1;
};
void u_blox_cfg_prt_set_data_default(struct u_blox_cfg_prt_set_data *data);

#define U_BLOX_CFG_PRT_SET_DATA_INIT(inst)		\
	struct u_blox_cfg_prt_set_data inst;		\
	(void) u_blox_cfg_prt_set_data_default(&inst);

#define UBX_CFG_RST_NAV_BBR_MASK_HOT_START	0x0000
#define UBX_CFG_RST_NAV_BBR_MASK_WARM_START	0x0001
#define UBX_CFG_RST_NAV_BBR_MASK_COLD_START	0xFFFF

#define UBX_CFG_RST_RESET_MODE_HARD_RESET				0x00
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_SOFT_RESET			0x01
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_SOFT_RESET_GNSS_ONLY		0x02
#define UBX_CFG_RST_RESET_MODE_HARD_RESET_AFTER_SHUTDOWN		0x04
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_GNSS_STOP			0x08
#define UBX_CFG_RST_RESET_MODE_CONTROLLED_GNSS_START			0x09

struct u_blox_cfg_rst_data {
	uint16_t nav_bbr_mask;
	uint8_t reset_mode;
	uint8_t reserved0;
};
void u_blox_cfg_rst_data_default(struct u_blox_cfg_rst_data *data);

#define U_BLOX_CFG_RST_DATA_INIT(inst)			\
	struct u_blox_cfg_rst_data inst;		\
	(void) u_blox_cfg_rst_data_default(&inst);

struct u_blox_cfg_nav5_data {
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

	uint16_t reserved0;

	uint16_t static_hold_dist_threshold;
	uint8_t utc_standard;
};
void u_blox_cfg_nav5_data_default(struct u_blox_cfg_nav5_data *data);

#define U_BLOX_CFG_NAV5_DATA_INIT(inst)			\
	struct u_blox_cfg_nav5_data inst;		\
	(void) u_blox_cfg_nav5_data_default(&inst);

#define U_BLOX_CFG_GNSS_DATA_MSG_VER			0x00
#define U_BLOX_CFG_GNSS_DATA_NUM_TRK_CH_HW_DEFAULT		0x31
#define U_BLOX_CFG_GNSS_DATA_NUM_TRK_CH_USE_DEFAULT		0x31

#define U_BLOX_CFG_GNSS_DATA_RESERVED0			0x00
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_ENABLE			BIT(0)
#define UBX_SGN_CNF_SHIFT						16
/* When gnssId is 0 (GPS) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_GPS_L1C_A	0x01 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_GPS_L2C	0x10 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_GPS_L5	0x20 << UBX_SGN_CNF_SHIFT
/* When gnssId is 1 (SBAS) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_SBAS_L1C_A	0x01 << UBX_SGN_CNF_SHIFT
/* When gnssId is 2 (Galileo) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_Galileo_E1	0x01 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_Galileo_E5A	0x10 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_Galileo_E5B	0x20 << UBX_SGN_CNF_SHIFT
/* When gnssId is 3 (BeiDou) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_BeiDou_B1I	0x01 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_BeiDou_B2I	0x10 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_BeiDou_B2A	0x80 << UBX_SGN_CNF_SHIFT
/* When gnssId is 4 (IMES) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_IMES_L1	0x01 << UBX_SGN_CNF_SHIFT
/* When gnssId is 5 (QZSS) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_QZSS_L1C_A	0x01 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_QZSS_L1S	0x04 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_QZSS_L2C	0x10 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_QZSS_L5	0x20 << UBX_SGN_CNF_SHIFT
/* When gnssId is 6 (GLONASS) */
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_GLONASS_L1	0x01 << UBX_SGN_CNF_SHIFT
#define U_BLOX_CFG_GNSS_DATA_CNF_BLK_FLAG_SGN_CNF_MASK_GLONASS_L2	0x10 << UBX_SGN_CNF_SHIFT

struct u_blox_cfg_gnss_data_config_block {
	uint8_t gnssId;
	uint8_t num_res_trk_ch;
	uint8_t max_num_trk_ch;
	uint8_t reserved0;
	uint32_t flags;
};
struct u_blox_cfg_gnss_data {
	uint8_t msg_ver;
	uint8_t num_trk_ch_hw;
	uint8_t num_trk_ch_use;
	uint8_t num_config_blocks;
	struct u_blox_cfg_gnss_data_config_block config_blocks[];
};
void u_blox_cfg_gnss_data_default(struct u_blox_cfg_gnss_data *data);

#define U_BLOX_CFG_GNSS_DATA_INIT(inst)			\
	struct u_blox_cfg_gnss_data inst;		\
	(void) u_blox_cfg_gnss_data_default(&inst);

#define U_BLOX_CFG_MSG_DATA_RATE_DEFAULT	1

struct u_blox_cfg_msg_data {
	uint8_t message_class;
	uint8_t message_id;
	uint8_t rate;
};
void u_blox_cfg_msg_data_default(struct u_blox_cfg_msg_data *data);

#define U_BLOX_CFG_MSG_DATA_INIT(inst)			\
	struct u_blox_cfg_msg_data inst;		\
	(void) u_blox_cfg_msg_data_default(&inst);

#endif /* ZEPHYR_U_BLOX_PROTOCOL_ */
