/*
   my_accessory.c
   Define the accessory in C language using the Macro in characteristics.h

   Created on: 2023-07-21
        Author: Aaron Wu
*/

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value) {
  printf("accessory identify\n");
}

homekit_characteristic_t current_temperature = HOMEKIT_CHARACTERISTIC_(
      CURRENT_TEMPERATURE, 0
    );
homekit_characteristic_t target_temperature  = HOMEKIT_CHARACTERISTIC_(
      TARGET_TEMPERATURE, 22
    );
homekit_characteristic_t units = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0);
homekit_characteristic_t current_state = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0);
homekit_characteristic_t target_state = HOMEKIT_CHARACTERISTIC_(
    TARGET_HEATING_COOLING_STATE, 0);
homekit_characteristic_t cooling_threshold = HOMEKIT_CHARACTERISTIC_(
      COOLING_THRESHOLD_TEMPERATURE, 25);
homekit_characteristic_t heating_threshold = HOMEKIT_CHARACTERISTIC_(
      HEATING_THRESHOLD_TEMPERATURE, 15);
homekit_characteristic_t current_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);

homekit_accessory_t *accessories[] = {

  HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_thermostat, .services = (homekit_service_t*[]) {
    HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"),
      HOMEKIT_CHARACTERISTIC(MANUFACTURER, "HaPK"),
      HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "001"),
      HOMEKIT_CHARACTERISTIC(MODEL, "MyThermostat"),
      HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
      HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
      NULL
    }),
    HOMEKIT_SERVICE(THERMOSTAT, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "Thermostat"),
                             &current_temperature,
                             &target_temperature,
                             &current_state,
                             &target_state,
                             &cooling_threshold,
                             &heating_threshold,
                             &units,
                             &current_humidity,
                             NULL
    }),
    NULL
  }),

  NULL
};

homekit_server_config_t config = {
  .accessories = accessories,
  .password = "111-11-111"
};
