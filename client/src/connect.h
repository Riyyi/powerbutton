#include <cstdint>

#define CHANNEL 11

#define POWER_LED_PIN 3
#define RESET_LED_PIN 1

extern bool need_power_ack;
extern bool need_reset_ack;

extern bool wait_power_ack;
extern bool wait_reset_ack;

extern uint8_t client_mac[6];
extern uint8_t server_mac[6];

void wifiSetup();
