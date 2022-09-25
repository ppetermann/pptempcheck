//
// Created by ppetermann on 24.09.22.
//

#ifndef PPTEMPCHECK_MQTTSTATUS_H
#define PPTEMPCHECK_MQTTSTATUS_H

#include "MQTTClient.h"

#include "StatusWrapper.h"
#include "states.h"
#include "config.h"

class MQTTStatus : public StatusWrapper {
public:
    explicit MQTTStatus(MQTTClient &mqttClient, void (*callback)());

    bool isConnected() override;

    void connect() override;

    bool wait() override;

    char *getServiceName() override;

    void success() override;

protected:
    MQTTClient &mqttClient;

    void (*successCallback)(){};
};


#endif //PPTEMPCHECK_MQTTSTATUS_H
