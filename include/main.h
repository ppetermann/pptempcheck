#ifndef PPTEMPCHECK_MAIN_H
#define PPTEMPCHECK_MAIN_H

#include <Arduino.h>
#include <cstdlib>
#include <U8g2lib.h>
#include <DHT.h>
#include <WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

#include "states.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

void renderTemp(float temp);
void blinkConnectionStatus(const char *service, const char *status);
float readTemp();
void publishTemp(float temp);
void publishDiscovery();

#endif //PPTEMPCHECK_MAIN_H
