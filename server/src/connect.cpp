#include <cstdint>

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "connect.h"
#include "secrets.h"

uint8_t client_mac[6] = CLIENT_MAC;
uint8_t server_mac[6] = SERVER_MAC;

bool powerRemotePressed = false;
bool resetRemotePressed = false;

// -----------------------------------------

void wifiSetupEncryption();

void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
}

void onDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len)
{
	Serial.print("Received: ");
	Serial.write(data, data_len);
	Serial.println();

	auto msg = String((const char*)data, data_len);

	if (msg == "power_pressed") {
		powerRemotePressed = true;
	}
	else if (msg == "power_released") {
		powerRemotePressed = false;
	}
	else if (msg == "reset_pressed") {
		resetRemotePressed = true;
	}
	else if (msg == "reset_released") {
		resetRemotePressed = false;
	}
	else {
		return;
	}

	String reply = msg + "_ack";
	esp_now_send(client_mac, (const uint8_t*)reply.c_str(), reply.length());
}

void wifiSetup()
{
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(false);
	WiFi.disconnect();
	delay(100);

	esp_wifi_set_channel(static_cast<uint8_t>(CHANNEL), WIFI_SECOND_CHAN_NONE);

	wifiSetupEncryption();
}

void wifiSetupEncryption()
{
	esp_now_deinit();

	if (esp_now_init() != ESP_OK) {
		Serial.println("Failed to init ESP-NOW");
		return;
	}

	if (esp_now_set_pmk((const uint8_t*)PMK) != ESP_OK) {
		Serial.println("Failed to set PMK");
		return;
	}

	auto peerInfo = esp_now_peer_info_t {
		.channel = static_cast<uint8_t>(CHANNEL),
		.encrypt = true,
	};

	memcpy(peerInfo.peer_addr, client_mac, 6);
	memcpy(peerInfo.lmk, LMK, 16);

	if (esp_now_add_peer(&peerInfo) != ESP_OK) {
		Serial.println("Failed to add peer");
		return;
	}

	esp_now_register_send_cb(onDataSent);
	esp_now_register_recv_cb(onDataRecv);

	Serial.println("Configured encryption!");
}

// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html
// https://docs.arduino.cc/libraries/wifi/ (old)
