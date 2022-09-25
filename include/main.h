#ifndef PPTEMPCHECK_MAIN_H
#define PPTEMPCHECK_MAIN_H

#include <Arduino.h>
#include <cstdlib>
#include <U8g2lib.h>
#include <DHT.h>
#include <WiFi.h>
#include <MQTTClient.h>
#include <MQTTPublisher.h>
#include <ArduinoJson.h>

#include "states.h"
#include "StatusWrapper/WifiStatus.h"
#include "StatusWrapper/MQTTStatus.h"


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

void blinkConnectionStatus(const char *service, const char *status);
bool ensureStatus(StatusWrapper &status);
float readTemperature();
float readHumidity();
void renderValues(float temp, float hum);


#endif //PPTEMPCHECK_MAIN_H
