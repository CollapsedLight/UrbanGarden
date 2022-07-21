#pragma once

#include <driver/adc.h>

class Moisture
{

public:
    typedef enum Calibrate
    {
        MinValue,
        MaxValue
    } Calibrate;

    Moisture();
    ~Moisture();
    Moisture(adc1_channel_t channel);
    Moisture(adc2_channel_t channel);

    Moisture SetCalibration(Moisture::Calibrate point);
    Moisture SetMultiSampling(int samples);

    int GetCalibration(Moisture::Calibrate point);

    int GetData();
    double GetMoisture();
    double GetVoltage();

private:
    adc_channel_t _channel;
    int _pin;
    int _adc;

    int _samples = 10;
    int _minValue = 620;
    int _maxValue = 1200;

    int _data = 0;
};

Moisture::~Moisture(){

};

Moisture::Moisture(adc1_channel_t channel)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);

    _channel = (adc_channel_t)channel;
    _adc = 1;
};

Moisture::Moisture(adc2_channel_t channel)
{
    adc2_config_channel_atten(channel, ADC_ATTEN_DB_11);

    _channel = (adc_channel_t)channel;
    _adc = 2;
};

Moisture Moisture::SetCalibration(Moisture::Calibrate point)
{
    switch (point)
    {
    case MinValue:
        _minValue = GetData();
        break;
    case MaxValue:
        _maxValue = GetData() - _minValue;
        break;
    default:
        break;
    }

    return *this;
};

Moisture Moisture::SetMultiSampling(int samples)
{
    _samples = samples;
    return *this;
};

int Moisture::GetCalibration(Moisture::Calibrate point)
{
    switch (point)
    {
    case MinValue:
        return _minValue;
    case MaxValue:
        return _maxValue;
    }

    return 0;
};

double Moisture::GetVoltage()
{
    return (double)GetData() / (double)4095 * 3.3;
};

double Moisture::GetMoisture()
{
    int value = GetData() - _minValue;
    return (double)value / (double)_maxValue;
};

int Moisture::GetData()
{
    int value = 0;

    for (int i = 0; i < _samples; i++)
    {
        int data = -1;
        switch (_adc)
        {
        case 1:
            data = adc1_get_raw((adc1_channel_t)_channel);
            break;
        case 2:
            adc2_get_raw((adc2_channel_t)_channel, ADC_WIDTH_BIT_12, &data);
            break;
        }
        value += data;
    }

    return 4096 - (value / _samples);
};
