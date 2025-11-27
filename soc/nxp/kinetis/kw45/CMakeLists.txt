# Copyright 2024 NXP
#
# SPDX-License-Identifier: Apache-2.0

zephyr_sources(../../mcx/mcxw/mcxw7xx/soc.c ../../mcx/mcxw/mcxw7xx/mcxw71_platform_init.S)

zephyr_include_directories(../../mcx/mcxw/mcxw7xx/)

zephyr_sources_ifdef(
  CONFIG_PM
  power.c
)

set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/../../mcx/mcxw/mcxw7xx/linker.ld CACHE INTERNAL "")

zephyr_linker_sources_ifdef(CONFIG_BT RAM_SECTIONS ../../mcx/mcxw/mcxw7xx/sections.ld)

zephyr_sources_ifdef(CONFIG_NXP_NBU
  ../../common/nxp_nbu.c
  )
