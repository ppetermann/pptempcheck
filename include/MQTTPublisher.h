//
// Created by ppetermann on 24.09.22.
//

#ifndef PPTEMPCHECK_MQTTPUBLISHER_H
#define PPTEMPCHECK_MQTTPUBLISHER_H

#include "MQTTClient.h"
#include "ArduinoJson.h"
#include "config.h"

class MQTTPublisher {
public:
    MQTTClient &mqttClient;

    explicit MQTTPublisher(MQTTClient &mqttClient);

    void publishDiscovery();

    void publishTemp(float temp, float hum);

private:
    StaticJsonDocument<200> jsonDoc;
};


#endif //PPTEMPCHECK_MQTTPUBLISHER_H
