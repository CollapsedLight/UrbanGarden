#pragma once

#include <string>
#include <MQTT/MQTTClient.hpp>
#include <Moisture.hpp>

class MoistureMQTT
{

public:
    MoistureMQTT();
    MoistureMQTT(Moisture *moisture, AsyncMqttClient *mqttClient);
    ~MoistureMQTT();

    void SendMoisture();
    void SendData();
    void SendVoltage();

private:
    const char *_baseTopic = "Plant/%d/";
    const char *_min = "Min";
    const char *_max = "Max";

    char _IdString[128];
    char _configTopic[128];

    AsyncMqttClient *_mqttClient;
    Moisture *_moisture;

    void OnMessage(const char *topic, const char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    void OnConnect(bool sessionPresent);

    void SendConfig(const char *valueName, int value);
    void SendDate(const char *valueName, double value);
    void SendDate(const char *valueName, int value);

    void SetTopic(char *str, const char *topic);
    void Publish(const char *topic, const char *msg);
};

static int _ID = 0;

MoistureMQTT::MoistureMQTT(Moisture *moisture, AsyncMqttClient *client)
{
    _mqttClient = client;
    _moisture = moisture;

    sprintf(_IdString, _baseTopic, _ID++); // "Plant/{_ID}/"

    strncpy(_configTopic, _IdString, 128);
    strncat(_configTopic, "Config", 128); // "Plant/{_ID}/Config"
    Serial.print("Subscribe to ");
    Serial.println(_configTopic);

    using namespace std::placeholders; // for _1, _2, _3...
    auto con = std::bind(&MoistureMQTT::OnConnect, this, _1);
    _mqttClient->onConnect(con);
}

MoistureMQTT::~MoistureMQTT() {}

void MoistureMQTT::OnConnect(bool sessionPresent)
{
    using namespace std::placeholders; // for _1, _2, _3...
    _mqttClient->subscribe(_configTopic, 1);
    auto rcv = std::bind(&MoistureMQTT::OnMessage, this, _1, _2, _3, _4, _5, _6);
    _mqttClient->onMessage(rcv);

    SendConfig(_min, _moisture->GetCalibration(Moisture::Calibrate::MinValue));
    SendConfig(_max, _moisture->GetCalibration(Moisture::Calibrate::MaxValue));
}

void MoistureMQTT::OnMessage(const char *topic, const char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    if (strncmp(topic, _configTopic, 128) != 0)
        return;

    Serial.println();
    Serial.print("Payload Received on ");
    Serial.println(_configTopic);
    Serial.print("Length = ");
    Serial.print(len);
    Serial.print(" Payload = ");
    Serial.println(payload);

    if (strcmp("SetMin", payload) == 0)
    {
        Serial.println("Publish SetMin");
        _moisture->SetCalibration(Moisture::Calibrate::MinValue);
    }
    if (strcmp("SetMax", payload) == 0)
    {
        Serial.println("Publish SetMax");
        _moisture->SetCalibration(Moisture::Calibrate::MaxValue);
    }
    if (strcmp("GetConfig", payload) == 0)
    {
        Serial.println("Publish GetConfig");
        SendConfig(_min, _moisture->GetCalibration(Moisture::Calibrate::MinValue));
        SendConfig(_max, _moisture->GetCalibration(Moisture::Calibrate::MaxValue));
    };
    if (strcmp("GetData", payload) == 0)
    {
        Serial.println("Publish GetData");
        SendMoisture();
        SendData();
    };
}

void MoistureMQTT::SendMoisture()
{
    SendDate((char *)"Moisture", _moisture->GetMoisture());
}

void MoistureMQTT::SendData()
{
    SendDate((char *)"RawData", _moisture->GetData());
}

void MoistureMQTT::SendVoltage()
{
    SendDate((char *)"Voltage", _moisture->GetVoltage());
}

void MoistureMQTT::SetTopic(char *str, const char *topic)
{
    strncpy(str, _IdString, 128);
    strncat(str, topic, 128);
}

void MoistureMQTT::Publish(const char *topic, const char *msg)
{
    Serial.print(topic);
    Serial.print(" = ");
    Serial.println(msg);

    _mqttClient->publish(topic, 1, true, msg);
}

void MoistureMQTT::SendDate(const char *valueName, int value)
{
    char msg[128];
    char topic[128];

    //SetTopic(topic, "Data/");
    strncat(topic, valueName, 128);

    sprintf(msg, "%d", value);

    Publish(topic, msg);
}

void MoistureMQTT::SendDate(const char *valueName, double value)
{
    char msg[128];
    char topic[128];

    //SetTopic(topic, "Data/");
    strncat(topic, valueName, 128);

    sprintf(msg, "%f", value);

    Publish(topic, msg);
}

void MoistureMQTT::SendConfig(const char *valueName, int value)
{
    char msg[128];
    char topic[128];

    SetTopic(topic, "Config/");
    strncat(topic, valueName, 128);

    sprintf(msg, "%d", value);

    Publish(topic, msg);
}
