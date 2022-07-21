#pragma once
#define INFLUXDB_CLIENT_DEBUG_ENABLE
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Logon/InfluxLogon.hpp>
#include <Logon/Logon.hpp>
#include <Moisture.hpp>
#include <WiFiMulti.h>

// Set timezone string according to
// https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET,
                            INFLUXDB_TOKEN, InfluxDbCloud2CACert);
WiFiMulti wifiMulti;
static int _influxID = 0;

class MoistureInfluxConnector
{

    Point _dataPoint = Point("Moisture");
    Moisture *_sensor;

    char _IdString[128];
    const char *_baseTopic = "%d";

public:
    MoistureInfluxConnector(Moisture *sensor);
    ~MoistureInfluxConnector();

    void SendDataPoint();
};

MoistureInfluxConnector::MoistureInfluxConnector(Moisture *sensor)
{
    sprintf(_IdString, _baseTopic, _influxID++); // "Moisture/{_ID}/"
    _dataPoint.addTag("ID", _IdString);
    _sensor = sensor;

    if (_influxID != 1)
        return;
    WiFi.mode(WIFI_STA);

    // timeSync(TZ_INFO, "0.de.pool.ntp.org", "time.nis.gov");
    // Check server connection
    Serial.println("Validate Connection ");
    if (influxClient.validateConnection())
    {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(influxClient.getServerUrl());
    }
    else
    {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(influxClient.getLastErrorMessage());
    }
};

MoistureInfluxConnector::~MoistureInfluxConnector(){

};

void MoistureInfluxConnector::SendDataPoint()
{
    _dataPoint.clearFields();
    _dataPoint.addField("Voltage", _sensor->GetVoltage());
    _dataPoint.addField("Value", _sensor->GetData());
    _dataPoint.addField("Moisture", _sensor->GetData());

    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(_dataPoint.toLineProtocol());

    // Write point
    if (!influxClient.writePoint(_dataPoint))
    {
        Serial.print("InfluxDB write failed: ");
        Serial.println(influxClient.getLastErrorMessage());
    }
}