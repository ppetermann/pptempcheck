#include "StatusWrapper/MQTTStatus.h"

bool MQTTStatus::isConnected() {
    return mqttClient.connected();
}

void MQTTStatus::connect() {
  mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);
}

bool MQTTStatus::wait() {
    return !mqttClient.connected();
}

char *MQTTStatus::getServiceName() {
    return (char *) CONN_SERVICE_MQTT;
}


void MQTTStatus::success() {
    successCallback();
}

MQTTStatus::MQTTStatus(MQTTClient &mqttClient, void (*callback)()) : mqttClient(mqttClient) {
    successCallback = callback;
}

