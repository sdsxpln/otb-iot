/*
 * OTB-IOT - Out of The Box Internet Of Things
 *
 * Copyright (C) 2016-8 Piers Finlayson
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version. 
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define OTB_MAIN_C
#include "otb.h"

void configModeCallback();
char ssid[32];
void mbus_init();

void ICACHE_FLASH_ATTR user_init(void)
{
  // According to docs required as first step, to enable us timer
  // Note that this means that maximum os_timer_arm (not us) value is about
  // 432s (0x689D0 in ms)
  system_timer_reinit();

  otb_util_init_logging();

  DEBUG("OTB: user_init entry");
  
  // Log some useful info
  otb_util_log_useful_info(FALSE);

  // Do some sanity checking
  otb_util_check_defs();

  // Initial internal I2C bus (must be done before try and read eeprom)
  otb_i2c_initialize_bus_internal();
  
  // Read the eeprom (if present) - this initializes the chip ID
  otb_eeprom_read();

  // Relog heap size (now have read into eeprom)
  INFO("OTB: Free heap size: %d bytes", system_get_free_heap_size());

  // Initialise flash access (this makes it work if OTB_SUPER_BIG_FLASH_8266 if defined).
  otb_flash_init();
  
  // Initialize GPIO.  Must happen before we clear reset (as this uses GPIO), but
  // after have populated them 
  otb_gpio_init();
  
  // Reset GPIO - pull pin 16 high
  otb_util_clear_reset();
  
  // Initialize serial
  otb_serial_init();

  // Initialize wifi - mostly this just disables wifi until we're ready to turn it on!
  otb_wifi_init();

  // Initialize nixie module
  otb_nixie_module_init();

#if 0
  OTB_WIFI_STATION_CONFIG wifi_conf;
  // Some code to burn an SSID/password into the flash
  wifi_set_opmode_current(STATION_MODE);
  strcpy(wifi_conf.ssid, "some_ssid");
  strcpy(wifi_conf.password, "some_password");
  wifi_conf.bssid_set = FALSE;
  strcpy(wifi_conf.bssid, "");
  otb_wifi_set_stored_conf(&wifi_conf);
  INFO("Pausing for 20s ...");
  otb_util_delay_ms(20000);
#endif  

  // Initialize and load config
  otb_conf_init();
  otb_conf_load();
  
  otb_led_wifi_update(OTB_LED_NEO_COLOUR_BLUE, TRUE);

#if 0  
  if (otb_gpio_get(OTB_GPIO_RESET_PIN, TRUE))
  {
#endif
    system_init_done_cb((init_done_cb_t)otb_wifi_kick_off);
#if 0
  }
  else
  {
    system_init_done_cb((init_done_cb_t)otb_gpio_reset_kick_off);
  }
#endif
//  system_init_done_cb((init_done_cb_t)mbus_init);
  
  DEBUG("OTB: user_init exit");

  return;
}
