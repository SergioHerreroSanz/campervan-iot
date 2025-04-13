#pragma once

#include <Arduino.h>

// Definir la configuración del ventilador (usando PWM)
constexpr int PWM_PIN = 14;
constexpr int PWM_CHANNEL = 0;
constexpr int PWM_FREQUENCY = 1000;
constexpr int PWM_RESOLUTION = 8;

void initFan()
{
    pinMode(PWM_PIN, OUTPUT);
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);
    ledcWrite(0, pow(2, PWM_RESOLUTION - 1));
}

void gradualPWMChange(uint16_t targetValue)
{
    // Not gradual yet
    ledcWrite(PWM_CHANNEL, targetValue);
}