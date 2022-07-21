#pragma once

#include <WiFi.h>
#include <Logon/Logon.hpp>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

namespace NeoN
{
	namespace WiFiConnection
	{
		static TimerHandle_t wifiReconnectTimer;

		static void Connect()
		{
			Serial.println("Connecting to Wi-Fi...");
			WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
		}

		static void WiFiEvent(WiFiEvent_t event)
		{
			Serial.printf("[WiFi-event] event: %d\n", event);
			switch (event)
			{
			case SYSTEM_EVENT_STA_GOT_IP:
				Serial.println("WiFi connected");
				Serial.println("IP address: ");
				Serial.println(WiFi.localIP());
				break;
			case SYSTEM_EVENT_STA_DISCONNECTED:
				Serial.println("WiFi lost connection");
				xTimerStart(wifiReconnectTimer, 0);
				break;
			default:
				break;
			}
		}

		static void Setup()
		{
			wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(Connect));
			WiFi.onEvent(WiFiEvent);

			Connect();
		}
	}
}