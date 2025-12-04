/* SPDX-License-Identifier: Apache-2.0
 * Copyright 2024 NXP
 * Copyright (C) 2021, Soren Friis
 *
 * Referred from https://github.com/opendroneid/transmitter-linux
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/util.h>
#include <math.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#include "opendroneid.h"

#define ODID_PAYLOAD_LEN (4 + 3 + ODID_PACK_MAX_MESSAGES * ODID_MESSAGE_SIZE)

static uint8_t payload[ODID_PAYLOAD_LEN] = {
	0xFA, 0xFF, /* 0xFFFA = ASTM International, ASTM Remote ID. */
	0x0D,       /* AD Application Code within the ASTM address space = ODID. */
	0x00,       /* message counter starting at 0x00 and wrapping around at 0xFF. */
	0x00,       /* 3 + ODID_PACK_MAX_MESSAGES * ODID_MESSAGE_SIZE bytes. */
};

static const struct bt_data ad[] = {
	{
		.type = BT_DATA_SVC_DATA16,
		.data_len = ODID_PAYLOAD_LEN,
		.data = payload,
	},
};

const struct bt_le_adv_param bt_adv_param = {
	.id = BT_ID_DEFAULT,
	.sid = 0,
	.secondary_max_skip = 0,
	.options = BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_IDENTITY,
	.interval_min = BT_GAP_ADV_FAST_INT_MIN_1,
	.interval_max = BT_GAP_ADV_FAST_INT_MAX_1,
	.peer = NULL,
};

#define MINIMUM(a, b) (((a) < (b)) ? (a) : (b))

#define BASIC_ID_POS_ZERO 0
#define BASIC_ID_POS_ONE  1

static struct ODID_UAS_Data uasData;
ODID_MessagePack_data message_pack_data;
ODID_MessagePack_encoded message_pack_encoded;

static void odid_fill_example_data(struct ODID_UAS_Data *uasData)
{
	uasData->BasicID[BASIC_ID_POS_ZERO].UAType = ODID_UATYPE_HELICOPTER_OR_MULTIROTOR;
	uasData->BasicID[BASIC_ID_POS_ZERO].IDType = ODID_IDTYPE_SERIAL_NUMBER;
	char uas_id[] = "112624150A90E3AE1EC0";

	strncpy(uasData->BasicID[BASIC_ID_POS_ZERO].UASID, uas_id,
		MINIMUM(sizeof(uas_id), sizeof(uasData->BasicID[BASIC_ID_POS_ZERO].UASID)));

	uasData->BasicID[BASIC_ID_POS_ONE].UAType = ODID_UATYPE_HELICOPTER_OR_MULTIROTOR;
	uasData->BasicID[BASIC_ID_POS_ONE].IDType = ODID_IDTYPE_SPECIFIC_SESSION_ID;
	char uas_caa_id[] = "FD3454B778E565C24B70";

	strncpy(uasData->BasicID[BASIC_ID_POS_ONE].UASID, uas_caa_id,
		MINIMUM(sizeof(uas_caa_id), sizeof(uasData->BasicID[BASIC_ID_POS_ONE].UASID)));

	uasData->Auth[0].AuthType = ODID_AUTH_UAS_ID_SIGNATURE;
	uasData->Auth[0].DataPage = 0;
	uasData->Auth[0].LastPageIndex = 2;
	uasData->Auth[0].Length = 63;
	uasData->Auth[0].Timestamp = 28000000;
	char auth0_data[] = "12345678901234567";

	memcpy(uasData->Auth[0].AuthData, auth0_data,
	       MINIMUM(sizeof(auth0_data), sizeof(uasData->Auth[0].AuthData)));

	uasData->Auth[1].AuthType = ODID_AUTH_UAS_ID_SIGNATURE;
	uasData->Auth[1].DataPage = 1;
	char auth1_data[] = "12345678901234567890123";

	memcpy(uasData->Auth[1].AuthData, auth1_data,
	       MINIMUM(sizeof(auth1_data), sizeof(uasData->Auth[1].AuthData)));

	uasData->Auth[2].AuthType = ODID_AUTH_UAS_ID_SIGNATURE;
	uasData->Auth[2].DataPage = 2;
	char auth2_data[] = "12345678901234567890123";

	memcpy(uasData->Auth[2].AuthData, auth2_data,
	       MINIMUM(sizeof(auth2_data), sizeof(uasData->Auth[2].AuthData)));

	uasData->SelfID.DescType = ODID_DESC_TYPE_TEXT;
	char description[] = "Drone ID test flight---";

	strncpy(uasData->SelfID.Desc, description,
		MINIMUM(sizeof(description), sizeof(uasData->SelfID.Desc)));

	uasData->System.OperatorLocationType = ODID_OPERATOR_LOCATION_TYPE_TAKEOFF;
	uasData->System.ClassificationType = ODID_CLASSIFICATION_TYPE_EU;
	uasData->System.OperatorLatitude = uasData->Location.Latitude + 0.001;
	uasData->System.OperatorLongitude = uasData->Location.Longitude - 0.001;
	uasData->System.AreaCount = 1;
	uasData->System.AreaRadius = 0;
	uasData->System.AreaCeiling = 0;
	uasData->System.AreaFloor = 0;
	uasData->System.CategoryEU = ODID_CATEGORY_EU_OPEN;
	uasData->System.ClassEU = ODID_CLASS_EU_CLASS_1;
	uasData->System.OperatorAltitudeGeo = 20.5f;
	uasData->System.Timestamp = 28056789;

	uasData->OperatorID.OperatorIdType = ODID_OPERATOR_ID;
	char operatorId[] = "FIN87astrdge12k8";

	strncpy(uasData->OperatorID.OperatorId, operatorId,
		MINIMUM(sizeof(operatorId), sizeof(uasData->OperatorID.OperatorId)));
}

