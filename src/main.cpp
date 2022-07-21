#include <stdio.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include "sdkconfig.h"
#include <Arduino.h>
#include <WiFi.h>
#include <driver/adc.h>
//  - framework-arduinoespressif32 3.10004.200129 (1.0.4)
//  - framework-espidf 3.30300.190916 (3.3.0)
//  - tool-esptoolpy 1.20600.0 (2.6.0)
//  - tool-mkspiffs 2.230.0 (2.30)
//  - toolchain-xtensa32 2.50200.80 (5.2.0)

#include <Influx/MoistureInflux.hpp>
#include <MQTT/MoistureMQTT.hpp>
#include <WiFiConnection.hpp>

Moisture *moisture[2];
MoistureMQTT *moistureMqtt[2];
MoistureInfluxConnector *moistureInflux[2];
int motor[2];
using namespace NeoN;
void setup()
{
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    // WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    delay(100);
    WiFiConnection::Setup();
    MQTTClient::Setup();

    // MQTTClient::Client.subscribe("Moisture", 1);

    Serial.println("Build Sensors");
    moisture[0] = new Moisture(ADC1_CHANNEL_6);
    moisture[1] = new Moisture(ADC1_CHANNEL_7);

    Serial.println("Setup Pumps");
    motor[0] = 26;
    motor[1] = 27;

    for (int i = 0; i < 2; i++)
        pinMode(motor[i], OUTPUT);

    Serial.print("Connect WiFi ...");
    while (!WiFi.isConnected())
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println("");
    // Serial.println("Build MQTT Wrapper");
    moistureMqtt[0] = new MoistureMQTT(moisture[0], &MQTTClient::Client);
    moistureMqtt[1] = new MoistureMQTT(moisture[1], &MQTTClient::Client);

    Serial.println("Build Influx Wrapper");
    moistureInflux[0] = new MoistureInfluxConnector(moisture[0]);
    moistureInflux[1] = new MoistureInfluxConnector(moisture[1]);

    MQTTClient::Client.beginRun();
}

void loop()
{
    Serial.print("Send Data: ");
    Serial.println(millis());

    for (int i = 0; i < 2; i++)
    {
        // moistureMqtt[i]->SendData();
        // moistureMqtt[i]->SendMoisture();
        // Serial.println();

        moistureInflux[i]->SendDataPoint();
        // if (moist <=1 0.4){
        //     sprintf(msg, "On");
        //     sprintf(pub, "%s/Motor", mqtt);
        //     mqttClient.publish(pub, 1, true, msg);
        //     digitalWrite(motor[i], HIGH);
        // }
        // else{
        //     sprintf(msg, "Off");
        //     sprintf(pub, "%s/Motor", mqtt);
        //     mqttClient.publish(pub, 1, true, msg);
        //     digitalWrite(motor[i], LOW);
        // }
    }
    delay(30000);
}
