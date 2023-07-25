/*
   ACController.ino
   Created on: 2023-07-21
        Author: Aaron Wu
        
   This example is a Thermostat controller  (aka AC controller).

   This example includes one controller:
   1. Thermostat controller

   TODO:
   Add TV  support
*/
#define ENABLE_OTA  //if OTA need
#define SENSOR_TYPE_DHT

#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"

#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>


#define ACSEND \
  ac.sendAc();  \
  digitalWrite(kIrLed,HIGH);      //艾韵专用


#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

#ifdef SENSOR_TYPE_DHT
#include "DHT.h"   //https://github.com/adafruit/DHT-sensor-library
#define DHT11_PIN 4
DHT DHT(DHT11_PIN, DHT11);
#endif


// IR settings
const uint16_t kIrLed = 14;  // The ESP GPIO pin to use that controls the IR LED.
IRac ac(kIrLed);  // Create a A/C object using GPIO to sending messages with.

#define LED_PIN 2

// Globals
bool queueCommand = false;
void flipQueueCommand(bool newState) {
  Serial.write("Flipping queueCommand to %d\n", newState);
  queueCommand = newState;
}

void ir_init() {
  // Set up what we want to send.
  // See state_t, opmode_t, fanspeed_t, swingv_t, & swingh_t in IRsend.h for
  // all the various options.
  ac.next.protocol = decode_type_t::KELVINATOR;  // Set a protocol to use.
  ac.next.model = 1;  // Some A/Cs have different models. Try just the first.
  ac.next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
  ac.next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
  ac.next.degrees = 25;  // 25 degrees.
  ac.next.fanspeed = stdAc::fanspeed_t::kMedium;  // Start the fan at medium.
  ac.next.swingv = stdAc::swingv_t::kOff;  // Don't swing the fan up or down.
  ac.next.swingh = stdAc::swingh_t::kOff;  // Don't swing the fan left or right.
  ac.next.light = false;  // Turn off any LED/Lights/Display that we can.
  ac.next.beep = false;  // Turn off any beep from the A/C if we can.
  ac.next.econo = false;  // Turn off any economy modes if we can.
  ac.next.filter = false;  // Turn off any Ion/Mold/Health filters if we can.
  ac.next.turbo = false;  // Don't use any turbo/powerful/etc modes.
  ac.next.quiet = false;  // Don't use any quiet/silent/etc modes.
  ac.next.sleep = -1;  // Don't set any sleep time or modes.
  ac.next.clean = false;  // Turn off any Cleaning options if we can.
  ac.next.clock = -1;  // Don't set any current time if we can avoid it.
  ac.next.power = false;  // Initially start with the unit off.
}


void setup() {
  Serial.begin(115200);
  wifi_connect(); // in wifi_info.h
#ifdef SENSOR_TYPE_DHT
  DHT.begin();
#endif
  //homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
  my_homekit_setup();
  //ac_rotation_speed.value.int_value = 100;
  Serial.write("HomeKit setup complete. About to start ac.begin()\n");
  ir_init();
}

void loop() {
  my_homekit_loop();
  delay(10);
  if (queueCommand)
  {
    Serial.write("Sending AC Command....\n");
    ACSEND
    flipQueueCommand(false);
  }
}
//==============================
// HomeKit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t current_temperature;
extern "C" homekit_characteristic_t target_temperature;
extern "C" homekit_characteristic_t units;
extern "C" homekit_characteristic_t current_state;
extern "C" homekit_characteristic_t target_state;
extern "C" homekit_characteristic_t cooling_threshold;
extern "C" homekit_characteristic_t heating_threshold;
extern "C" homekit_characteristic_t current_humidity;

// --- GETTERS
homekit_value_t current_state_getter()
{
  LOG_D("CURRENT STATE GETTER CALLED! RESPONDING WITH: %d", current_state.value.int_value);

  return HOMEKIT_UINT8_CPP(current_state.value.int_value);

}

homekit_value_t target_state_getter()
{
  LOG_D("TARGET STATE GETTER CALLED! RESPONDING WITH: %d", target_state.value.int_value);
  return HOMEKIT_UINT8_CPP(target_state.value.int_value);

}

homekit_value_t target_temp_getter()
{
  LOG_D("TARGET TEMP GETTER CALLED! RESPONDING WITH: %f", target_temperature.value.float_value);
  return target_temperature.value;
}

// END GETTERS

void update_state();

void on_update(homekit_value_t value) {
  LOG_D("value:%d %d", value, value.int_value);
  target_state.value = value;
  update_state();
}


void update_state() {
  uint8_t state = target_state.value.int_value;
  if ((state == 1 && current_temperature.value.float_value < target_temperature.value.float_value) ||
      (state == 3 && current_temperature.value.float_value < heating_threshold.value.float_value)) {
    if (current_state.value.int_value != 1) {
      current_state.value = HOMEKIT_UINT8_CPP(1);
      homekit_characteristic_notify(&current_state, current_state.value);

      ac.next.model = 1;
      ac.next.degrees = target_temperature.value.int_value;
      ACSEND

    }
  } else if ((state == 2 && current_temperature.value.float_value > target_temperature.value.float_value) ||
             (state == 3 && current_temperature.value.float_value > cooling_threshold.value.float_value)) {
    if (current_state.value.int_value != 2) {
      current_state.value = HOMEKIT_UINT8_CPP(2);
      homekit_characteristic_notify(&current_state, current_state.value);

      ac.next.model = 2;
      ac.next.degrees = target_temperature.value.int_value;
      ACSEND
    }
  } else {
    if (current_state.value.int_value != 0) {
      current_state.value = HOMEKIT_UINT8_CPP(0);
      homekit_characteristic_notify(&current_state, current_state.value);
      ac.next.model = 0;
      ACSEND
    }
  }
}

void my_homekit_setup() {

  current_state.getter = current_state_getter;
  target_state.getter = target_state_getter;
  target_temperature.getter = target_temp_getter;

  target_temperature.setter = (on_update);
  current_state.setter = (on_update);
  target_state.setter = (on_update);
  cooling_threshold.setter = (on_update);
  heating_threshold.setter = (on_update);

  LOG_D("about to call arduino_homekit_setup\n");
  arduino_homekit_setup(&config);
}

static uint32_t next_heap_millis = 0;
static uint32_t next_report_millis = 0;

void my_homekit_loop() {
  arduino_homekit_loop();
  const uint32_t t = millis();
  if (t > next_report_millis) {
    // report sensor values every 10 seconds
    next_report_millis = t + 10 * 1000;
    my_homekit_report();
  }
  if (t > next_heap_millis) {
    // Show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
          ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

  }
}

void my_homekit_report() {
  float t = DHT.readTemperature();;
  float h = DHT.readHumidity();

  current_temperature.value.float_value = t;
  current_humidity.value.float_value = h;

  homekit_characteristic_notify(&current_temperature, current_temperature.value);
  homekit_characteristic_notify(&current_humidity, current_humidity.value);

  LOG_D("t %.1f, h %.1f", t, h);
  LOG_D("current_state %d, target_state %d,target_temperature %.1f", current_state.value.int_value, target_state.value.int_value, target_temperature.value);
}
