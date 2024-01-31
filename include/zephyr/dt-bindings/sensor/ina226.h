/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Referred from file "zephyr/include/zephyr/dt-bindings/sensor/ina230.h". */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_INA226_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_INA226_H_

#include <zephyr/dt-bindings/dt-util.h>

/* Reset Mode. */
#define INA226_RST_NORMAL_OPERATION	0x00
#define INA226_RST_SYSTEM_RESET		0x01

/* Averaging Mode. */
#define INA226_AVG_MODE_1    0x00
#define INA226_AVG_MODE_4    0x01
#define INA226_AVG_MODE_16   0x02
#define INA226_AVG_MODE_64   0x03
#define INA226_AVG_MODE_128  0x04
#define INA226_AVG_MODE_256  0x05
#define INA226_AVG_MODE_512  0x06
#define INA226_AVG_MODE_1024 0x07

/* Conversion time for bus and shunt voltage in micro-seconds. */
#define INA226_CONV_TIME_140				0x00
#define INA226_CONV_TIME_204				0x01
#define INA226_CONV_TIME_332				0x02
#define INA226_CONV_TIME_588				0x03
#define INA226_CONV_TIME_1100				0x04
#define INA226_CONV_TIME_2116				0x05
#define INA226_CONV_TIME_4156				0x06
#define INA226_CONV_TIME_8244				0x07

/* Operating Mode. */
#define INA226_OPER_MODE_POWER_DOWN			0x00
#define INA226_OPER_MODE_SHUNT_VOLTAGE_TRIG		0x01
#define INA226_OPER_MODE_BUS_VOLTAGE_TRIG		0x02
#define INA226_OPER_MODE_SHUNT_BUS_VOLTAGE_TRIG		0x03
#define INA226_OPER_MODE_SHUNT_VOLTAGE_CONT		0x05
#define INA226_OPER_MODE_BUS_VOLTAGE_CONT		0x06
#define INA226_OPER_MODE_SHUNT_BUS_VOLTAGE_CONT		0x07

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_INA226_H_ */
