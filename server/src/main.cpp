#include <Arduino.h>

#include "connect.h"

#define POWER_BUTTON_PIN 10
#define RESET_BUTTON_PIN 2

#define POWER_LED_PIN 3
#define RESET_LED_PIN 1

#define POWER_GATE_PIN 6
#define RESET_GATE_PIN 7

int previousPowerButtonState = HIGH;
int previousResetButtonState = HIGH;

bool powerButtonPressed = false;
bool resetButtonPressed = false;

void processButtons();
void setPowerPin(bool enable);
void setResetPin(bool enable);

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

	pinMode(POWER_GATE_PIN, OUTPUT);
	pinMode(POWER_LED_PIN, OUTPUT);
	pinMode(RESET_GATE_PIN, OUTPUT);
	pinMode(RESET_LED_PIN, OUTPUT);

	delay(3000);
	Serial.println("Server booted!");

	wifiSetup();
}

void loop()
{
	delay(20); // used for button debounce

	int powerButtonState = digitalRead(POWER_BUTTON_PIN);
	int resetButtonState = digitalRead(RESET_BUTTON_PIN);

	// Unsupported usecase
	if (powerButtonState == LOW && resetButtonState == LOW) {
		previousPowerButtonState = HIGH;
		previousResetButtonState = HIGH;
		powerButtonPressed = false;
		resetButtonPressed = false;
		return;
	}

	powerButtonPressed = powerButtonState == LOW;
	resetButtonPressed = resetButtonState == LOW;

	processButtons();
}

// -----------------------------------------

void processButtons()
{
	if ((powerRemotePressed && resetRemotePressed)
	    || (powerButtonPressed && resetButtonPressed)) {
		Serial.println("Double input..");
		setPowerPin(false);
		setResetPin(false);
		return;
	}

	setPowerPin(powerRemotePressed || powerButtonPressed);
	setResetPin(resetRemotePressed || resetButtonPressed);
}

void setPowerPin(bool enable)
{
	// digitalWrite(POWER_GATE_PIN, (enable) ? HIGH : LOW);
	digitalWrite(POWER_LED_PIN, (enable) ? HIGH : LOW);
}

void setResetPin(bool enable)
{
	// digitalWrite(RESET_GATE_PIN, (enable) ? HIGH : LOW);
	digitalWrite(RESET_LED_PIN, (enable) ? HIGH : LOW);
}

// connect + to drain
// connect - to source