static void odid_fill_example_gps_data(struct ODID_UAS_Data *uasData)
{
	uasData->Location.Status = ODID_STATUS_AIRBORNE;
	uasData->Location.Direction = 361.f;
	uasData->Location.SpeedHorizontal = 0.0f;
	uasData->Location.SpeedVertical = 0.35f;
	uasData->Location.Latitude = 51.4791;
	uasData->Location.Longitude = -0.0013;
	uasData->Location.AltitudeBaro = 100;
	uasData->Location.AltitudeGeo = 110;
	uasData->Location.HeightType = ODID_HEIGHT_REF_OVER_GROUND;
	uasData->Location.Height = 80;
	uasData->Location.HorizAccuracy = createEnumHorizontalAccuracy(5.5f);
	uasData->Location.VertAccuracy = createEnumVerticalAccuracy(9.5f);
	uasData->Location.BaroAccuracy = createEnumVerticalAccuracy(0.5f);
	uasData->Location.SpeedAccuracy = createEnumSpeedAccuracy(0.5f);
	uasData->Location.TSAccuracy = createEnumTimestampAccuracy(0.1f);
	uasData->Location.TimeStamp = 360.52f;
}

int odid_message_pack_data_init(ODID_MessagePack_data *message_pack_data)
{
	int err;

	/* Initialize UAS data. */
	odid_initUasData(&uasData);
	odid_fill_example_data(&uasData);
	odid_fill_example_gps_data(&uasData);

	odid_initMessagePackData(message_pack_data);
	message_pack_data->MsgPackSize = ODID_PACK_MAX_MESSAGES;

	err = encodeBasicIDMessage((ODID_BasicID_encoded *)&message_pack_data->Messages[0], &uasData.BasicID[0]);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeBasicIDMessage((ODID_BasicID_encoded *)&message_pack_data->Messages[1], &uasData.BasicID[1]);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeLocationMessage((ODID_Location_encoded *)&message_pack_data->Messages[2], &uasData.Location);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeAuthMessage((ODID_Auth_encoded *)&message_pack_data->Messages[3], &uasData.Auth[0]);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeAuthMessage((ODID_Auth_encoded *)&message_pack_data->Messages[4], &uasData.Auth[1]);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeAuthMessage((ODID_Auth_encoded *)&message_pack_data->Messages[5], &uasData.Auth[2]);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeSelfIDMessage((ODID_SelfID_encoded *)&message_pack_data->Messages[6], &uasData.SelfID);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeSystemMessage((ODID_System_encoded *)&message_pack_data->Messages[7], &uasData.System);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}
	err = encodeOperatorIDMessage((ODID_OperatorID_encoded *)&message_pack_data->Messages[8], &uasData.OperatorID);
	if (err != ODID_SUCCESS) {
		printf("Encoding ODID message failed. err = %d.\n", err);
		return -1;
	}

	return 0;
}

int odid_update_message_pack_encoded(ODID_MessagePack_encoded *message_pack_encoded)
{
	int err;

	static double theta = 0.0;
	static double radius = 0.0;

	uasData.Location.Latitude -= radius * cos(theta);
	uasData.Location.Longitude += (radius * 1.5) * sin(theta);
	err = encodeLocationMessage((ODID_Location_encoded *)&message_pack_data.Messages[2], &uasData.Location);
	if (err != ODID_SUCCESS) {
		printf("Encoding location message failed. err = %d.\n", err);
		return -1;
	}

	err = encodeMessagePack(message_pack_encoded, &message_pack_data);
	if (err != ODID_SUCCESS) {
		printf("Encoding message pack data failed. err = %d.\n", err);
		return -1;
	}

	memcpy(&payload[4], message_pack_encoded, sizeof(*message_pack_encoded));

	theta += 0.25;
	radius += 0.0001;

	return 0;
}

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_NODE DT_NODELABEL(lpuart0)

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define HDR_BYTE 0xA5

