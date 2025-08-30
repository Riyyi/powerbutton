#include <Arduino.h>
#include <WiFiClient.h>

#include "connect.h"

#define PORT 1234

#define POWER_BUTTON_PIN 10
#define RESET_BUTTON_PIN 2
#define POWER_LED_PIN 3
#define RESET_LED_PIN 1

WiFiClient client;

int previousPowerButtonState = HIGH;
int previousResetButtonState = HIGH;

void ack();

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

	pinMode(POWER_LED_PIN, OUTPUT);
	pinMode(RESET_LED_PIN, OUTPUT);

	wifiSetup();

	delay(3000);
	Serial.println("Client booted!");
}

void loop()
{
	delay(20); // used for button debounce

	wifiConnect();

	digitalWrite(POWER_LED_PIN, LOW);
	digitalWrite(RESET_LED_PIN, LOW);

	int powerButtonState = digitalRead(POWER_BUTTON_PIN);
	int resetButtonState = digitalRead(RESET_BUTTON_PIN);

	// Unsupported usecase
	if (powerButtonState == LOW && resetButtonState == LOW) {
		previousPowerButtonState = HIGH;
		previousResetButtonState = HIGH;
		if (client.connected()) {
			client.stop();
		}
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

		if (!client.connected() && !client.connect(wifiHost(), PORT)) {
			Serial.println("Connection failed");
			return;
		}

		client.print(button + "_pressed\n");
		Serial.println("Sent " + button + " button press");

		ack();
	}
	else {
		if (!client.connected()) {
			return;
		}

		client.print("power_released\n");
		Serial.println("Sent power button release");
		ack();

		client.print("reset_released\n");
		Serial.println("Sent reset button release");
		ack();

		client.stop();
	}
}

// -----------------------------------------

void ack()
{
	while (client.connected()) {
		if (client.available()) {
			// Wait for acknowledgment from the receiver
			String response = client.readStringUntil('\n');
			if (response == "ACK") {
				digitalWrite(POWER_LED_PIN, HIGH);
				Serial.println("Received ACK");
				return;
			}
		}
	}
}
