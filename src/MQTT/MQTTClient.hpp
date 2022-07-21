#pragma once

#include <WiFi.h>
#include <AsyncMqttClient.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#define MQTT_HOST IPAddress(192, 168, 0, 186)
#define MQTT_PORT 1883

namespace NeoN
{
	namespace MQTTClient
	{

		AsyncMqttClient Client;
		TimerHandle_t mqttReconnectTimer;

		void ConnectToMqtt()
		{
			Serial.println("Connecting to MQTT...");
			Client.connect();
		}

		void OnMqttConnect(bool sessionPresent)
		{
			Serial.println("Connected to MQTT.");
		}

		void OnMqttDisconnect(AsyncMqttClientDisconnectReason reason)
		{
			Serial.println("Disconnected from MQTT.");

			if (WiFi.isConnected())
			{
				xTimerStart(mqttReconnectTimer, 0);
			}
		}

		void WiFiEvent(WiFiEvent_t event)
		{
			Serial.printf("[WiFi-event] event: %d\n", event);
			switch (event)
			{
			case SYSTEM_EVENT_STA_GOT_IP:
				ConnectToMqtt();
				break;
			case SYSTEM_EVENT_STA_DISCONNECTED:
				Serial.println("WiFi lost connection");
				xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
				break;
			default:
				break;
			}
		}

		void Setup()
		{
			mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(ConnectToMqtt));

			WiFi.onEvent(WiFiEvent);

			Client.onConnect(OnMqttConnect);
			Client.onDisconnect(OnMqttDisconnect);
			Client.setServer(MQTT_HOST, MQTT_PORT);
		}
	}
}
