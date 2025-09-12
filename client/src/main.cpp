#include <Arduino.h>
#include <esp_now.h>

#include "connect.h"

#define POWER_BUTTON_PIN 10
#define RESET_BUTTON_PIN 2

int previousPowerButtonState = HIGH;
int previousResetButtonState = HIGH;

bool need_power_ack = false;
bool need_reset_ack = false;

bool wait_power_ack = false;
bool wait_reset_ack = false;

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

	pinMode(POWER_LED_PIN, OUTPUT);
	pinMode(RESET_LED_PIN, OUTPUT);

	delay(3000);
	Serial.println("Client booted!");

	wifiSetup();
}

void loop()
{
	delay(20); // used for button debounce

	// digitalWrite(POWER_LED_PIN, LOW);
	digitalWrite(RESET_LED_PIN, LOW);

	int powerButtonState = digitalRead(POWER_BUTTON_PIN);
	int resetButtonState = digitalRead(RESET_BUTTON_PIN);

	// Unsupported usecase
	if (powerButtonState == LOW && resetButtonState == LOW) {
		previousPowerButtonState = HIGH;
		previousResetButtonState = HIGH;
		return;
	}

	bool isButtonPressed = (previousPowerButtonState == LOW && powerButtonState == LOW)
	                       || (previousResetButtonState == LOW && resetButtonState == LOW);
	previousPowerButtonState = powerButtonState;
	previousResetButtonState = resetButtonState;

	if (isButtonPressed) {
		digitalWrite(RESET_LED_PIN, HIGH);

		String button = (powerButtonState == LOW) ? "power" : "reset";

		Serial.println("Pressed " + button + " button!");

		String msg = button + "_pressed";
		esp_now_send(server_mac, (const uint8_t*)msg.c_str(), msg.length());
		Serial.println("Sent " + button + " button press");

		need_power_ack = true;
		need_reset_ack = true;

		wait_power_ack = false;
		wait_reset_ack = false;

		return;
	}

	if (need_power_ack && !wait_power_ack) {
		String msg = "power_released";
		esp_now_send(server_mac, (const uint8_t*)msg.c_str(), msg.length());
		Serial.println("Sent power button release");

		wait_power_ack = true;
	}
	if (need_reset_ack && !wait_reset_ack) {
		String msg = "reset_released";
		esp_now_send(server_mac, (const uint8_t*)msg.c_str(), msg.length());
		Serial.println("Sent reset button release");

		wait_reset_ack = true;
	}
}
