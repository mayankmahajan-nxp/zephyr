/* SPDX-License-Identifier: Apache-2.0
 * Copyright 2024 NXP
 * Copyright (C) 2021, Soren Friis
 *
 * Referred from https://github.com/opendroneid/transmitter-linux
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/util.h>

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

#define MINIMUM(a, b) (((a) < (b)) ? (a) : (b))

#define BASIC_ID_POS_ZERO 0
#define BASIC_ID_POS_ONE  1

ODID_MessagePack_data message_pack_data;
ODID_MessagePack_encoded message_pack_encoded;

int odid_update_adv_data(struct bt_le_ext_adv *adv, ODID_MessagePack_encoded *message_pack_encoded);

void odid_bt_data_sent_cb(struct bt_le_ext_adv *adv, struct bt_le_ext_adv_sent_info *info)
{
	int err;
	static uint8_t num_sent_prev = 0;
	uint8_t num_sent_curr = info->num_sent;

	printk("num_sent_prev = %d, num_sent_curr = %d\n", num_sent_prev, num_sent_curr);
	if (num_sent_prev < num_sent_curr || num_sent_curr == 0) {
		err = odid_update_adv_data(adv, &message_pack_encoded);
		if (err != 0) {
			printf("Updating bluetooth advertising data failed. err = %d.\n", err);
		};

	}

	num_sent_prev = num_sent_curr;

	return;
}

const static struct bt_le_ext_adv_cb bt_le_cb = {
	.sent = odid_bt_data_sent_cb,
};

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
	struct ODID_UAS_Data uasData;

	/* Initialize UAS data. */
	odid_initUasData(&uasData);
	odid_fill_example_data(&uasData);
	odid_fill_example_gps_data(&uasData);

	odid_initMessagePackData(message_pack_data);
	message_pack_data->MsgPackSize = 6;

	encodeBasicIDMessage((ODID_BasicID_encoded *)&message_pack_data->Messages[0], &uasData.BasicID[0]);
	encodeBasicIDMessage((ODID_BasicID_encoded *)&message_pack_data->Messages[1], &uasData.BasicID[1]);
	encodeLocationMessage((ODID_Location_encoded *)&message_pack_data->Messages[2], &uasData.Location);
	encodeAuthMessage((ODID_Auth_encoded *)&message_pack_data->Messages[3], &uasData.Auth[0]);
	encodeAuthMessage((ODID_Auth_encoded *)&message_pack_data->Messages[4], &uasData.Auth[1]);
	encodeAuthMessage((ODID_Auth_encoded *)&message_pack_data->Messages[5], &uasData.Auth[2]);
	encodeSelfIDMessage((ODID_SelfID_encoded *)&message_pack_data->Messages[6], &uasData.SelfID);
	encodeSystemMessage((ODID_System_encoded *)&message_pack_data->Messages[7], &uasData.System);
	encodeOperatorIDMessage((ODID_OperatorID_encoded *)&message_pack_data->Messages[8], &uasData.OperatorID);

	return 0;
}

int odid_bt_le_ext_adv_init(struct bt_le_ext_adv *adv)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printf("Bluetooth init failed (err %d)\n", err);
		return -1;
	}

	/* Create a non-connectable advertising set */
	const struct bt_le_adv_param param = {
		.id = BT_ID_DEFAULT,
		.sid = 0,
		.secondary_max_skip = 0,
		.options = BT_LE_ADV_OPT_EXT_ADV | BT_LE_ADV_OPT_USE_IDENTITY,
		.interval_min = BT_GAP_ADV_FAST_INT_MIN_1,
		.interval_max = BT_GAP_ADV_FAST_INT_MAX_1,
		.peer = NULL,
	};
	err = bt_le_ext_adv_create(&param, &bt_le_cb, &adv);
	if (err) {
		printk("Failed to create advertising set (err %d)\n", err);
		return -1;
	}

	while (true) {
		if (bt_is_ready()) {
			break;
		}

		printf("Bluetooth not ready. Checking again in 100 ms\n");
		k_sleep(K_MSEC(100));
	}

	return 0;
}

int odid_update_adv_data(struct bt_le_ext_adv *adv, ODID_MessagePack_encoded *message_pack_encoded)
{
	int err;

	memcpy(&payload[4], message_pack_encoded, 3 + ODID_PACK_MAX_MESSAGES * ODID_MESSAGE_SIZE);

	err = bt_le_ext_adv_set_data(adv, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err != 0) {
		printk("Failed to set extended advertising data (err %d)\n", err);
		return err;
	}

	return 0;
}

int main(void)
{
	int err;
	struct bt_le_ext_adv *adv = NULL;

	printf("Starting ODID Demo\n");

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

	/* Initialize the Bluetooth Subsystem. */
	err = odid_bt_le_ext_adv_init(adv);
	if (err != 0) {
		printf("Initializing bluetooth extended advertising failed. err = %d.\n", err);
		return -1;
	};

	err = odid_update_adv_data(adv, &message_pack_encoded);
	if (err != 0) {
		printf("Updating bluetooth advertising data failed. err = %d.\n", err);
		return -1;
	};

	err = bt_le_ext_adv_start(adv, BT_LE_EXT_ADV_START_DEFAULT);
	if (err) {
		printk("Failed to start extended advertising (err %d)\n", err);
		return 0;
	}

	return 0;
}