typedef enum {
    ST_SYNC,
    ST_LEN,
    ST_DATA,
    ST_CRC1,
    ST_CRC2
} rx_state_t;

static uint16_t crc16_ccitt_false(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; ++b) {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

static inline float float_from_le_bytes(const uint8_t b[4]) {
    float f; memcpy(&f, b, 4); return f;
}

int main(void)
{
	int err;
	struct bt_le_ext_adv *adv;

	printf("Starting ODID Demo\n");

	const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

	if (!device_is_ready(uart_dev)) {
		printf("UART device not ready!\n");
		return -1;
	}

	printf("UART poll demo started (baud set in DTS, expecting 57600)...\n");
    rx_state_t st = ST_SYNC;
    uint8_t len = 0;
    uint8_t data[32];
    uint8_t idx = 0;
    uint16_t rx_crc = 0;

    while (1) {
        uint8_t c;
        if (uart_poll_in(uart_dev, &c) == 0) {
            switch (st) {
            case ST_SYNC:
                st = (c == HDR_BYTE) ? ST_LEN : ST_SYNC;
                break;

            case ST_LEN:
                len = c;
                if (len == 0 || len > sizeof(data)) {
                    st = ST_SYNC; // invalid; resync
                } else {
                    idx = 0;
                    st = ST_DATA;
                }
                break;

            case ST_DATA:
                data[idx++] = c;
                if (idx >= len) {
                    st = ST_CRC1;
                }
                break;

            case ST_CRC1:
                rx_crc = ((uint16_t)c) << 8;
                st = ST_CRC2;
                break;

            case ST_CRC2: {
                rx_crc |= c;

                // Compute CRC over [LEN || PAYLOAD]
                uint8_t crc_buf[1 + idx];
                crc_buf[0] = len;
                memcpy(&crc_buf[1], data, idx);
                uint16_t calc = crc16_ccitt_false(crc_buf, sizeof(crc_buf));

                if (calc == rx_crc) {
                    if (len == 8) {
                        float f1 = float_from_le_bytes(&data[0]);
                        float f2 = float_from_le_bytes(&data[4]);
                        printf("Floats: %f, %f\n", f1, f2);
                    } else {
                        printf("Valid frame len=%u (not 8)\n", len);
                    }
                } else {
                    printf("CRC mismatch: calc=0x%04X rx=0x%04X\n", calc, rx_crc);
                }
                st = ST_SYNC; // resync for next frame
                break;
            }
            }
        } else {
            k_sleep(K_USEC(200)); // avoid busy-wait
        }
    }

	while (1) {
		uint8_t c;
		/* uart_poll_in returns 0 when a byte was read, -1 if nothing available */
		if (uart_poll_in(uart_dev, &c) == 0) {
			/* Print received byte; as char and hex for visibility */
			printf("RX: '%c' (0x%02X)\n", (c >= 32 && c < 127) ? c : '.', c);
			// rx_two_floats_raw(c);
		}

		/* Small sleep to avoid 100% CPU in tight loop; tune as needed */
		// k_msleep(1);
	}

	err = odid_message_pack_data_init(&message_pack_data);
	if (err != ODID_SUCCESS) {
		printf("Initializing message pack data failed. err = %d.\n", err);
		return -1;
	}

	err = encodeMessagePack(&message_pack_encoded, &message_pack_data);
	if (err != ODID_SUCCESS) {
		printf("Encoding message pack data failed. err = %d.\n", err);
		return -1;
	}

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(NULL);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return -1;
	}

	/* Create a non-connectable advertising set */
	err = bt_le_ext_adv_create(&bt_adv_param, NULL, &adv);
	if (err) {
		printk("Failed to create advertising set (err %d)\n", err);
		return 0;
	}

	while (true) {
		if (bt_is_ready()) {
			break;
		}

		printf("Bluetooth not ready. Checking again in 100 ms\n");
		k_sleep(K_MSEC(100));
	}

	memcpy(&payload[4], &message_pack_encoded, sizeof(message_pack_encoded));

	err = bt_le_ext_adv_set_data(adv, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err != 0) {
		printk("Failed to set extended advertising data (err %d)\n", err);
		return err;
	}

	err = bt_le_ext_adv_start(adv, BT_LE_EXT_ADV_START_DEFAULT);
	if (err) {
		printk("Failed to start extended advertising (err %d)\n", err);
		return 0;
	}

	while (true) {
		odid_update_message_pack_encoded(&message_pack_encoded);

		err = bt_le_ext_adv_set_data(adv, ad, ARRAY_SIZE(ad), NULL, 0);
		if (err != 0) {
			printk("Failed to set extended advertising data (err %d)\n", err);
			return err;
		}

		k_sleep(K_MSEC(250));
	}

	return 0;
}
